#include <remotemo/remotemo.hpp>
#include <SDL_image.h>

namespace remoTemo {
Temo::~Temo() noexcept
{
  if (m_cleanup_all) {
    SDL_Quit();
  } else {
    SDL_QuitSubSystem(sdl_init_flags);
  }
}
Temo::Temo(Temo&& other) noexcept
    : m_cleanup_all(std::exchange(other.m_cleanup_all, false))
{}
bool Temo::initialize(const Config& config)
{
  if (SDL_Init(sdl_init_flags) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init() failed: %s\n",
        SDL_GetError());
    return false;
  } else {
    m_cleanup_all = config.m_cleanup_all;
  }
  return true;
}
std::optional<Temo> create()
{
  return create(Config {});
}

std::optional<Temo> create(const Config& config)
{
  Temo temo {};
  if (temo.initialize(config)) {
    return temo;
  } else {
    return {};
  }
}
} // namespace remoTemo
