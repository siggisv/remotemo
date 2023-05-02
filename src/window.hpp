#ifndef REMOTEMO_SRC_WINDOW_HPP
#define REMOTEMO_SRC_WINDOW_HPP

#include <string>
#include <utility>
#include <optional>

#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include <SDL.h>

namespace remotemo {
class Window : public Res_handler<SDL_Window> {
public:
  constexpr explicit Window(Res_handler<SDL_Window>&& res_handler) noexcept
      : Res_handler<SDL_Window>(std::move(res_handler))
  {}

  static std::optional<Window> create(const Window_config& window_config,
      Res_handler<SDL_Window>&& res_handler);
  void refresh_local_size();
  [[nodiscard]] const SDL_Point& size() { return m_size; }

private:
  bool setup(const Window_config& window_config);

  SDL_Point m_size {0, 0};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_WINDOW_HPP
