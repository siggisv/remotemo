#include "remotemo/remotemo.hpp"

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

using trompeloeil::eq;

class Dummy_test {
  MAKE_MOCK0(dummy_func, void());
};
Dummy_test dummy_test;

class Mock_SDL {
  MAKE_MOCK1(mock_Init, int(Uint32));
  MAKE_MOCK0(mock_Quit, void());
  MAKE_MOCK1(mock_QuitSubSystem, void(Uint32));
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
}
Uint32 init_flags = 0, quit_flags = 0;


TEST_CASE("Test create() - SDL_Init() succeeds", "[Init]")
{
  trompeloeil::sequence seq;

  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32))) //
      .RETURN(0)                                 //
      .IN_SEQUENCE(seq);                         //
  REQUIRE_CALL(dummy_test, dummy_func())         //
      .IN_SEQUENCE(seq);                         //
  REQUIRE_CALL(mock_SDL, mock_Quit())            //
      .IN_SEQUENCE(seq);                         //

  {
    auto t = remoTemo::create(remoTemo::Config());
    REQUIRE(t.has_value() == true);
    dummy_test.dummy_func();
  }
}

TEST_CASE("Test create() - SDL_Init() fails", "[Init]")
{
  trompeloeil::sequence seq;

  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32))) //
      .RETURN(-1)                                //
      .IN_SEQUENCE(seq);                         //
  ALLOW_CALL(mock_SDL, mock_Quit())              //
      .IN_SEQUENCE(seq);                         //
  REQUIRE_CALL(dummy_test, dummy_func())         //
      .IN_SEQUENCE(seq);                         //

  {
    auto t = remoTemo::create();
    REQUIRE(static_cast<bool>(t) == false);
    dummy_test.dummy_func();
  }
}

TEST_CASE(
    "Test create(config.cleanup_all(false)) - SDL_Init() succeeds", "[Init]")
{
  trompeloeil::sequence seq;

  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))          //
      .SIDE_EFFECT(init_flags = _1)                       //
      .RETURN(0)                                          //
      .IN_SEQUENCE(seq);                                  //
  REQUIRE_CALL(dummy_test, dummy_func())                  //
      .IN_SEQUENCE(seq);                                  //
  REQUIRE_CALL(mock_SDL, mock_QuitSubSystem(ANY(Uint32))) //
      .SIDE_EFFECT(quit_flags = _1)                       //
      .IN_SEQUENCE(seq);                                  //

  {
    auto t = remoTemo::create(remoTemo::Config().cleanup_all(false));
    REQUIRE(t.has_value() == true);
    dummy_test.dummy_func();
  }
  REQUIRE(init_flags == quit_flags);
}

TEST_CASE(
    "Test create(config.cleanup_all(false)) - SDL_Init() fails", "[Init]")
{
  trompeloeil::sequence seq;

  REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32))) //
      .RETURN(-1)                                //
      .IN_SEQUENCE(seq);                         //

  {
    auto t = remoTemo::create(remoTemo::Config().cleanup_all(false));
    REQUIRE(t.has_value() == false);
  }
}
