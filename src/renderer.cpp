#include "renderer.hpp"

namespace remotemo {
std::optional<Renderer> Renderer::create(
    SDL_Window* window, Res_handler<SDL_Renderer>&& res_handler)
{
  Renderer renderer {std::move(res_handler)};
  if (renderer.res() == nullptr) {
    if (!renderer.setup(window)) {
      return {};
    }
  }
  return renderer;
}

bool Renderer::setup(SDL_Window* window)
{
  res(::SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE));
  if (res() == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateRenderer() failed: %s\n", ::SDL_GetError());
    return false;
  }
  is_owned(true);
  return true;
}
} // namespace remotemo
