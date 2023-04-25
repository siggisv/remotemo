#include "window.hpp"

namespace remotemo {
std::optional<Window> Window::create(
    const Window_config& window_config, Res_handler<SDL_Window>&& res_handler)
{
  Window window {std::move(res_handler)};
  if (window.res() == nullptr) {
    if (!window.setup(window_config)) {
      return {};
    }
  }
  return window;
}

bool Window::setup(const Window_config& window_config)
{
  res(::SDL_CreateWindow(window_config.title.c_str(), window_config.pos_x,
      window_config.pos_y, window_config.width, window_config.height,
      (window_config.is_resizable ? SDL_WINDOW_RESIZABLE : 0) |
          (window_config.is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)));
  if (res() == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateWindow() failed: %s\n", ::SDL_GetError());
    return false;
  }
  is_owned(true);
  return true;
}

} // namespace remotemo
