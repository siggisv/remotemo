#include <string>
#include <iomanip>
#include <iterator>
#include <vector>

#include "remotemo/remotemo.hpp"

#include "init.hpp"

Uint32 init_flags = 0, quit_flags = 0;

// Test cases:

TEST_CASE("create() - config resources ARE valid (but SDL_Init() fails):")
{
  Init_status init;
  auto& exps = init.exps;
  auto& seqs = init.seqs;

  auto do_cleanup_all = GENERATE(false, true);
  auto conf = GENERATE_REF(from_range(valid_conf_res));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << conf.describe())
  {
    conf.all_checks_succeeds(&exps, &seqs);
    init.attempt_init(false);

    if (do_cleanup_all) {
      conf.expected_cleanup(&exps, &seqs);
    }
    require_init_has_ended(&exps, &seqs);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
  }
}

TEST_CASE("create() - resourceless config - create window fails:")
{
  Init_status init;
  auto& exps = init.exps;
  auto& seqs = init.seqs;

  auto do_cleanup_all = GENERATE(false, true);
  auto conf = valid_conf_res[0];
  auto set_hint = GENERATE(false, true);
  DYNAMIC_SECTION(
      "... SDL_Init() succeeds, ...\n"
      << "... SDL_SetHint() "
      << (set_hint ? "succeeds, ..."
                   : "fails! (but isn't fatal so we continue), ...")
      << "\n"
      << "... SDL_CreateWindow() fails, ...\n"
      << "Checking cleanup...\nShould clean up all: " << std::boolalpha
      << do_cleanup_all << "\n"
      << std::noboolalpha << conf.describe())
  {
    conf.all_checks_succeeds(&exps, &seqs);

    init.attempt_init(true);
    init.attempt_set_hint(set_hint);
    init.attempt_create_window(false);

    if (do_cleanup_all) {
      conf.expected_cleanup(&exps, &seqs);
    } else {
      exps.push_back(
          NAMED_REQUIRE_CALL(mock_SDL, mock_QuitSubSystem(ANY(Uint32)))
              .LR_SIDE_EFFECT(init.quit_flags = _1)
              .IN_SEQUENCE(seqs.main));
    }
    require_init_has_ended(&exps, &seqs);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
    if (!do_cleanup_all) {
      REQUIRE(init.init_flags == init.quit_flags);
    }
  }
}

TEST_CASE("create() - valid rendererless config - create renderer fails:")
{
  Init_status init;
  auto& exps = init.exps;
  auto& seqs = init.seqs;

  auto do_cleanup_all = GENERATE(false, true);
  init.do_cleanup_all = do_cleanup_all;
  auto conf = GENERATE_REF(valid_conf_res[0], valid_conf_res[1]);
  auto set_hint = GENERATE(false, true);
  DYNAMIC_SECTION(
      "... SDL_Init() succeeds, ...\n"
      << "... SDL_SetHint() "
      << (set_hint ? "succeeds, ..." : "fails! (but isn't fatal), ...")
      << "\n"
      << "... SDL_CreateWindow() "
      << (conf.res.win != nullptr ? "skipped" : "succeeds") << ", ...\n"
      << "... SDL_CreateRenderer() fails!\n"
      << "Checking cleanup...\nShould clean up all: " << std::boolalpha
      << do_cleanup_all << "\n"
      << std::noboolalpha << conf.describe())
  {
    conf.all_checks_succeeds(&exps, &seqs);
    init.set_res_from_config(conf);

    init.attempt_init(true);
    init.attempt_set_hint(set_hint);
    if (init.ready_res.win == nullptr) {
      init.attempt_create_window(true);
    }
    init.attempt_create_renderer(false);

    init.expected_cleanup();
    require_init_has_ended(&exps, &seqs);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
    if (!do_cleanup_all) {
      REQUIRE(init.init_flags == init.quit_flags);
    }
  }
}

