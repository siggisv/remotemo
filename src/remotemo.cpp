#include <remotemo/remotemo.hpp>

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

int Remotemo::move_cursor([[maybe_unused]] int x, [[maybe_unused]] int y)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return 0;
}
int Remotemo::set_cursor(
    [[maybe_unused]] int column, [[maybe_unused]] int line)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return 0;
}
int Remotemo::set_cursor_column([[maybe_unused]] int column)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return 0;
}
int Remotemo::set_cursor_line([[maybe_unused]] int line)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return 0;
}
SDL_Point Remotemo::get_cursor_position()
{
  return SDL_Point {0, 0};
}

int Remotemo::pause([[maybe_unused]] int pause)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
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
int Remotemo::print_at([[maybe_unused]] int column, [[maybe_unused]] int line,
    [[maybe_unused]] const std::string& text)
{
  m_engine->main_loop_once();
  m_engine->main_loop_once();
  return 0;
}
int Remotemo::set_inverse([[maybe_unused]] bool inverse)
{
  return 0;
}
} // namespace remotemo
