#include <remotemo/remotemo.hpp>

#include "engine.hpp"

namespace remotemo {
Remotemo::~Remotemo() noexcept = default;
Remotemo::Remotemo(Remotemo&& other) noexcept = default;
Remotemo& Remotemo::operator=(Remotemo&& other) noexcept = default;

Remotemo::Remotemo(std::unique_ptr<Engine> engine,
    [[maybe_unused]] const Config& config) noexcept
    : m_engine(std::move(engine))
{
  m_engine->render_window();
}

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

int Remotemo::move_cursor(const SDL_Point& move)
{
  auto new_pos = m_engine->cursor_pos();
  auto area_size = m_engine->text_area_size();
  int return_code = 0;
  new_pos.x += move.x;
  if (new_pos.x >= area_size.x) {
    new_pos.x = area_size.x - 1;
    return_code -= 1;
  } else if (new_pos.x < 0) {
    new_pos.x = 0;
    return_code -= 4;
  }
  new_pos.y += move.y;
  // NOTE Cursor is allowed to be one line below visible area:
  if (new_pos.y > area_size.y) {
    new_pos.y = area_size.y;
    return_code -= 2;
  } else if (new_pos.y < 0) {
    new_pos.y = 0;
    return_code -= 8;
  }
  m_engine->cursor_pos(new_pos);
  return return_code;
}
int Remotemo::set_cursor(const SDL_Point& pos)
{
  auto area_size = m_engine->text_area_size();
  if (pos.x < 0 || pos.x >= area_size.x ||
      // NOTE Cursor is allowed to be one line below visible area:
      pos.y < 0 || pos.y > area_size.y) {
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
SDL_Point Remotemo::get_cursor_position()
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
int Remotemo::clear()
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return 0;
}
Key Remotemo::get_key()
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return Key::K_0;
}
std::string Remotemo::get_input([[maybe_unused]] int max_length)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return "null";
}

int Remotemo::print([[maybe_unused]] const std::string& text)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return 0;
}
int Remotemo::print_at(int column, int line, const std::string& text)
{
  auto result = set_cursor(column, line);
  if (result != 0) {
    return result;
  }
  return print(text);
}
int Remotemo::set_inverse([[maybe_unused]] bool inverse)
{
  return 0;
}
} // namespace remotemo
