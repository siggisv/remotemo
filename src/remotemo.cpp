#include <remotemo/remotemo.hpp>

#include "engine.hpp"

namespace remotemo {
Remotemo::~Remotemo() noexcept = default;
Remotemo::Remotemo(Remotemo&& other) noexcept = default;
Remotemo& Remotemo::operator=(Remotemo&& other) noexcept = default;

void Remotemo::engine(std::unique_ptr<Engine> engine)
{
  m_engine = std::move(engine);
}

bool Remotemo::initialize(const Config& config)
{
  m_engine = Engine::create(config);
  return (m_engine != nullptr);
}

std::optional<Remotemo> create()
{
  return create(Config {});
}

std::optional<Remotemo> create(const Config& config)
{
  Remotemo temo {};
  if (temo.initialize(config)) {
    return temo;
  }
  return {};
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
  return 0;
}
int Remotemo::set_cursor_column([[maybe_unused]] int column)
{
  return 0;
}
int Remotemo::set_cursor_line([[maybe_unused]] int line)
{
  return 0;
}
SDL_Point Remotemo::get_cursor_position()
{
  return SDL_Point {0, 0};
}

int Remotemo::pause([[maybe_unused]] int pause)
{
  return 0;
}
int Remotemo::clear()
{
  return 0;
}
Key Remotemo::get_key()
{
  return Key::K_0;
}
std::string Remotemo::get_input([[maybe_unused]] int max_length)
{
  return "null";
}

int Remotemo::print([[maybe_unused]] const std::string& text)
{
  return 0;
}
int Remotemo::print_at([[maybe_unused]] int column, [[maybe_unused]] int line,
    [[maybe_unused]] const std::string& text)
{
  return 0;
}
int Remotemo::set_inverse([[maybe_unused]] bool inverse)
{
  return 0;
}
} // namespace remotemo
