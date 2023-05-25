#include <algorithm>
#include <deque>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include <chrono>

#include "remotemo/remotemo.hpp"
#include "../../src/engine.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

using std::string_literals::operator""s;

constexpr int default_columns = 40;
constexpr int default_lines = 24;

struct Console_content {
  Console_content() = default;
  Console_content(int lines, const std::string def_line,
      const std::deque<bool> def_is_line_inv)
      : text(std::deque<std::string>(lines, def_line)),
        is_inv(std::deque<std::deque<bool>>(lines, def_is_line_inv))
  {}

  std::deque<std::string> text {};
  std::deque<std::deque<bool>> is_inv {};
};
// ----- Helper functions ------

std::string get_line_str(int line_num, const remotemo::Engine* engine)
{
  std::stringstream line {};
  auto area_size = engine->text_area_size();
  for (SDL_Point pos {0, line_num}; pos.x < area_size.x; pos.x++) {
    line << engine->char_at(pos);
  }
  return line.str();
}

void compare_to_content(const std::deque<std::string>& expected_text_content,
    const remotemo::Engine* engine)
{
  int max_line = expected_text_content.size();
  for (int i = 0; i < max_line; i++) {
    INFO("Checking line " << i);
    std::string line_content = get_line_str(i, engine);
    REQUIRE(line_content == expected_text_content[i]);
  }
}

std::deque<bool> get_line_is_inverse(
    int line_num, const remotemo::Engine* engine)
{
  std::deque<bool> line {};
  auto area_size = engine->text_area_size();
  for (SDL_Point pos {0, line_num}; pos.x < area_size.x; pos.x++) {
    line.push_back(engine->is_inverse_at(pos));
  }
  return line;
}

void compare_to_content(
    const std::deque<std::deque<bool>>& expected_is_content_inv,
    const remotemo::Engine* engine)
{
  int max_line = expected_is_content_inv.size();
  for (int i = 0; i < max_line; i++) {
    INFO("Checking line " << i);
    std::deque<bool> line_inverse = get_line_is_inverse(i, engine);
    REQUIRE(line_inverse == expected_is_content_inv[i]);
  }
}

void check_status(const Console_content& expected_content,
    const SDL_Point& expected_cursor_pos, const remotemo::Engine* engine)
{
  compare_to_content(expected_content.is_inv, engine);
  compare_to_content(expected_content.text, engine);
  auto cursor_pos = engine->cursor_pos();
  REQUIRE(cursor_pos.x == expected_cursor_pos.x);
  REQUIRE(cursor_pos.y == expected_cursor_pos.y);
}

remotemo::Config setup(int columns = 0, int lines = 0)
{
  static char env_string[] = "SDL_VIDEODRIVER=dummy";
  putenv(&env_string[0]);

  remotemo::Config config {};
  config.background_file_path("../res/img/terminal_screen.png")
      .font_bitmap_file_path("../res/img/font_bitmap.png");
  if (columns <= 0 && lines <= 0) {
    return config; // Use default text area size (40, 24)
  }
  if (columns <= 0) {
    columns = default_columns;
  }
  if (lines <= 0) {
    lines = default_lines;
  }
  config.text_area_size(columns, lines);
  return config;
}

// ----- Tests ------

