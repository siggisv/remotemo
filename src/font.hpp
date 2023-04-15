#ifndef REMOTEMO_SRC_FONT_HPP
#define REMOTEMO_SRC_FONT_HPP

#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include "texture.hpp"
#include <SDL.h>

namespace remotemo {
class Font : public Texture {
public:
  constexpr Font() noexcept = default;
  constexpr Font(const Font_config& font_config, bool is_owned) noexcept
      : Texture(font_config.raw_sdl, is_owned), m_width(font_config.width),
        m_height(font_config.height)
  {}

  static std::optional<Font> create(
      const Font_config& font_config, bool is_owned, SDL_Renderer* renderer);

protected:
  int m_width {0};
  int m_height {0};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_FONT_HPP
