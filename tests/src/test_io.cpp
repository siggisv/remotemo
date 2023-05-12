#include <memory>

#include "remotemo/remotemo.hpp"
#include "../../src/engine.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/trompeloeil.hpp>

class Mock_engine : public remotemo::Engine {
public:
  Mock_engine(const remotemo::Config& config)
      : remotemo::Engine {remotemo::Main_SDL_handler {false},
            remotemo::Window {remotemo::Res_handler<SDL_Window> {}},
            remotemo::Renderer {remotemo::Res_handler<SDL_Renderer> {}},
            remotemo::Background {config.background(),
                remotemo::Texture {remotemo::Res_handler<SDL_Texture> {}}},
            remotemo::Text_display {
                remotemo::Font {config.font(),
                    remotemo::Texture {
                        remotemo::Res_handler<SDL_Texture> {}}},
                nullptr, nullptr, config.text_area()}}
  {}
  MAKE_MOCK0(main_loop_once, void(), override);
};

TEST_CASE("Cursor position can be controlled directly", "[cursor]")
{
  remotemo::Config config {};
  Mock_engine* mock_engine = new Mock_engine(config);
  remotemo::Remotemo t {std::unique_ptr<Mock_engine>(mock_engine), config};

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
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
    t.move_cursor(10, 10);

    auto m = GENERATE_REF(from_range(moves));
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
    t.move_cursor(10, 10);

    auto m = GENERATE_REF(from_range(moves));
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
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
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
    t.set_cursor(10, 20);

    for (auto p : positions) {
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
      REQUIRE(t.set_cursor_line(p.param.y) == p.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }
}