TEST_CASE("Cursor position can be controlled directly", "[cursor]")
{
  struct Cursor_param {
    SDL_Point param;
    int result;
    SDL_Point pos;
  };

  const std::vector<SDL_Point> area_sizes {{
      {default_columns, default_lines}, // i.e. use default
      {20, 12},                         // default / 2
      {80, 36}                          //
  }};
  auto a_size = GENERATE_REF(from_range(area_sizes));
  auto config = setup(a_size.x, a_size.y);
  auto t = remotemo::create(config);
  t->set_text_delay(0);

  SECTION("Starting position should be (0, 0)")
  {
    auto pos = t->get_cursor_position();
    REQUIRE(pos.x == 0);
    REQUIRE(pos.y == 0);
  }

  SECTION("move_cursor(), with valid parameters, should work")
  {
    const std::vector<Cursor_param> valid_moves {{
        {{4, 8}, 0, {14, 18}}, //
        {{-2, 6}, 0, {8, 16}}, //
        {{3, -5}, 0, {13, 5}}, //
        {{-2, -3}, 0, {8, 7}}, //
        {{-3, 14}, 0, {7, 24}} //
    }};

    auto move = GENERATE_COPY(from_range(valid_moves));
    if (move.param.x + 10 >= a_size.x) {
      move.param.x = a_size.x - 11;
      move.pos.x = a_size.x - 1;
    }
    if (move.param.y + 10 > a_size.y) {
      move.param.y = a_size.y - 10;
      move.pos.y = a_size.y;
    }
    INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                       << "Start pos (10, 10). Move (" << move.param.x << ", "
                       << move.param.y << ") Expected: (" << move.pos.x
                       << ", " << move.pos.y << ")\n");

    SECTION("move_cursor(int, int), with valid parameters, should work")
    {
      t->set_cursor(10, 10);

      REQUIRE(t->move_cursor(move.param.x, move.param.y) == move.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == move.pos.x);
      REQUIRE(pos.y == move.pos.y);
    }

    SECTION("move_cursor(SDL_Point&), with valid parameters, should work")
    {
      t->set_cursor(10, 10);

      REQUIRE(t->move_cursor(move.param) == move.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == move.pos.x);
      REQUIRE(pos.y == move.pos.y);
    }
  }

  SECTION("move_cursor(), with invalid parameters, should fail")
  {
    const std::vector<Cursor_param> invalid_moves {{
        {{400, 5}, -1, {a_size.x - 1, 10}},       // right
        {{6, 400}, -2, {16, a_size.y}},           // bottom
        {{-13, 6}, -4, {0, 11}},                  // left
        {{4, -17}, -8, {14, 0}},                  // top
        {{90, 90}, -3, {a_size.x - 1, a_size.y}}, // bottom right
        {{99, -14}, -9, {a_size.x - 1, 0}},       // top right
        {{-15, 60}, -6, {0, a_size.y}},           // bottom left
        {{-13, -12}, -12, {0, 0}}                 // top left
    }};

    auto move = GENERATE_REF(from_range(invalid_moves));
    INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                       << "Start pos (10, 5). Move (" << move.param.x << ", "
                       << move.param.y << ") Expected: (" << move.pos.x
                       << ", " << move.pos.y << ")\n");

    SECTION("move_cursor(int, int), with invalid parameters, should fail")
    {
      t->set_cursor(10, 5);

      REQUIRE(t->move_cursor(move.param.x, move.param.y) == move.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == move.pos.x);
      REQUIRE(pos.y == move.pos.y);
    }
    SECTION("move_cursor(SDL_Point&), with invalid parameters, should fail")
    {
      t->set_cursor(10, 5);

      REQUIRE(t->move_cursor(move.param.x, move.param.y) == move.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == move.pos.x);
      REQUIRE(pos.y == move.pos.y);
    }
  }

  SECTION("set_cursor(), with valid parameters, should work")
  {
    const std::vector<Cursor_param> positions {{
        {{4, 8}, 0, {4, 8}},                       //
        {{2, 6}, 0, {2, 6}},                       //
        {{a_size.x - 1, 5}, 0, {a_size.x - 1, 5}}, //
        {{2, a_size.y}, 0, {2, a_size.y}}          //
    }};

    auto p = GENERATE_REF(from_range(positions));
    INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                       << "Set (" << p.param.x << ", " << p.param.y
                       << ") Expected: (" << p.pos.x << ", " << p.pos.y
                       << ")\n");

    SECTION("set_cursor(int, int), with valid parameters, should work")
    {
      REQUIRE(t->set_cursor(p.param.x, p.param.y) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
    SECTION("set_cursor(SDL_Point&), with valid parameters, should work")
    {
      REQUIRE(t->set_cursor(p.param) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }
  SECTION("set_cursor(), with invalid parameters, should fail")
  {
    const std::vector<Cursor_param> positions {{
        {{-4, 8}, -1, {15, 10}},            //
        {{8, -7}, -1, {15, 10}},            //
        {{a_size.x, 10}, -1, {15, 10}},     //
        {{15, a_size.y + 1}, -1, {15, 10}}, //
        {{a_size.x + 5, 10}, -1, {15, 10}}, //
        {{15, a_size.y + 5}, -1, {15, 10}}, //
    }};
    t->set_cursor(15, 10);

    auto p = GENERATE_REF(from_range(positions));
    INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                       << "Set (" << p.param.x << ", " << p.param.y
                       << ") Expected: (" << p.pos.x << ", " << p.pos.y
                       << ")\n");

    SECTION("set_cursor(int, int), with invalid parameters, should fail")
    {
      REQUIRE(t->set_cursor(p.param.x, p.param.y) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
    SECTION("set_cursor(SDL_Point&), with invalid parameters, should fail")
    {
      REQUIRE(t->set_cursor(p.param) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_column(int), with a valid parameter, should work")
  {
    const std::vector<Cursor_param> positions {{
        {{4, 0}, 0, {4, 5}},                       //
        {{0, 0}, 0, {0, 5}},                       //
        {{a_size.x - 7, 0}, 0, {a_size.x - 7, 5}}, //
        {{a_size.x - 1, 0}, 0, {a_size.x - 1, 5}}  //
    }};
    for (auto p : positions) {
      t->set_cursor(5, 5);
      INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                         << "Set column (" << p.param.x << ") Expected: ("
                         << p.pos.x << ", " << p.pos.y << ")\n");

      REQUIRE(t->set_cursor_column(p.param.x) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_column(int), with invalid parameters, should fail")
  {
    const std::vector<Cursor_param> positions {{
        {{-4, 0}, -1, {10, 10}},           //
        {{a_size.x, 0}, -1, {10, 10}},     //
        {{a_size.x + 4, 0}, -1, {10, 10}}, //
    }};
    for (auto p : positions) {
      t->set_cursor(10, 10);
      INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                         << "Set column (" << p.param.x << ") Expected: ("
                         << p.pos.x << ", " << p.pos.y << ")\n");

      REQUIRE(t->set_cursor_column(p.param.x) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_line(int), with a valid parameter, should work")
  {
    const std::vector<Cursor_param> positions {{
        {{0, 0}, 0, {5, 0}},              //
        {{0, 6}, 0, {5, 6}},              //
        {{0, 5}, 0, {5, 5}},              //
        {{0, a_size.y}, 0, {5, a_size.y}} //
    }};
    for (auto p : positions) {
      t->set_cursor(5, 5);
      INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                         << "Set line (" << p.param.y << ") Expected: ("
                         << p.pos.x << ", " << p.pos.y << ")\n");

      REQUIRE(t->set_cursor_line(p.param.y) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }

  SECTION("set_cursor_line(int), with invalid parameters, should fail")
  {
    const std::vector<Cursor_param> positions {{
        {{0, -8}, -1, {5, 10}},           //
        {{0, a_size.y + 1}, -1, {5, 10}}, //
        {{0, 50}, -1, {5, 10}},           //
    }};

    for (auto p : positions) {
      t->set_cursor(5, 10);
      INFO("Area size (" << a_size.x << ", " << a_size.y << ") "
                         << "Set line (" << p.param.y << ") Expected: ("
                         << p.pos.x << ", " << p.pos.y << ")\n");

      REQUIRE(t->set_cursor_line(p.param.y) == p.result);
      auto pos = t->get_cursor_position();
      REQUIRE(pos.x == p.pos.x);
      REQUIRE(pos.y == p.pos.y);
    }
  }
}

TEST_CASE("pause(int)", "[pause]")
{
  auto config = setup();
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
                             << "ms to run.\n");
      REQUIRE(abs(elapsed_ms.count() - ok_duration) < 15);
    }
  }
}

TEST_CASE("print() and print_at() functions", "[print]")
{
  constexpr int columns = 20;
  constexpr int lines = 5;
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto eng = remotemo::Engine::create(config);
  auto* engine = eng.get();
  remotemo::Remotemo t {std::move(eng), config};
  t.set_text_delay(0);
  Console_content expected_content {lines, empty_line, normal_line};

  SECTION("Checking starting status")
  {
    check_status(expected_content, SDL_Point {0, 0}, engine);
    REQUIRE(t.get_scrolling() == true);
    REQUIRE(t.get_wrapping() == remotemo::Wrapping::character);
    REQUIRE(t.get_inverse() == false);
  }

  SECTION("Printing text from starting position")
  {
    SDL_Point expected_cursor_pos {0, 0};
    for (const auto& text : {"Foo!"s, "_bar_"s, "<spam>"s}) {
      REQUIRE(t.print(text) == 0);
      expected_content.text[expected_cursor_pos.y].replace(
          expected_cursor_pos.x, text.size(), text);
      expected_cursor_pos.x += text.size();
      check_status(expected_content, expected_cursor_pos, engine);
    }
    SECTION("Printing \"\\n\" should move cursor to beginning of next line")
    {
      REQUIRE(t.print("\n\n") == 0);
      expected_cursor_pos.x = 0;
      expected_cursor_pos.y += 2;
      check_status(expected_content, expected_cursor_pos, engine);
      for (const auto& text : {"Foo!"s, "_bar_"s, "<spam>"s}) {
        REQUIRE(t.print(text + "\n") == 0);
        expected_content.text[expected_cursor_pos.y].replace(
            expected_cursor_pos.x, text.size(), text);
        expected_cursor_pos.x = 0;
        expected_cursor_pos.y++;
        check_status(expected_content, expected_cursor_pos, engine);
      }
    }
  }

  SECTION("Printing text at given position")
  {
    SDL_Point expected_cursor_pos {0, 0};
    for (const auto& text : {"Foo!!!"s, "_bar_"s, "spam"s}) {
      REQUIRE(t.print_at(5, 2, text) == 0);
      expected_content.text[2].replace(5, text.size(), text);
      expected_cursor_pos.x = 5 + text.size();
      expected_cursor_pos.y = 2;
      check_status(expected_content, expected_cursor_pos, engine);
    }
    SECTION("Printing \"\\b\' (backspace) should delete previous content")
    {
      REQUIRE(t.print("\b\b\b") == 0);
      expected_cursor_pos.x -= 3;
      expected_content.text[expected_cursor_pos.y].replace(
          expected_cursor_pos.x, 3, "   ");
      check_status(expected_content, expected_cursor_pos, engine);
    }
  }
}

TEST_CASE("print() with delay", "[print][pause]")
{
  constexpr int allowed_error = 50;
  auto t = remotemo::create(setup());

  for (int delay_ms : {0, 30, 60, 100, 200}) {
    t->set_cursor(0, 0);
    t->set_text_delay(delay_ms);
    for (const auto& text : {"Foo!"s, "_bar_"s, "<spam>"s}) {
      int expected_duration = delay_ms * text.size();
      auto start = std::chrono::high_resolution_clock::now();
      t->print(text);
      auto end = std::chrono::high_resolution_clock::now();
      auto elapsed_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
      UNSCOPED_INFO("print(\"" << text << "\") with delay per char "
                               << delay_ms << "ms and expected time "
                               << expected_duration << "ms took "
                               << elapsed_ms.count() << "ms to run.\n");
      REQUIRE(abs(elapsed_ms.count() - expected_duration) / text.size() <
              allowed_error);
    }
  }
}

TEST_CASE("print() - scroll set to true", "[print][scroll]")
{
  constexpr int columns = 24;
  constexpr int lines = 5;
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto eng = remotemo::Engine::create(config);
  auto* engine = eng.get();
  remotemo::Remotemo t {std::move(eng), config};
  t.set_text_delay(0);
  REQUIRE(t.get_scrolling() == true);
  Console_content expected_content {lines, empty_line, normal_line};

  SECTION("Printing text from starting position")
  {
    SDL_Point expected_cursor_pos {0, 0};

    // Fill screen:
    for (const auto& text :
        {"Foo!"s, "_bar_"s, "<spam>"s, "remoTemo"s, "Bottom"s}) {
      REQUIRE(t.print(text + "\n") == 0);
      expected_content.text[expected_cursor_pos.y].replace(
          expected_cursor_pos.x, text.size(), text);
      expected_cursor_pos.x = 0;
      expected_cursor_pos.y++;
    }
    check_status(expected_content, expected_cursor_pos, engine);

    // Start scrolling:
    for (const auto& text : {"Start scrolling"s, "next 2 lines empty"s, ""s,
             ""s, "New bottom line"s}) {
      REQUIRE(t.print(text + "\n") == 0);
      expected_content.text.pop_front();
      expected_content.text.push_back(empty_line);
      expected_content.text[lines - 1].replace(
          expected_cursor_pos.x, text.size(), text);
      expected_cursor_pos.x = 0;
      expected_cursor_pos.y = lines;
      check_status(expected_content, expected_cursor_pos, engine);
    }
  }

  SECTION("print_at() w/ content one line below bottom should scroll up")
  {
    SDL_Point expected_cursor_pos {0, 0};
    const SDL_Point print_to_pos {2, lines};

    for (const auto& text : {"Start scrolling"s, "no lines empty"s, "---"s,
             " "s, "New bottom line"s}) {
      REQUIRE(t.print_at(print_to_pos, text) == 0);
      expected_content.text.pop_front();
      expected_content.text.push_back(empty_line);
      expected_content.text[lines - 1].replace(
          print_to_pos.x, text.size(), text);
      expected_cursor_pos.x = print_to_pos.x + text.size();
      expected_cursor_pos.y = lines - 1;
      check_status(expected_content, expected_cursor_pos, engine);
    }
    SECTION("print_at() without content should NOT scroll up")
    {
      for (const auto& text : {""s, ""s, ""s, ""s}) {
        REQUIRE(t.print_at(print_to_pos, text) == 0);
        expected_cursor_pos.x = print_to_pos.x;
        expected_cursor_pos.y = print_to_pos.y;
        check_status(expected_content, expected_cursor_pos, engine);
      }
    }
  }
}

TEST_CASE("print() - wrap set to character", "[print][wrap]")
{
  constexpr int columns = 20;
  constexpr int lines = 5;
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto eng = remotemo::Engine::create(config);
  auto* engine = eng.get();
  remotemo::Remotemo t {std::move(eng), config};
  t.set_text_delay(0);
  REQUIRE(t.get_wrapping() == remotemo::Wrapping::character);
  Console_content expected_content {lines, empty_line, normal_line};

  SECTION("Printing text from starting position")
  {
    SDL_Point expected_cursor_pos {0, 0};

    for (const auto& text :
        {"This line is a bit long for a screen not wider than this"s,
            "-- more long text that should wrap."s}) {
      REQUIRE(t.print(text) == 0);
      int pos_in_text = 0;
      while (pos_in_text < text.size()) {
        int left_of_line = columns - expected_cursor_pos.x;
        int length_to_copy = std::min(
            static_cast<int>(text.size() - pos_in_text), left_of_line);
        expected_content.text[expected_cursor_pos.y].replace(
            expected_cursor_pos.x, length_to_copy, text, pos_in_text,
            length_to_copy);
        pos_in_text += length_to_copy;
        expected_cursor_pos.x += length_to_copy;
        if (expected_cursor_pos.x == columns) {
          expected_cursor_pos.x = 0;
          expected_cursor_pos.y++;
        }
      }
      check_status(expected_content, expected_cursor_pos, engine);
    }
  }
}

TEST_CASE("print() - scroll is on and wrap set to character",
    "[print][scroll][wrap]")
{
  constexpr int columns = 12;
  constexpr int lines = 5;
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto eng = remotemo::Engine::create(config);
  auto* engine = eng.get();
  remotemo::Remotemo t {std::move(eng), config};
  t.set_text_delay(0);
  REQUIRE(t.get_wrapping() == remotemo::Wrapping::character);
  Console_content expected_content {lines, empty_line, normal_line};

  SECTION("Printing text from starting position")
  {
    SDL_Point expected_cursor_pos {0, 0};

    for (const auto& text :
        {"This line is a bit long for a screen not wider than this"s,
            " ... more text that now should wrap AND scroll......"s,
            "... scroll and wrap and scroll, etc. ...."s}) {
      REQUIRE(t.print(text) == 0);
      int pos_in_text = 0;
      while (pos_in_text < text.size()) {
        int left_of_line = columns - expected_cursor_pos.x;
        if (expected_cursor_pos.y == lines) {
          expected_content.text.pop_front();
          expected_content.text.push_back(empty_line);
          expected_cursor_pos.y = lines - 1;
        }
        int length_to_copy = std::min(
            static_cast<int>(text.size() - pos_in_text), left_of_line);
        expected_content.text[expected_cursor_pos.y].replace(
            expected_cursor_pos.x, length_to_copy, text, pos_in_text,
            length_to_copy);
        pos_in_text += length_to_copy;
        expected_cursor_pos.x += length_to_copy;
        if (expected_cursor_pos.x == columns) {
          expected_cursor_pos.x = 0;
          expected_cursor_pos.y++;
        }
      }
      check_status(expected_content, expected_cursor_pos, engine);
    }
  }
}

TEST_CASE("set_inverse() should affect the 'inverse' setting", "[inverse]")
{
  auto t = remotemo::create(setup());

  SECTION("Default for inverse should be 'false'")
  {
    REQUIRE(t->get_inverse() == false);
  }

  SECTION("Try setting off and on again :P")
  {
    for (auto inv : {false, true, true, false, false, true}) {
      t->set_inverse(inv);
      REQUIRE(t->get_inverse() == inv);
    }
  }
}

TEST_CASE("The 'inverse' setting should affect printing", "[print][inverse]")
{
  constexpr int columns = 20;
  constexpr int lines = 4;
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto eng = remotemo::Engine::create(config);
  auto* engine = eng.get();
  remotemo::Remotemo t {std::move(eng), config};
  t.set_text_delay(0);
  Console_content expected_content {lines, empty_line, normal_line};
  SDL_Point expected_cursor_pos {0, 0};

  SECTION("Just calling set_inverse() should not change screen content")
  {
    for (auto inv : {false, true, true, false}) {
      t.set_inverse(inv);
      check_status(expected_content, expected_cursor_pos, engine);
    }
  }

  SECTION("print() with inverse set to 'true'")
  {
    t.set_inverse(true);

    for (const auto& text : {"   "s, "Hello"s, "- there -"s}) {
      int col = (columns - text.size()) / 2;
      int& line = expected_cursor_pos.y;
      t.set_cursor_column(col);
      t.print(text + "\n");
      expected_content.text[line].replace(col, text.size(), text);
      for (int i = col; i < col + text.size(); i++) {
        expected_content.is_inv[line][i] = true;
      }
      expected_cursor_pos.y++;
      check_status(expected_content, expected_cursor_pos, engine);
    }

    SECTION("Just print(\"\\n\") should not change the content")
    {
      for (const auto& text : {"\n"s, "\n\n"s}) {
        t.set_cursor(0, 0);
        t.print(text);
        expected_cursor_pos.x = 0;
        expected_cursor_pos.y = text.size();
        check_status(expected_content, expected_cursor_pos, engine);
      }
    }

    SECTION("print() over inversed after setting 'inverse' back to false")
    {
      t.set_cursor(0, 0);
      expected_cursor_pos.x = 0;
      expected_cursor_pos.y = 0;
      t.set_inverse(false);
      for (const auto& text : {" "s, "Allo"s, "====="s}) {
        int col = (columns - text.size()) / 2;
        int& line = expected_cursor_pos.y;
        t.set_cursor_column(col);
        t.print(text + "\n");
        expected_content.text[line].replace(col, text.size(), text);
        for (int i = col; i < col + text.size(); i++) {
          expected_content.is_inv[line][i] = false;
        }
        expected_cursor_pos.y++;
        check_status(expected_content, expected_cursor_pos, engine);
      }
    }
  }
}

TEST_CASE("Inversed content should scroll the same way as the text",
    "[print][inverse][scroll]")
{
  constexpr int columns = 20;
  constexpr int lines = 4;
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto eng = remotemo::Engine::create(config);
  auto* engine = eng.get();
  remotemo::Remotemo t {std::move(eng), config};
  t.set_text_delay(0);
  Console_content expected_content {lines, empty_line, normal_line};
  SDL_Point expected_cursor_pos {0, 0};

  t.set_inverse(true);

  for (const auto& text : {"   "s, "Hello"s, "- there -"s, "          "s,
           "start"s, "....scrolling!"s, ""s, ""s, "ok"s}) {
    int col = (columns - text.size()) / 2;
    int& line = expected_cursor_pos.y;
    t.set_cursor_column(col);
    t.print(text + "\n");
    if (line == lines) {
      expected_content.text.pop_front();
      expected_content.text.push_back(empty_line);
      expected_content.is_inv.pop_front();
      expected_content.is_inv.push_back(normal_line);
      line--;
    }
    expected_content.text[line].replace(col, text.size(), text);
    for (int i = col; i < col + text.size(); i++) {
      expected_content.is_inv[line][i] = true;
    }
    expected_cursor_pos.y++;
    check_status(expected_content, expected_cursor_pos, engine);
  }
}
