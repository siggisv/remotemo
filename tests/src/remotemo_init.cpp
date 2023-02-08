#include "remotemo/remotemo.hpp"

#include <string>
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

using trompeloeil::eq;
using trompeloeil::re;


class Mock_SDL {
  MAKE_MOCK1(mock_Init, int(Uint32));
  MAKE_MOCK0(mock_Quit, void());
  MAKE_MOCK1(mock_QuitSubSystem, void(Uint32));
  MAKE_MOCK2(mock_SetHint, SDL_bool(const char*, const char*));
  MAKE_MOCK6(mock_CreateWindow,
      SDL_Window*(const char*, int, int, int, int, Uint32));
  MAKE_MOCK1(mock_DestroyWindow, void(SDL_Window*));
};
Mock_SDL mock_SDL;


extern "C" {
int SDL_Init(Uint32 flags)
{
  return mock_SDL.mock_Init(flags);
}
void SDL_Quit()
{
  mock_SDL.mock_Quit();
}
void SDL_QuitSubSystem(Uint32 flags)
{
  mock_SDL.mock_QuitSubSystem(flags);
}
SDL_bool SDL_SetHint(const char* name, const char* value)
{
  return mock_SDL.mock_SetHint(name, value);
}
SDL_Window* SDL_CreateWindow(
    const char* title, int x, int y, int w, int h, Uint32 flags)
{
  return mock_SDL.mock_CreateWindow(title, x, y, w, h, flags);
}
void SDL_DestroyWindow(SDL_Window* window)
{
  mock_SDL.mock_DestroyWindow(window);
}
} // extern "C"


class Dummy_test {
  MAKE_MOCK0(func, void());
};
Dummy_test dummy_t;

struct Dummy_window {
  char dummy_text[2000];
  int dummy_int;
};
Dummy_window dummy_w {{"Dummy window."}, 5};
SDL_Window* dummy_window = reinterpret_cast<SDL_Window*>(&dummy_w);

Uint32 init_flags = 0, quit_flags = 0;
constexpr char regex_hint_name[] = "^" SDL_HINT_RENDER_SCALE_QUALITY "$";
constexpr char separator[] = "====================================="
                             "====================================";

enum Resources {
  Res_Init = 0,
  Res_SetHint,
  Res_CreateWin,
  Res_MAX_NUM,
};

TEST_CASE("Test create() ...")
{
  trompeloeil::sequence main_seq;
  std::unique_ptr<trompeloeil::expectation> setups[Res_MAX_NUM];
  std::unique_ptr<trompeloeil::expectation> cleanups[Res_MAX_NUM];
  std::unique_ptr<trompeloeil::expectation> dummy_exp;
  bool should_succeed = true;

  auto do_cleanup_all = GENERATE(true, false);
  UNSCOPED_INFO(separator);
  UNSCOPED_INFO("... with config.cleanup_all("
                << (do_cleanup_all ? "true) ..." : "false) ..."));

  auto init_ret = GENERATE(-1, 0);
  UNSCOPED_INFO(
      "... SDL_Init() " << (init_ret == 0 ? "succeeds, ..." : "fails!"));
  setups[Res_Init] = NAMED_REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))
                         .SIDE_EFFECT(init_flags = _1)
                         .RETURN(init_ret)
                         .IN_SEQUENCE(main_seq);
  if (init_ret == -1) {
    should_succeed = false;
    UNSCOPED_INFO(separator);
  } else {
    auto set_hint_ret = GENERATE(SDL_TRUE, SDL_FALSE);
    UNSCOPED_INFO(
        "... SDL_SetHint() "
        << (set_hint_ret == SDL_TRUE
                   ? "succeeds, ..."
                   : "fails! (but isn't fatal so we continue), ..."));
    setups[Res_SetHint] = NAMED_REQUIRE_CALL(
        mock_SDL, mock_SetHint(re(regex_hint_name), re("^linear$")))
                              .RETURN(set_hint_ret)
                              .IN_SEQUENCE(main_seq);
  }

  if (should_succeed) {
    auto create_win_ret =
        GENERATE(static_cast<SDL_Window*>(nullptr), dummy_window);
    UNSCOPED_INFO(
        "... SDL_CreateWindow() "
        << (create_win_ret == dummy_window ? "succeeds, ..." : "fails!"));
    setups[Res_CreateWin] = NAMED_REQUIRE_CALL(
        mock_SDL, mock_CreateWindow(re("^Retro Monochrome Text Monitor$"),
                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280,
                      720, SDL_WINDOW_RESIZABLE))
                                .RETURN(create_win_ret)
                                .IN_SEQUENCE(main_seq);
    if (create_win_ret == nullptr) {
      should_succeed = false;
      UNSCOPED_INFO(separator);
    } else {
      dummy_exp = NAMED_REQUIRE_CALL(dummy_t, func()).IN_SEQUENCE(main_seq);
      cleanups[Res_CreateWin] =
          NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyWindow(dummy_window))
              .IN_SEQUENCE(main_seq);
      UNSCOPED_INFO(separator);
      SUCCEED();
    }
  }

  if (do_cleanup_all) {
    cleanups[Res_Init] =
        NAMED_REQUIRE_CALL(mock_SDL, mock_Quit()).IN_SEQUENCE(main_seq);
  } else if (init_ret == 0) {
    cleanups[Res_Init] =
        NAMED_REQUIRE_CALL(mock_SDL, mock_QuitSubSystem(ANY(Uint32)))
            .SIDE_EFFECT(quit_flags = _1)
            .IN_SEQUENCE(main_seq);
  }

  if (should_succeed == false) {
    dummy_exp = NAMED_REQUIRE_CALL(dummy_t, func()).IN_SEQUENCE(main_seq);
  }
  {
    auto t = remoTemo::create(remoTemo::Config().cleanup_all(do_cleanup_all));
    REQUIRE(t.has_value() == should_succeed);
    dummy_t.func();
  }
  if (!do_cleanup_all && init_ret == 0) {
    REQUIRE(init_flags == quit_flags);
  }
}
