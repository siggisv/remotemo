#include "remotemo/remotemo.hpp"

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

class Dummy_test {
  MAKE_MOCK0(dummy_func, void());
};
Dummy_test dummy_test;

class Mock_SDL {
  MAKE_MOCK1(mock_Init, int(Uint32));
  MAKE_MOCK0(mock_Quit, void());
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
}

TEST_CASE("Test create()", "Init")
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

TEST_CASE("Test create() - SDL_Init() fails", "Init")
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
