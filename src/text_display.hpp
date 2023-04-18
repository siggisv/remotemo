#ifndef REMOTEMO_SRC_TEXT_DISPLAY_HPP
#define REMOTEMO_SRC_TEXT_DISPLAY_HPP

#include <string>
#include <utility>
#include <optional>

#include "remotemo/config.hpp"
#include "texture.hpp"
#include "font.hpp"
#include <SDL.h>

namespace remotemo {
class Text_display : public Texture {
public:
  Text_display(Font&& font, SDL_Texture* texture,
      const Text_area_config& text_area_config) noexcept
      : Texture(texture, true), m_font(std::move(font)),
        m_columns(text_area_config.columns), m_lines(text_area_config.lines)
  {}

  static std::optional<Text_display> create(Font&& font,
      const Text_area_config& text_area_config, SDL_Renderer* renderer);

private:
  Font m_font;
  int m_columns;
  int m_lines;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_TEXT_DISPLAY_HPP
