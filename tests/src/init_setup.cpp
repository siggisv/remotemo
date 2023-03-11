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
  std::list<tr_exp>& exps = init.exps;
  Test_seqs& seqs = init.seqs;

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
  std::list<tr_exp>& exps = init.exps;
  Test_seqs& seqs = init.seqs;

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
  std::list<tr_exp>& exps = init.exps;
  Test_seqs& seqs = init.seqs;

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
    init.set_from_config(conf);

    init.attempt_init(true);
    init.attempt_set_hint(set_hint);
    if (conf.res.win == nullptr) {
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
