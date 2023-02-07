#include "remotemo/remotemo.hpp"

#include <string>
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

using trompeloeil::eq;
using trompeloeil::re;

class Dummy_test {
  MAKE_MOCK0(dummy_func, void());
};
Dummy_test dummy_test;

struct Dummy_window {
  char dummy_text[2000];
  int dummy_int;
};
Dummy_window dummy_w {{"Dummy window."}, 5};
SDL_Window* dummy_window = reinterpret_cast<SDL_Window*>(&dummy_w);

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

Uint32 init_flags = 0, quit_flags = 0;
const char regex_hint_name[] = "^" SDL_HINT_RENDER_SCALE_QUALITY "$";

TEST_CASE("Test create() - SDL_Init() fails", "[Init]")
{
  trompeloeil::sequence seq;
  // clang-format off
  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))
      .RETURN(-1)
      .IN_SEQUENCE(seq);
  REQUIRE_CALL(mock_SDL, mock_Quit())
      .IN_SEQUENCE(seq);
  REQUIRE_CALL(dummy_test, dummy_func())
      .IN_SEQUENCE(seq);
  // clang-format on
  {
    auto t = remoTemo::create();
    REQUIRE(t.has_value() == false);
    dummy_test.dummy_func();
  }
}

TEST_CASE(
    "Test create(config.cleanup_all(false)) - SDL_Init() fails", "[Init]")
{
  trompeloeil::sequence seq;
  // clang-format off
  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))
      .RETURN(-1)
      .IN_SEQUENCE(seq);
  // clang-format on
  {
    auto t = remoTemo::create(remoTemo::Config().cleanup_all(false));
    REQUIRE(t.has_value() == false);
  }
}


TEST_CASE("Test create() - SDL_Init() succeeds, ...", "[Init]")
{
  trompeloeil::sequence init_seq, cleanup_seq;
  // clang-format off
  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))
      .RETURN(0)
      .IN_SEQUENCE(init_seq);
  // clang-format on

  SECTION("... but SDL_SetHint() fails!")
  {
    // clang-format off
    REQUIRE_CALL(mock_SDL, mock_SetHint(re(regex_hint_name), re("^linear$")))
        .RETURN(SDL_FALSE)
        .IN_SEQUENCE(init_seq);
    REQUIRE_CALL(dummy_test, dummy_func())
        .IN_SEQUENCE(init_seq, cleanup_seq);
    REQUIRE_CALL(mock_SDL, mock_Quit())
        .IN_SEQUENCE(cleanup_seq);
    // clang-format on
    {
      auto t = remoTemo::create(remoTemo::Config());
      REQUIRE(t.has_value() == true);
      dummy_test.dummy_func();
    }
  }

  SECTION("...  SDL_SetHint() succeeds, ...")
  {
    // clang-format off
    REQUIRE_CALL(mock_SDL, mock_SetHint(re(regex_hint_name), re("^linear$")))
        .RETURN(SDL_TRUE)
        .IN_SEQUENCE(init_seq);

    SECTION("... but SDL_CreateWindow() fails!")
    {
      // clang-format off
      REQUIRE_CALL(mock_SDL, mock_CreateWindow(
              re("^Retro Monochrome Text Monitor$"),
              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
              1280, 720, SDL_WINDOW_RESIZABLE))
          .RETURN(nullptr)
          .IN_SEQUENCE(init_seq);
      REQUIRE_CALL(mock_SDL, mock_Quit())
          .IN_SEQUENCE(init_seq, cleanup_seq);
      REQUIRE_CALL(dummy_test, dummy_func())
          .IN_SEQUENCE(cleanup_seq);
      // clang-format on
      {
        auto t = remoTemo::create();
        REQUIRE(t.has_value() == false);
        dummy_test.dummy_func();
      }
    }

    SECTION("... SDL_CreateWindow() succeeds, ...")
    {
      // clang-format off
      REQUIRE_CALL(mock_SDL, mock_CreateWindow(
              re("^Retro Monochrome Text Monitor$"),
              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
              1280, 720, SDL_WINDOW_RESIZABLE))
          .RETURN(dummy_window)
          .IN_SEQUENCE(init_seq);
      REQUIRE_CALL(dummy_test, dummy_func())
          .IN_SEQUENCE(init_seq, cleanup_seq);
      REQUIRE_CALL(mock_SDL, mock_DestroyWindow(dummy_window))
          .IN_SEQUENCE(cleanup_seq);
      REQUIRE_CALL(mock_SDL, mock_Quit())
          .IN_SEQUENCE(cleanup_seq);
      // clang-format on
      {
        auto t = remoTemo::create();
        REQUIRE(t.has_value() == true);
        dummy_test.dummy_func();
      }
    }
  }
}

TEST_CASE("Test create(config.cleanup_all(false)) - SDL_Init() succeeds, ...",
    "[Init]")
{
  trompeloeil::sequence init_seq, cleanup_seq;
  // clang-format off
  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))
      .SIDE_EFFECT(init_flags = _1)
      .RETURN(0)
      .IN_SEQUENCE(init_seq);
  // clang-format on

  SECTION("... but SDL_SetHint() fails!")
  {
    // clang-format off
    REQUIRE_CALL(mock_SDL, mock_SetHint(re(regex_hint_name), re("^linear$")))
        .RETURN(SDL_FALSE)
        .IN_SEQUENCE(init_seq);
    REQUIRE_CALL(dummy_test, dummy_func())
        .IN_SEQUENCE(init_seq, cleanup_seq);
    REQUIRE_CALL(mock_SDL, mock_QuitSubSystem(ANY(Uint32)))
        .SIDE_EFFECT(quit_flags = _1)
        .IN_SEQUENCE(cleanup_seq);
    // clang-format on
    {
      auto t = remoTemo::create(remoTemo::Config().cleanup_all(false));
      REQUIRE(t.has_value() == true);
      dummy_test.dummy_func();
    }
    REQUIRE(init_flags == quit_flags);
  }

  SECTION("...  SDL_SetHint() succeeds, ...")
  {
    // clang-format off
    REQUIRE_CALL(mock_SDL, mock_SetHint(re(regex_hint_name), re("^linear$")))
        .RETURN(SDL_FALSE)
        .IN_SEQUENCE(init_seq);
    REQUIRE_CALL(dummy_test, dummy_func())
        .IN_SEQUENCE(init_seq, cleanup_seq);
    REQUIRE_CALL(mock_SDL, mock_QuitSubSystem(ANY(Uint32)))
        .SIDE_EFFECT(quit_flags = _1)
        .IN_SEQUENCE(cleanup_seq);
    // clang-format on
    {
      auto t = remoTemo::create(remoTemo::Config().cleanup_all(false));
      REQUIRE(t.has_value() == true);
      dummy_test.dummy_func();
    }
    REQUIRE(init_flags == quit_flags);
  }
}
