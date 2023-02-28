#include <string>
#include <iomanip>
#include <list>

#include "remotemo/remotemo.hpp"

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

#include "mock_sdl_all.hpp"

using trompeloeil::eq;
using trompeloeil::re;
using tr_seq = trompeloeil::sequence;
using tr_exp = std::unique_ptr<trompeloeil::expectation>;
using trompeloeil::_;


Dummy_object dummies[] {
    {{"Dummy conf window"}, 0},
    {{"Dummy new window"}, 1},
    {{"Dummy conf renderer"}, 2},
    {{"Dummy new renderer"}, 3},
    {{"Dummy conf font bitmap"}, 4},
    {{"Dummy new font bitmap"}, 5},
    {{"Dummy conf background"}, 6},
    {{"Dummy new background"}, 7},
    {{"Dummy new text area"}, 8},
};
SDL_Window* d_conf_win = reinterpret_cast<SDL_Window*>(&dummies[0]);
SDL_Window* d_new_win = reinterpret_cast<SDL_Window*>(&dummies[1]);
SDL_Renderer* d_conf_render = reinterpret_cast<SDL_Renderer*>(&dummies[2]);
SDL_Renderer* d_new_render = reinterpret_cast<SDL_Renderer*>(&dummies[3]);
SDL_Texture* d_conf_font_bitmap = reinterpret_cast<SDL_Texture*>(&dummies[4]);
SDL_Texture* d_new_font_bitmap = reinterpret_cast<SDL_Texture*>(&dummies[5]);
SDL_Texture* d_conf_backgr = reinterpret_cast<SDL_Texture*>(&dummies[6]);
SDL_Texture* d_new_backgr = reinterpret_cast<SDL_Texture*>(&dummies[7]);
SDL_Texture* d_new_text_area = reinterpret_cast<SDL_Texture*>(&dummies[8]);

struct Test_create_resources {
  SDL_Window* conf_win;
  SDL_Renderer* conf_render;
  SDL_Texture* conf_backgr;
  SDL_Texture* conf_font;
  bool conf_backgr_is_valid;
  bool conf_font_is_valid;
};

struct Test_seqs {
  tr_seq main;
  tr_seq backgr;
  tr_seq font;
};

// Helper functions:

void check_cleanup(std::list<tr_exp>* exps, const Test_create_resources& res,
    Test_seqs* seqs)
{
  if (res.conf_font != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyTexture(res.conf_font))
            .IN_SEQUENCE(seqs->font));
  }
  if (res.conf_backgr != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyTexture(res.conf_backgr))
            .IN_SEQUENCE(seqs->backgr));
  }
  if (res.conf_render != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyRenderer(res.conf_render))
            .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  }
  if (res.conf_win != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyWindow(res.conf_win))
            .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  }
  exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_Quit())
                      .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
}

void check_has_renderer(std::list<tr_exp>* exps,
    const Test_create_resources& res, Test_seqs* seqs)
{
  exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_GetRenderer(res.conf_win))
                      .TIMES(AT_LEAST(1))
                      .RETURN(res.conf_render)
                      .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
}

void check_renderer_settings(std::list<tr_exp>* exps,
    const Test_create_resources& res, Test_seqs* seqs, Uint32 render_flag,
    int ret_val)
{
  if (ret_val == 0) {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL,
        mock_GetRendererInfo(res.conf_render, ANY(SDL_RendererInfo*)))
                        .SIDE_EFFECT(_2->flags = render_flag)
                        .RETURN(0)
                        .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  } else {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL,
        mock_GetRendererInfo(res.conf_render, ANY(SDL_RendererInfo*)))
                        .RETURN(ret_val)
                        .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  }
}

bool try_running_create(bool do_cleanup_all, const Test_create_resources& res)
{
  remotemo::Config config;
  config.cleanup_all(do_cleanup_all)
      .the_window(res.conf_win)
      .background(res.conf_backgr)
      .font_bitmap(res.conf_font);
  auto t = remotemo::create(config);
  // This dummy function is here so we can check if cleanup happens before
  // `t` goes out of scope or after:
  dummy_t.func();
  return t.has_value();
}


// Test cases:

