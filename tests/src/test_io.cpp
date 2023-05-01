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
                nullptr, config.text_area()}}
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

  struct Move {
    int move_x;
    int move_y;
    int result;
    int pos_x;
    int pos_y;
  };

  SECTION("move_cursor(), with valid parameters, should work")
  {
    const std::vector<Move> moves {{
        {4, 8, 0, 4, 8},   //
        {-2, 6, 0, 2, 14}, //
        {3, -5, 0, 5, 9},  //
        {-2, -3, 0, 3, 6}  //
    }};
    for (auto m : moves) {
      REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
      REQUIRE(t.move_cursor(m.move_x, m.move_y) == m.result);
      auto pos = t.get_cursor_position();
      REQUIRE(pos.x == m.pos_x);
      REQUIRE(pos.y == m.pos_y);
    }
  }

  SECTION("move_cursor(), w/invalid parameters, should fail")
  {
    const std::vector<Move> moves {{
        {400, 10, -1, 39, 10}, // right
        {6, 400, -2, 6, 23},   // bottom
        {-3, 8, -4, 0, 8},     // left
        {4, -7, -8, 4, 0},     // top
        {90, 90, -3, 39, 23},  // bottom right
        {99, -4, -9, 39, 0},   // top right
        {-5, 60, -6, 0, 23},   // bottom left
        {-3, -2, -12, 0, 0}    // top left
    }};
    auto m = GENERATE_REF(from_range(moves));
    REQUIRE_CALL(*mock_engine, main_loop_once()).TIMES(1, 2);
    REQUIRE(t.move_cursor(m.move_x, m.move_y) == m.result);
    auto pos = t.get_cursor_position();
    REQUIRE(pos.x == m.pos_x);
    REQUIRE(pos.y == m.pos_y);
  }
}
