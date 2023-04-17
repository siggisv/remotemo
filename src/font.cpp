#include "font.hpp"

namespace remotemo {

std::optional<Font> Font::create(const Font_config& font_config,
    Texture&& font_texture, SDL_Renderer* renderer)
{
  Font font {font_config, std::move(font_texture)};
  if (font_config.raw_sdl == nullptr) {
    if (!font.load(renderer, font_config.file_path)) {
      return {};
    }
  }
  return font;
}
} // namespace remotemo
