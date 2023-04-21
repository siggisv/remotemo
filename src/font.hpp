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
      : Texture(std::move(font_texture)), m_width(font_config.width),
        m_height(font_config.height)
  {}

  static std::optional<Font> create(const Font_config& font_config,
      Res_handler<SDL_Texture>&& font_texture, SDL_Renderer* renderer);
  [[nodiscard]] const int& width() const { return m_width; }
  [[nodiscard]] const int& height() const { return m_height; }

private:
  int m_width;
  int m_height;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_FONT_HPP
