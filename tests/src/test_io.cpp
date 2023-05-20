#include <cmath>
#include <chrono>

#include "remotemo/remotemo.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Cursor position can be controlled directly", "[cursor]")
{
  char env_string[] = "SDL_VIDEODRIVER=dummy";
  putenv(&env_string[0]);

  remotemo::Config config {};
  config.background_file_path("../res/img/terminal_screen.png")
      .font_bitmap_file_path("../res/img/font_bitmap.png")
      .text_area_size(40, 24); // Should be the default
  auto temo = remotemo::create(config);
  auto& t = *temo;
  t.set_text_delay(0);

  SECTION("Starting position should be (0, 0)")
  {
    auto pos = t.get_cursor_position();
    REQUIRE(pos.x == 0);
    REQUIRE(pos.y == 0);
  }

  struct Test_param {
    SDL_Point param;
    int result;
    SDL_Point pos;
  };

  SECTION("move_cursor(int, int), with valid parameters, should work")
  {
    const std::vector<Test_param> moves {{
        {{4, 8}, 0, {4, 8}},   //
        {{-2, 6}, 0, {2, 14}}, //
        {{3, -5}, 0, {5, 9}},  //
        {{-2, -3}, 0, {3, 6}}, //
        {{-3, 18}, 0, {0, 24}} //
    }};
    for (auto m : moves) {
      REQUIRE(t.move_cursor(m.param.x, m.param.y) == m.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == m.pos.x);
      REQUIRE(pos.y == m.pos.y);
    }
  }

  SECTION("move_cursor(SDL_Point&), with valid parameters, should work")
  {
    const std::vector<Test_param> moves {{
        {{4, 8}, 0, {4, 8}},   //
        {{-2, 6}, 0, {2, 14}}, //
        {{3, -5}, 0, {5, 9}},  //
        {{-2, -3}, 0, {3, 6}}, //
        {{-3, 18}, 0, {0, 24}} //
    }};
    for (auto m : moves) {
      REQUIRE(t.move_cursor(m.param) == m.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == m.pos.x);
      REQUIRE(pos.y == m.pos.y);
    }
  }

  SECTION("move_cursor(int, int), with invalid parameters, should fail")
  {
    const std::vector<Test_param> moves {{
        {{400, 10}, -1, {39, 20}}, // right
        {{6, 400}, -2, {16, 24}},  // bottom
        {{-13, 8}, -4, {0, 18}},   // left
        {{4, -17}, -8, {14, 0}},   // top
        {{90, 90}, -3, {39, 24}},  // bottom right
        {{99, -14}, -9, {39, 0}},  // top right
        {{-15, 60}, -6, {0, 24}},  // bottom left
        {{-13, -12}, -12, {0, 0}}  // top left
    }};
    t.move_cursor(10, 10);

    auto m = GENERATE_REF(from_range(moves));
    REQUIRE(t.move_cursor(m.param.x, m.param.y) == m.result);
    auto pos = t.get_cursor_position();
    REQUIRE(pos.x == m.pos.x);
    REQUIRE(pos.y == m.pos.y);
  }

  SECTION("move_cursor(SDL_Point&), with invalid parameters, should fail")
  {
    const std::vector<Test_param> moves {{
        {{400, 10}, -1, {39, 20}}, // right
        {{6, 400}, -2, {16, 24}},  // bottom
        {{-13, 8}, -4, {0, 18}},   // left
        {{4, -17}, -8, {14, 0}},   // top
        {{90, 90}, -3, {39, 24}},  // bottom right
        {{99, -14}, -9, {39, 0}},  // top right
        {{-15, 60}, -6, {0, 24}},  // bottom left
        {{-13, -12}, -12, {0, 0}}  // top left
    }};
    t.move_cursor(10, 10);

    auto m = GENERATE_REF(from_range(moves));
    REQUIRE(t.move_cursor(m.param) == m.result);
    auto pos = t.get_cursor_position();
    REQUIRE(pos.x == m.pos.x);
    REQUIRE(pos.y == m.pos.y);
  }

  SECTION("set_cursor(int, int), with valid parameters, should work")
  {
    const std::vector<Test_param> positions {{
        {{4, 8}, 0, {4, 8}},   //
        {{2, 6}, 0, {2, 6}},   //
        {{33, 5}, 0, {33, 5}}, //
        {{2, 24}, 0, {2, 24}}  //
    }};
    for (auto p : positions) {
      REQUIRE(t.set_cursor(p.param.x, p.param.y) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor(SDL_Point&), with valid parameters, should work")
  {
    const std::vector<Test_param> positions {{
        {{4, 8}, 0, {4, 8}},   //
        {{2, 6}, 0, {2, 6}},   //
        {{33, 5}, 0, {33, 5}}, //
        {{2, 24}, 0, {2, 24}}  //
    }};
    for (auto p : positions) {
      REQUIRE(t.set_cursor(p.param) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor(int, int), with invalid parameters, should fail")
  {
    const std::vector<Test_param> positions {{
        {{-4, 8}, -1, {10, 20}},  //
        {{8, -7}, -1, {10, 20}},  //
        {{40, 10}, -1, {10, 20}}, //
        {{15, 25}, -1, {10, 20}}, //
    }};
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE(t.set_cursor(p.param.x, p.param.y) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor(SDL_Point&), with invalid parameters, should fail")
  {
    const std::vector<Test_param> positions {{
        {{-4, 8}, -1, {10, 20}},  //
        {{8, -7}, -1, {10, 20}},  //
        {{40, 10}, -1, {10, 20}}, //
        {{15, 25}, -1, {10, 20}}, //
    }};
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE(t.set_cursor(p.param) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_column(int), with a valid parameter, should work")
  {
    const std::vector<Test_param> positions {{
        {{4, 8}, 0, {4, 0}},   //
        {{2, 6}, 0, {2, 0}},   //
        {{33, 5}, 0, {33, 0}}, //
        {{2, 24}, 0, {2, 0}}   //
    }};
    for (auto p : positions) {
      REQUIRE(t.set_cursor_column(p.param.x) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_column(int), with invalid parameters, should fail")
  {
    const std::vector<Test_param> positions {{
        {{-4, 8}, -1, {10, 20}},  //
        {{40, 10}, -1, {10, 20}}, //
    }};
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE(t.set_cursor_column(p.param.x) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_line(int), with a valid parameter, should work")
  {
    const std::vector<Test_param> positions {{
        {{4, 8}, 0, {0, 8}},  //
        {{2, 6}, 0, {0, 6}},  //
        {{33, 5}, 0, {0, 5}}, //
        {{2, 24}, 0, {0, 24}} //
    }};
    for (auto p : positions) {
      REQUIRE(t.set_cursor_line(p.param.y) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_line(int), with invalid parameters, should fail")
  {
    const std::vector<Test_param> positions {{
        {{4, -8}, -1, {10, 20}},  //
        {{20, 50}, -1, {10, 20}}, //
    }};
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE(t.set_cursor_line(p.param.y) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }
}

TEST_CASE("pause(int)", "[pause]")
{
  char env_string[] = "SDL_VIDEODRIVER=dummy";
  putenv(&env_string[0]);

  remotemo::Config config {};
  config.background_file_path("../res/img/terminal_screen.png")
      .font_bitmap_file_path("../res/img/font_bitmap.png");
  auto t = remotemo::create(config);
  t->set_text_delay(0);

  SECTION("pause(int), with negative numbers should fail")
  {
    for (int bad_duration : {-1, -35, -10'000}) {
      REQUIRE(t->pause(bad_duration) != 0);
    }
  }

  SECTION("pause(int), with positive parameters, should wait for aproximate "
          "the given time (in milliseconds)")
  {
    for (int ok_duration : {0, 1, 35, 490, 2000, 10'000}) {
      auto start = std::chrono::high_resolution_clock::now();
      REQUIRE(t->pause(ok_duration) == 0);
      auto end = std::chrono::high_resolution_clock::now();
      auto elapsed_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
      UNSCOPED_INFO("pause(" << ok_duration << ") took " << elapsed_ms.count()
                             << " to run.\n");
      REQUIRE(abs(elapsed_ms.count() - ok_duration) < 15);
    }
  }
}
