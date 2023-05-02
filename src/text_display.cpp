#include "text_display.hpp"

namespace remotemo {
std::optional<Text_display> Text_display::create(Font&& font,
    const Text_area_config& text_area_config, SDL_Renderer* renderer)
{
  SDL_Point area_size {(font.char_width() * text_area_config.columns) + 2,
      (font.char_height() * text_area_config.lines) + 2};
  auto* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
      SDL_TEXTUREACCESS_TARGET, area_size.x, area_size.y);
  if (texture == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateTexture() failed: %s\n", ::SDL_GetError());
    return {};
  }
  SDL_SetTextureBlendMode(texture, text_area_config.blend_mode);
  SDL_SetTextureColorMod(texture, text_area_config.color.red,
      text_area_config.color.green, text_area_config.color.blue);
  Text_display text_display {std::move(font), texture, text_area_config};
  text_display.texture_size(area_size);
  return text_display;
}
} // namespace remotemo
