#include <remotemo/remotemo.hpp>
#include <SDL_image.h>

#include <filesystem>

#include "engine.hpp"

namespace remotemo {
Remotemo::~Remotemo() noexcept = default;
Remotemo::Remotemo(Remotemo&& other) noexcept
    : m_engine(std::move(other.m_engine))
{}
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
} // namespace remotemo
