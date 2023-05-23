#include <deque>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include <chrono>

#include "remotemo/remotemo.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

using std::string_literals::operator""s;

constexpr int default_columns = 40;
constexpr int default_lines = 24;

// ----- Helper functions ------

std::string get_line_str(int line_num, const remotemo::Remotemo* t_monitor)
{
  std::stringstream line {};
  int col = 0;
  char c {};
  while ((c = t_monitor->get_char_at(col++, line_num)) != 0) {
    line << c;
  }
  return line.str();
}

void compare_to_content(const std::deque<std::string>& expected_content,
    const remotemo::Remotemo* t)
{
  int max_line = expected_content.size();
  for (int i = 0; i < max_line; i++) {
    INFO("Checking line " << i);
    std::string line_content = get_line_str(i, t);
    REQUIRE(line_content == expected_content[i]);
  }
}

std::deque<bool> get_line_is_inverse(
    int line_num, const remotemo::Remotemo* t_monitor)
{
  std::deque<bool> line {};
  int col = 0;
  while (t_monitor->get_char_at(col, line_num) != 0) {
    line.push_back(t_monitor->is_inverse_at(col, line_num));
    col++;
  }
  return line;
}

void compare_to_content(
    const std::deque<std::deque<bool>>& expected_is_content_inv,
    const remotemo::Remotemo* t)
{
  int max_line = expected_is_content_inv.size();
  for (int i = 0; i < max_line; i++) {
    INFO("Checking line " << i);
    std::deque<bool> line_inverse = get_line_is_inverse(i, t);
    REQUIRE(line_inverse == expected_is_content_inv[i]);
  }
}

void check_status(const std::deque<std::string>& expected_content,
    const std::deque<std::deque<bool>>& expected_is_content_inv,
    const SDL_Point& expected_cursor_pos, const remotemo::Remotemo* t,
    bool expected_inverse = false, bool expected_scrolling = true,
    remotemo::Wrapping expected_wrapping = remotemo::Wrapping::character)
{
  REQUIRE(t->get_scrolling() == expected_scrolling);
  REQUIRE(t->get_wrapping() == expected_wrapping);
  REQUIRE(t->get_inverse() == expected_inverse);
  compare_to_content(expected_is_content_inv, t);
  compare_to_content(expected_content, t);
  auto cursor_pos = t->get_cursor_position();
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
  constexpr SDL_Point a_size {columns, lines};
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto t = remotemo::create(config);
  t->set_text_delay(0);

  SECTION("Checking starting status")
  {
    std::deque<std::string> empty_console(lines, empty_line);
    std::deque<std::deque<bool>> normal_console(lines, normal_line);
    check_status(empty_console, normal_console, SDL_Point {0, 0}, &(*t));
  }

  SECTION("Printing text from starting position")
  {
    std::deque<std::string> expected_content(lines, empty_line);
    std::deque<std::deque<bool>> expected_is_inverse(lines, normal_line);
    SDL_Point expected_cursor_pos {0, 0};
    for (const auto& text : {"Foo!"s, "_bar_"s, "<spam>"s}) {
      REQUIRE(t->print(text) == 0);
      expected_content[expected_cursor_pos.y].replace(
          expected_cursor_pos.x, text.size(), text);
      expected_cursor_pos.x += text.size();
      check_status(
          expected_content, expected_is_inverse, expected_cursor_pos, &(*t));
    }
    SECTION("Printing \"\\n\" should move cursor to beginning of next line")
    {
      REQUIRE(t->print("\n\n") == 0);
      expected_cursor_pos.x = 0;
      expected_cursor_pos.y += 2;
      check_status(
          expected_content, expected_is_inverse, expected_cursor_pos, &(*t));
      for (const auto& text : {"Foo!"s, "_bar_"s, "<spam>"s}) {
        REQUIRE(t->print(text + "\n") == 0);
        expected_content[expected_cursor_pos.y].replace(
            expected_cursor_pos.x, text.size(), text);
        expected_cursor_pos.x = 0;
        expected_cursor_pos.y++;
        check_status(expected_content, expected_is_inverse,
            expected_cursor_pos, &(*t));
      }
    }
  }

  SECTION("Printing text at given position")
  {
    std::deque<std::string> expected_content(lines, empty_line);
    std::deque<std::deque<bool>> expected_is_inverse(lines, normal_line);
    SDL_Point expected_cursor_pos {0, 0};
    for (const auto& text : {"Foo!!!"s, "_bar_"s, "spam"s}) {
      REQUIRE(t->print_at(5, 2, text) == 0);
      expected_content[2].replace(5, text.size(), text);
      expected_cursor_pos.x = 5 + text.size();
      expected_cursor_pos.y = 2;
      check_status(
          expected_content, expected_is_inverse, expected_cursor_pos, &(*t));
    }
    SECTION("Printing \"\\b\' (backspace) should delete previous content")
    {
      REQUIRE(t->print("\b\b\b") == 0);
      expected_cursor_pos.x -= 3;
      expected_content[expected_cursor_pos.y].replace(
          expected_cursor_pos.x, 3, "   ");
      check_status(
          expected_content, expected_is_inverse, expected_cursor_pos, &(*t));
    }
  }
}

TEST_CASE("print() with delay", "[print][pause]")
{
  constexpr int allowed_error = 50;
  constexpr int columns = 20;
  constexpr int lines = 5;
  constexpr SDL_Point a_size {columns, lines};
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto t = remotemo::create(config);

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
  constexpr int columns = 20;
  constexpr int lines = 5;
  constexpr SDL_Point a_size {columns, lines};
  const std::string empty_line(columns, ' ');
  const std::deque<bool> normal_line(columns, false);
  auto config = setup(columns, lines);
  auto t = remotemo::create(config);
  t->set_text_delay(0);

  SECTION("Printing text from starting position")
  {
    std::deque<std::string> expected_content(lines, empty_line);
    std::deque<std::deque<bool>> expected_is_inverse(lines, normal_line);
    SDL_Point expected_cursor_pos {0, 0};

    // Fill screen:
    for (const auto& text :
        {"Foo!"s, "_bar_"s, "<spam>"s, "remoTemo"s, "Bottom"s}) {
      REQUIRE(t->print(text + "\n") == 0);
      expected_content[expected_cursor_pos.y].replace(
          expected_cursor_pos.x, text.size(), text);
      expected_cursor_pos.x = 0;
      expected_cursor_pos.y++;
    }
    check_status(
        expected_content, expected_is_inverse, expected_cursor_pos, &(*t));

    // Start scrolling:
    for (const auto& text : {"Start"s, "scrolling"s, "now"s, "Bottom"s}) {
      REQUIRE(t->print(text + "\n") == 0);
      expected_content.pop_front();
      expected_content.push_back(empty_line);
      expected_content[lines - 1].replace(
          expected_cursor_pos.x, text.size(), text);
      expected_cursor_pos.x = 0;
      expected_cursor_pos.y = lines;
      check_status(
          expected_content, expected_is_inverse, expected_cursor_pos, &(*t));
    }
  }
}
