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
  window.refresh_local_flags();
  window.refresh_local_size();
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

void Window::refresh_local_size()
{
  ::SDL_GetWindowSize(res(), &m_size.width, &m_size.height);
}

void Window::refresh_local_flags()
{
  auto window_flags = SDL_GetWindowFlags(res());
  m_is_fullscreen = ((window_flags & SDL_WINDOW_FULLSCREEN) != 0);
  m_is_visible =
      ((window_flags & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED)) == 0);
}

void Window::set_fullscreen(bool do_make_fullscreen)
{
  if (do_make_fullscreen) {
    SDL_SetWindowFullscreen(res(), SDL_WINDOW_FULLSCREEN_DESKTOP);
  } else {
    SDL_SetWindowFullscreen(res(), 0);
  }
  refresh_local_flags();
}

} // namespace remotemo
