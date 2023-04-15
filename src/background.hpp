#ifndef REMOTEMO_SRC_BACKGROUND_HPP
#define REMOTEMO_SRC_BACKGROUND_HPP

#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include "texture.hpp"
#include <SDL.h>

namespace remotemo {
class Background : public Texture {
public:
  constexpr Background() noexcept = default;
  constexpr Background(
      const Backgr_config& backgr_config, bool is_owned) noexcept
      : Texture(backgr_config.raw_sdl, is_owned),
        m_min_area(backgr_config.min_area),
        m_text_area(backgr_config.text_area)
  {}

  static std::optional<Background> create(const Backgr_config& backgr_config,
      bool is_owned, SDL_Renderer* renderer);

protected:
  SDL_Rect m_min_area {0, 0, 0, 0};
  SDL_FRect m_text_area {0.0f, 0.0f, 0.0f, 0.0f};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_BACKGROUND_HPP