TEST_CASE("create() - valid config - setup of textures fails:")
{
  char* d_bpath = const_cast<char*>(&d_basepath[0]);
  const std::vector<Texture_results> all_texture_results {{
      {nullptr, nullptr, nullptr, nullptr},
      {nullptr, nullptr, nullptr, d_new_text_area},
      {d_bpath, nullptr, nullptr, nullptr},
      {d_bpath, nullptr, nullptr, d_new_text_area},
      {d_bpath, d_new_font_bitmap, nullptr, nullptr},
      {d_bpath, d_new_font_bitmap, nullptr, d_new_text_area},
      {d_bpath, nullptr, d_new_backgr, nullptr},
      {d_bpath, nullptr, d_new_backgr, d_new_text_area},
      {d_bpath, d_new_font_bitmap, d_new_backgr, nullptr},
  }};

  auto do_cleanup_all = GENERATE(false, true);
  auto conf = GENERATE_REF(from_range(valid_conf_res));
  auto set_hint = GENERATE(false, true);
  auto textures = GENERATE_REF(filter(
      [=](Texture_results t_res) {
        if (conf.res.backgr != nullptr && t_res.basepath != nullptr &&
            t_res.backgr == nullptr) {
          return false;
        }
        if (conf.res.font != nullptr && t_res.basepath != nullptr &&
            t_res.font == nullptr) {
          return false;
        }
        if (conf.res.font != nullptr && conf.res.backgr != nullptr &&
            t_res.basepath == nullptr) {
          return false;
        }
        return true;
      },
      from_range(all_texture_results)));
  DYNAMIC_SECTION(
      "... SDL_Init() succeeds, ...\n"
      << "... SDL_SetHint() "
      << (set_hint ? "succeeds, ..." : "fails! (but isn't fatal), ...")
      << "\n"
      << "... SDL_CreateWindow() "
      << (conf.res.win != nullptr ? "skipped" : "succeeds") << ", ...\n"
      << "... SDL_CreateRenderer() "
      << (conf.res.render != nullptr ? "skipped" : "succeeds") << ", ...\n"
      << "Texture genereation:\n"
      << textures.describe() << "Checking cleanup...\n"
      << "Should clean up all: " << std::boolalpha << do_cleanup_all
      << std::noboolalpha << "\n"
      << conf.describe())
  {
    Init_status init {};
    auto& exps = init.exps;
    auto& seqs = init.seqs;
    init.do_cleanup_all = do_cleanup_all;

    conf.all_checks_succeeds(&exps, &seqs);
    init.set_res_from_config(conf);

    init.attempt_init(true);
    init.attempt_set_hint(set_hint);
    if (init.ready_res.win == nullptr) {
      init.attempt_create_window(true);
    }
    if (init.ready_res.render == nullptr) {
      init.attempt_create_renderer(true);
    }

    init.attempt_setup_textures(textures);

    init.expected_cleanup();
    require_init_has_ended(&exps, &seqs);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
    if (!do_cleanup_all) {
      REQUIRE(init.init_flags == init.quit_flags);
    }
    REQUIRE(init.check_a_texture_failed());
    init.check_texture_cleanup();
  }
}

