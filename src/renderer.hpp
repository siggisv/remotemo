#ifndef REMOTEMO_SRC_RENDERER_HPP
#define REMOTEMO_SRC_RENDERER_HPP

#include <utility>
#include <optional>

#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include <SDL.h>

namespace remotemo {
class Renderer : public Res_handler<SDL_Renderer> {
public:
  constexpr explicit Renderer(
      SDL_Window* window, bool is_owned = true) noexcept
      : Res_handler<SDL_Renderer>(
            window == nullptr ? nullptr : ::SDL_GetRenderer(window), is_owned)
  {}
  constexpr explicit Renderer(
      Res_handler<SDL_Renderer>&& res_handler) noexcept
      : Res_handler<SDL_Renderer>(std::move(res_handler))
  {}

  static std::optional<Renderer> create(
      SDL_Window* window, Res_handler<SDL_Renderer>&& res_handler);

private:
  bool setup(SDL_Window* window);
};
} // namespace remotemo
#endif // REMOTEMO_SRC_RENDERER_HPP
