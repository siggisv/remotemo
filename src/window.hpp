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

private:
  bool setup(const Window_config& window_config);
};
} // namespace remotemo
#endif // REMOTEMO_SRC_WINDOW_HPP
