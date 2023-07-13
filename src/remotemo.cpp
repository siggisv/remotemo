#include <remotemo/remotemo.hpp>

#include <sstream>
#include <vector>

#include "version.hpp"
#include "engine.hpp"

namespace remotemo {
Remotemo::~Remotemo() noexcept = default;
Remotemo::Remotemo(Remotemo&& other) noexcept = default;
Remotemo& Remotemo::operator=(Remotemo&& other) noexcept = default;

Remotemo::Remotemo(std::unique_ptr<Engine> engine,
    [[maybe_unused]] const Config& config) noexcept
    : m_engine(std::move(engine))
{}

std::optional<Remotemo> create()
{
  return create(Config {});
}

std::optional<Remotemo> create(const Config& config)
{
  auto engine = Engine::create(config);
  if (engine == nullptr) {
    return {};
  }
  return Remotemo {std::move(engine), config};
}

std::string full_name()
{
  std::stringstream full_n;
  full_n << "remoTemo v" << version_.full;
  return full_n.str();
}

Version version()
{
  return version_;
}

bool version_is_pre_release()
{
  return !version_.pre_release_label.empty();
}


int Remotemo::move_cursor(const Size& move)
{
  auto new_pos = m_engine->cursor_pos();
  auto area_size = m_engine->text_area_size();
  int return_code = 0;
  new_pos.x += move.width;
  if (new_pos.x >= area_size.width) {
    new_pos.x = area_size.width - 1;
    return_code += static_cast<int>(Move_cursor_error::past_right_edge);
  } else if (new_pos.x < 0) {
    new_pos.x = 0;
    return_code += static_cast<int>(Move_cursor_error::past_left_edge);
  }
  new_pos.y += move.height;
  // NOTE Cursor is allowed to be one line below visible area:
  if (new_pos.y > area_size.height) {
    new_pos.y = area_size.height;
    return_code += static_cast<int>(Move_cursor_error::past_bottom_edge);
  } else if (new_pos.y < 0) {
    new_pos.y = 0;
    return_code += static_cast<int>(Move_cursor_error::past_top_edge);
  }
  m_engine->cursor_pos(new_pos);
  return return_code;
}

int Remotemo::set_cursor(const Point& pos)
{
  auto area_size = m_engine->text_area_size();
  if (pos.x < 0 || pos.x >= area_size.width ||
      // NOTE Cursor is allowed to be one line below visible area:
      pos.y < 0 || pos.y > area_size.height) {
    m_engine->main_loop_once();
    return -1;
  }
  m_engine->cursor_pos(pos);
  return 0;
}

int Remotemo::set_cursor_column(int column)
{
  auto pos = m_engine->cursor_pos();
  pos.x = column;
  return set_cursor(pos);
}

int Remotemo::set_cursor_line(int line)
{
  auto pos = m_engine->cursor_pos();
  pos.y = line;
  return set_cursor(pos);
}

Point Remotemo::get_cursor_position() const
{
  return m_engine->cursor_pos();
}

int Remotemo::pause(int pause_in_ms)
{
  if (pause_in_ms < 0) {
    return -1;
  }
  m_engine->delay(pause_in_ms);
  return 0;
}

void Remotemo::clear(Do_reset do_reset)
{
  auto old_cursor_pos = m_engine->cursor_pos();
  m_engine->clear_screen();
  if (do_reset == Do_reset::inverse || do_reset == Do_reset::all) {
    set_inverse(false);
  }
  if (do_reset == Do_reset::cursor || do_reset == Do_reset::all) {
    set_cursor(0, 0);
  } else {
    set_cursor(old_cursor_pos);
  }
}

Key Remotemo::get_key()
{
  return m_engine->get_key();
}

std::string Remotemo::get_input([[maybe_unused]] int max_length)
{
  // TODO Implement this function
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return "null";
}

int Remotemo::print(const std::string& text)
{
  // TODO Implement wrap being set to 'word'
  if (!m_engine->display_string_at_cursor(text, m_text_wrapping)) {
    return -2;
  }
  return 0;
}

int Remotemo::print_at(const Point& pos, const std::string& text)
{
  auto result = set_cursor(pos);
  if (result != 0) {
    return result;
  }
  return print(text);
}

char Remotemo::get_char_at(const Point& pos) const
{
  auto area_size = m_engine->text_area_size();
  if (pos.x < 0 || pos.x >= area_size.width ||
      // NOTE Although the cursor is allowed to be one line below the screen,
      // there is no content there.
      pos.y < 0 || pos.y > area_size.height) {
    return 0;
  }
  return m_engine->char_at(pos);
}

bool Remotemo::is_inverse_at(const Point& pos) const
{
  auto area_size = m_engine->text_area_size();
  if (pos.x < 0 || pos.x >= area_size.width ||
      // NOTE Although the cursor is allowed to be one line below the screen,
      // there is no content there.
      pos.y < 0 || pos.y > area_size.height) {
    return false;
  }
  return m_engine->is_inverse_at(pos);
}

bool Remotemo::set_text_delay(int delay_in_ms)
{
  if (delay_in_ms < 0) {
    return false;
  }
  m_engine->delay_between_chars_ms(delay_in_ms);
  return true;
}

constexpr double ms_per_second = 1000.0;

bool Remotemo::set_text_speed(double char_per_second)
{
  if (char_per_second <= 0) {
    return false;
  }
  return set_text_delay(static_cast<int>(ms_per_second / char_per_second));
}

int Remotemo::get_text_delay() const
{
  return m_engine->delay_between_chars_ms();
}

double Remotemo::get_text_speed() const
{
  return ms_per_second / get_text_delay();
}

void Remotemo::set_inverse(bool inverse)
{
  m_engine->is_output_inversed(inverse);
}

bool Remotemo::get_inverse() const
{
  return m_engine->is_output_inversed();
}

void Remotemo::set_scrolling(bool is_scrolling)
{
  m_engine->is_scrolling_allowed(is_scrolling);
}

bool Remotemo::get_scrolling() const
{
  return m_engine->is_scrolling_allowed();
}

void Remotemo::set_wrapping(Wrapping wrapping)
{
  m_text_wrapping = wrapping;
  if (wrapping == Wrapping::word) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "Wrapping whole words not implemented yet. "
        "Behaves as if set to 'character'.");
  }
}

} // namespace remotemo
