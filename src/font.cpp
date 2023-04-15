#include "font.hpp"

namespace remotemo {

std::optional<Font> Font::create(
    const Font_config& font_config, bool is_owned, SDL_Renderer* renderer)
{
  Font font {font_config, is_owned};
  if (font_config.raw_sdl == nullptr) {
    if (!font.load(renderer, font_config.file_path)) {
      return {};
    }
  }
  return font;
}
} // namespace remotemo