TEST_CASE("create() - all succeeds:")
{
  auto do_cleanup_all = GENERATE(false, true);
  auto conf = GENERATE_REF(from_range(valid_conf_res));
  auto set_hint = GENERATE(false, true);

  char* d_bpath = const_cast<char*>(&d_basepath[0]);
  Texture_results textures {
      d_bpath, d_new_font_bitmap, d_new_backgr, d_new_text_area};
  DYNAMIC_SECTION(
      "... SDL_Init() succeeds, ...\n"
      << "... SDL_SetHint() "
      << (set_hint ? "succeeds, ..." : "fails! (but isn't fatal), ...")
      << "\n"
      << "... SDL_CreateWindow() "
      << (conf.res.win != nullptr ? "skipped" : "succeeds") << ", ...\n"
      << "... SDL_CreateRenderer() "
      << (conf.res.render != nullptr ? "skipped" : "succeeds") << ", ...\n"
      << "Texture genereation:\n"
      << textures.describe() << "Checking cleanup...\n"
      << "Should clean up all: " << std::boolalpha << do_cleanup_all
      << std::noboolalpha << "\n"
      << conf.describe())
  {
    Init_status init {};
    auto& exps = init.exps;
    auto& seqs = init.seqs;
    init.do_cleanup_all = do_cleanup_all;

    conf.all_checks_succeeds(&exps, &seqs);
    init.set_res_from_config(conf);

    init.attempt_init(true);
    init.attempt_set_hint(set_hint);
    if (init.ready_res.win == nullptr) {
      init.attempt_create_window(true);
    }
    if (init.ready_res.render == nullptr) {
      init.attempt_create_renderer(true);
    }

    init.attempt_setup_textures(textures);

    require_init_has_ended(&exps, &seqs);
    init.expected_cleanup();

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success);
    if (!do_cleanup_all) {
      REQUIRE(init.init_flags == init.quit_flags);
    }
    REQUIRE(init.check_a_texture_failed() == false);
    init.check_texture_cleanup();
  }
}

TEST_CASE("create() succeeds - test if config is used to create the window:")
{
  auto do_cleanup_all = GENERATE(false, true);
  auto conf_res = valid_conf_res[0];
  auto set_hint = GENERATE(false, true);

  // NOTE All those settings should be passed as they are without validation
  // to SDL_CreateWindow(), even if they don't make sence (e.g. negative
  // sizes). SDL_CreateWindow() will then return an error if the settings are
  // unusable. The handling of those errors is tested in a different test.
  const std::vector<Win_conf> win_config_settings {{
      {"Small size", {{10, 10}}, {{0, 0}}, false, false},
      {std::nullopt, std::nullopt, std::nullopt, false, true},
      {"Test", std::nullopt, {{-4000, -4000}}, std::nullopt, std::nullopt},
      {std::nullopt, {{-40, -40}}, std::nullopt, std::nullopt, true},
      {"This should be enough", std::nullopt, std::nullopt, true, true},
  }};
  auto win_conf = GENERATE_REF(from_range(win_config_settings));

  char* d_bpath = const_cast<char*>(&d_basepath[0]);
  Texture_results textures {
      d_bpath, d_new_font_bitmap, d_new_backgr, d_new_text_area};
  DYNAMIC_SECTION(
      "... SDL_Init() succeeds, ...\n"
      << "... SDL_SetHint() "
      << (set_hint ? "succeeds, ..." : "fails! (but isn't fatal), ...")
      << "\n"
      << "... SDL_CreateWindow() with settings from config:\n"
      << win_conf.describe() << "...\n"
      << "... SDL_CreateRenderer() succeeds, ...\n"
      << "... Texture generation succeeds.\n"
      << "Should clean up all: " << std::boolalpha << do_cleanup_all
      << std::noboolalpha << "\n")
  {
    Init_status init {};
    auto& exps = init.exps;
    auto& seqs = init.seqs;
    init.do_cleanup_all = do_cleanup_all;

    conf_res.all_checks_succeeds(&exps, &seqs);
    init.set_res_from_config(conf_res);

    init.attempt_init(true);
    init.attempt_set_hint(set_hint);
    init.attempt_create_window(true, win_conf);
    init.attempt_create_renderer(true);
    init.attempt_setup_textures(textures);

    require_init_has_ended(&exps, &seqs);
    init.expected_cleanup();

    bool success = try_running_create(do_cleanup_all, conf_res, win_conf);
    REQUIRE(success);
    if (!do_cleanup_all) {
      REQUIRE(init.init_flags == init.quit_flags);
    }
    REQUIRE(init.check_a_texture_failed() == false);
    init.check_texture_cleanup();
  }
}
