#ifndef REMOTEMO_SRC_BACKGROUND_HPP
#define REMOTEMO_SRC_BACKGROUND_HPP

#include <utility>
#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include "texture.hpp"
#include <SDL.h>

namespace remotemo {
class Background : public Texture {
public:
  Background(
      const Backgr_config& backgr_config, Texture&& backgr_texture) noexcept
      : Texture(std::move(backgr_texture)),
        m_min_area(backgr_config.min_area),
        m_text_area(backgr_config.text_area)
  {}

  static std::optional<Background> create(const Backgr_config& backgr_config,
      Texture&& backgr_texture, SDL_Renderer* renderer);
  const SDL_Rect& min_area() const { return m_min_area; }
  const SDL_FRect& text_area() const { return m_text_area; }

private:
  SDL_Rect m_min_area;
  SDL_FRect m_text_area;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_BACKGROUND_HPP