TEST_CASE("create() - config resources invalid - invalid window")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 0;
  auto do_cleanup_all = GENERATE(false, true);
  auto res = GENERATE(values<Test_create_resources>({//
      {d_conf_win, nullptr, nullptr, nullptr},
      {d_conf_win, nullptr, nullptr, d_conf_font_bitmap},
      {d_conf_win, nullptr, d_conf_backgr, nullptr},
      {d_conf_win, nullptr, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << "Textures in conf: "
                  << (res.conf_backgr == nullptr ? "" : "background ")
                  << (res.conf_font == nullptr ? "" : "font_bitmap "))
  {
    Test_seqs seqs;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(res.conf_win))
        .RETURN(conf_winID)
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);
    ALLOW_CALL(mock_SDL, mock_GetRenderer(res.conf_win))
        .RETURN(res.conf_render);
    if (do_cleanup_all) {
      check_cleanup(&exps, res, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, res);
    REQUIRE(success == false);
  }
}

TEST_CASE("create() - config resources invalid - texture(s) but no renderer")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 1;
  auto do_cleanup_all = GENERATE(false, true);
  auto res = GENERATE(values<Test_create_resources>({//
      {nullptr, nullptr, nullptr, d_conf_font_bitmap},
      {nullptr, nullptr, d_conf_backgr, nullptr},
      {nullptr, nullptr, d_conf_backgr, d_conf_font_bitmap},
      {d_conf_win, nullptr, nullptr, d_conf_font_bitmap},
      {d_conf_win, nullptr, d_conf_backgr, nullptr},
      {d_conf_win, nullptr, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << "Textures in conf: "
                  << (res.conf_backgr == nullptr ? "" : "background ")
                  << (res.conf_font == nullptr ? "" : "font_bitmap "))
  {
    Test_seqs seqs;

    if (res.conf_win != nullptr) {
      exps.push_back(
          NAMED_REQUIRE_CALL(mock_SDL, mock_GetWindowID(res.conf_win))
              .RETURN(conf_winID));
      check_has_renderer(&exps, res, &seqs);
    }
    if (do_cleanup_all) {
      check_cleanup(&exps, res, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, res);
    REQUIRE(success == false);
  }
}

TEST_CASE("create() - config resources invalid - can't get renderer settings")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 1;
  auto do_cleanup_all = GENERATE(false, true);
  auto res = GENERATE(values<Test_create_resources>({//
      {d_conf_win, d_conf_render, nullptr, nullptr},
      {d_conf_win, d_conf_render, nullptr, d_conf_font_bitmap},
      {d_conf_win, d_conf_render, d_conf_backgr, nullptr},
      {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << "Textures in conf: "
                  << (res.conf_backgr == nullptr ? "" : "background ")
                  << (res.conf_font == nullptr ? "" : "font_bitmap "))
  {
    Test_seqs seqs;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(res.conf_win)).RETURN(conf_winID);
    check_has_renderer(&exps, res, &seqs);
    check_renderer_settings(&exps, res, &seqs, 0, -1);

    if (do_cleanup_all) {
      check_cleanup(&exps, res, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, res);
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
  auto res = GENERATE(values<Test_create_resources>({//
      {d_conf_win, d_conf_render, nullptr, nullptr},
      {d_conf_win, d_conf_render, nullptr, d_conf_font_bitmap},
      {d_conf_win, d_conf_render, d_conf_backgr, nullptr},
      {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << "Textures in conf: "
                  << (res.conf_backgr == nullptr ? "" : "background ")
                  << (res.conf_font == nullptr ? "" : "font_bitmap "))
  {
    Test_seqs seqs;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(res.conf_win)).RETURN(conf_winID);
    check_has_renderer(&exps, res, &seqs);
    check_renderer_settings(&exps, res, &seqs, render_flag, 0);

    if (do_cleanup_all) {
      check_cleanup(&exps, res, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, res);
    REQUIRE(success == false);
  }
}

TEST_CASE(
    "create() - config resources invalid - texture has incorrect renderer")
{
  std::list<tr_exp> exps {};
  Uint32 conf_winID = 1;
  auto do_cleanup_all = GENERATE(false, true);
  auto res = GENERATE(values<Test_create_resources>({//
      {d_conf_win, d_conf_render, nullptr, d_conf_font_bitmap, false, false},
      {d_conf_win, d_conf_render, d_conf_backgr, nullptr, false, false},
      {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap, false,
          false},
      {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap, false,
          true},
      {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap, true,
          false}}));
  DYNAMIC_SECTION("Checking cleanup...\nShould clean up all: "
                  << std::boolalpha << do_cleanup_all << "\n"
                  << std::noboolalpha << "Textures in conf: "
                  << (res.conf_backgr == nullptr ? "" : "background ")
                  << (res.conf_font == nullptr ? "" : "font_bitmap "))
  {
    Test_seqs seqs;
    tr_exp exp_backgr, exp_font;

    REQUIRE_CALL(mock_SDL, mock_GetWindowID(res.conf_win)).RETURN(conf_winID);
    check_has_renderer(&exps, res, &seqs);
    check_renderer_settings(&exps, res, &seqs, SDL_RENDERER_TARGETTEXTURE, 0);

    // If the other texture is invalid (and not nullpt) then it might fail its
    // check before this texture is checked. Therefore the minimum times this
    // texture should be checked would in that instance be zero:
    int backgr_min =
        (res.conf_font != nullptr && !res.conf_font_is_valid) ? 0 : 1;
    int font_min =
        (res.conf_backgr != nullptr && !res.conf_backgr_is_valid) ? 0 : 1;

    if (res.conf_backgr != nullptr) {
      if (backgr_min == 1) {
        exp_backgr = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(res.conf_render, res.conf_backgr, _, _))
                         .IN_SEQUENCE(seqs.backgr)
                         .RETURN(res.conf_backgr_is_valid ? 0 : -1);
      } else {
        exp_backgr = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(res.conf_render, res.conf_backgr, _, _))
                         .IN_SEQUENCE(seqs.backgr)
                         .RETURN(res.conf_backgr_is_valid ? 0 : -1)
                         .TIMES(0, 1);
      }
    }
    if (res.conf_font != nullptr) {
      if (font_min == 1) {
        exp_font = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(res.conf_render, res.conf_font, _, _))
                       .IN_SEQUENCE(seqs.font)
                       .RETURN(res.conf_font_is_valid ? 0 : -1);
      } else {
        exp_font = NAMED_REQUIRE_CALL(
            mock_SDL, mock_RenderCopy(res.conf_render, res.conf_font, _, _))
                       .IN_SEQUENCE(seqs.font)
                       .RETURN(res.conf_font_is_valid ? 0 : -1)
                       .TIMES(0, 1);
      }
    }

    if (do_cleanup_all) {
      check_cleanup(&exps, res, &seqs);
    }
    REQUIRE_CALL(dummy_t, func())
        .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr);

    bool success = try_running_create(do_cleanup_all, res);
    REQUIRE(success == false);

    // If both textures are invalid (and non-nullptr) then both have the
    // minimum-number-of-checks set to zero. Also only one of them should have
    // been checked (as it should have failed that check and thus prevented
    // the other one from being checked):
    if (backgr_min == 0 && font_min == 0) {
      REQUIRE(exp_backgr->is_saturated() != exp_font->is_saturated());
    }
  }
}
