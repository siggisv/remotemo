#include <string>
#include <iomanip>
#include <vector>

#include "remotemo/remotemo.hpp"

#include "init.hpp"

using trompeloeil::_;
using trompeloeil::eq;
using trompeloeil::re;

// Test cases:

TEST_CASE("create() - config resources invalid - invalid window")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 0;
  auto do_cleanup_all = GENERATE(false, true);
  auto conf = GENERATE(values<Conf_resources>({//
      {d_conf_win, nullptr, nullptr, nullptr},
      {d_conf_win, nullptr, nullptr, d_conf_font_bitmap},
      {d_conf_win, nullptr, d_conf_backgr, nullptr},
      {d_conf_win, nullptr, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << conf.list_textures())
  {
    Test_seqs seqs;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(conf.res.win))
        .RETURN(conf_winID)
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);
    ALLOW_CALL(mock_SDL, mock_GetRenderer(conf.res.win))
        .RETURN(conf.res.render);
    if (do_cleanup_all) {
      conf.check_cleanup(&exps, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
  }
}

TEST_CASE("create() - config resources invalid - texture(s) but no renderer")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 1;
  auto do_cleanup_all = GENERATE(false, true);
  auto conf = GENERATE(values<Conf_resources>({//
      {nullptr, nullptr, nullptr, d_conf_font_bitmap},
      {nullptr, nullptr, d_conf_backgr, nullptr},
      {nullptr, nullptr, d_conf_backgr, d_conf_font_bitmap},
      {d_conf_win, nullptr, nullptr, d_conf_font_bitmap},
      {d_conf_win, nullptr, d_conf_backgr, nullptr},
      {d_conf_win, nullptr, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << conf.list_textures())
  {
    Test_seqs seqs;

    if (conf.res.win != nullptr) {
      exps.push_back(
          NAMED_REQUIRE_CALL(mock_SDL, mock_GetWindowID(conf.res.win))
              .RETURN(conf_winID));
      conf.check_win_has_renderer(&exps, &seqs);
    }
    if (do_cleanup_all) {
      conf.check_cleanup(&exps, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
  }
}

TEST_CASE("create() - config resources invalid - can't get renderer settings")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 1;
  auto do_cleanup_all = GENERATE(false, true);
  auto conf = GENERATE(values<Conf_resources>({//
      {d_conf_win, d_conf_render, nullptr, nullptr},
      {d_conf_win, d_conf_render, nullptr, d_conf_font_bitmap},
      {d_conf_win, d_conf_render, d_conf_backgr, nullptr},
      {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << conf.list_textures())
  {
    Test_seqs seqs;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(conf.res.win)).RETURN(conf_winID);
    conf.check_win_has_renderer(&exps, &seqs);
    check_renderer_settings(&exps, conf.res.render, &seqs, 0, -1);

    if (do_cleanup_all) {
      conf.check_cleanup(&exps, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
  }
}

TEST_CASE("create() - config resources invalid - incorrect renderer settings")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 1;
  auto do_cleanup_all = GENERATE(false, true);
  auto render_flag = GENERATE(0, SDL_RENDERER_SOFTWARE,
      SDL_RENDERER_ACCELERATED, SDL_RENDERER_PRESENTVSYNC,
      (SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC),
      (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
  auto conf = GENERATE(values<Conf_resources>({//
      {d_conf_win, d_conf_render, nullptr, nullptr},
      {d_conf_win, d_conf_render, nullptr, d_conf_font_bitmap},
      {d_conf_win, d_conf_render, d_conf_backgr, nullptr},
      {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << conf.list_textures())
  {
    Test_seqs seqs;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(conf.res.win)).RETURN(conf_winID);
    conf.check_win_has_renderer(&exps, &seqs);
    check_renderer_settings(&exps, conf.res.render, &seqs, render_flag, 0);

    if (do_cleanup_all) {
      conf.check_cleanup(&exps, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);
  }
}

TEST_CASE(
    "create() - config resources invalid - texture has incorrect renderer")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 1;
  auto do_cleanup_all = GENERATE(false, true);
  auto conf = GENERATE(values<Conf_resources>({//
      {{d_conf_win, d_conf_render, nullptr, d_conf_font_bitmap}, false, false},
      {{d_conf_win, d_conf_render, d_conf_backgr, nullptr}, false, false},
      {{d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}, false,
          false},
      {{d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}, false,
          true},
      {{d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}, true,
          false}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << conf.list_textures())
  {
    Test_seqs seqs;
    tr_exp exp_backgr, exp_font;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(conf.res.win)).RETURN(conf_winID);
    conf.check_win_has_renderer(&exps, &seqs);
    check_renderer_settings(
        &exps, conf.res.render, &seqs, SDL_RENDERER_TARGETTEXTURE, 0);

    // If the other texture is invalid (and not nullpt) then it might fail its
    // check before this texture is checked. Therefore the minimum times this
    // texture should be checked would in that instance be zero:
    int backgr_min =
        (conf.res.font != nullptr && !conf.font_is_valid) ? 0 : 1;
    int font_min =
        (conf.res.backgr != nullptr && !conf.backgr_is_valid) ? 0 : 1;

    if (conf.res.backgr != nullptr) {
      if (backgr_min == 1) {
        exp_backgr = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(conf.res.render, conf.res.backgr, _, _))
                         .IN_SEQUENCE(seqs.backgr)
                         .RETURN(conf.backgr_is_valid ? 0 : -1);
      } else {
        exp_backgr = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(conf.res.render, conf.res.backgr, _, _))
                         .IN_SEQUENCE(seqs.backgr)
                         .RETURN(conf.backgr_is_valid ? 0 : -1)
                         .TIMES(0, 1);
      }
    }
    if (conf.res.font != nullptr) {
      if (font_min == 1) {
        exp_font = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(conf.res.render, conf.res.font, _, _))
                       .IN_SEQUENCE(seqs.font)
                       .RETURN(conf.font_is_valid ? 0 : -1);
      } else {
        exp_font = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(conf.res.render, conf.res.font, _, _))
                       .IN_SEQUENCE(seqs.font)
                       .RETURN(conf.font_is_valid ? 0 : -1)
                       .TIMES(0, 1);
      }
    }

    if (do_cleanup_all) {
      conf.check_cleanup(&exps, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, conf);
    REQUIRE(success == false);

    // If a non-nullptr texture is invalid, then the other one does not need
    // to be checked. But if both textures are invalid (and non-nullptr) then
    // both have the minimum-number-of-checks set to zero.
    // In that case, at least one of them must still be checked:
    if (backgr_min == 0 && font_min == 0) {
      REQUIRE((exp_backgr->is_saturated() || exp_font->is_saturated()));
    }
  }
}
