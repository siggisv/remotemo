#ifndef REMOTEMO_SRC_FONT_HPP
#define REMOTEMO_SRC_FONT_HPP

#include <utility>
#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include "texture.hpp"
#include <SDL.h>

namespace remotemo {
class Font : public Texture {
public:
  Font(const Font_config& font_config, Texture&& font_texture) noexcept
      : Texture(std::move(font_texture)), m_char_width(font_config.width),
        m_char_height(font_config.height)
  {}

  static std::optional<Font> create(const Font_config& font_config,
      Res_handler<SDL_Texture>&& font_texture, SDL_Renderer* renderer);
  [[nodiscard]] int char_width() const { return m_char_width; }
  [[nodiscard]] int char_height() const { return m_char_height; }

private:
  int m_char_width;
  int m_char_height;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_FONT_HPP
