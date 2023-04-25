#include "text_display.hpp"

namespace remotemo {
std::optional<Text_display> Text_display::create(Font&& font,
    const Text_area_config& text_area_config, SDL_Renderer* renderer)
{
  auto* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
      SDL_TEXTUREACCESS_TARGET, (font.width() * text_area_config.columns) + 2,
      (font.height() * text_area_config.lines) + 2);
  if (texture == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateTexture() failed: %s\n", ::SDL_GetError());
    return {};
  }
  SDL_SetTextureBlendMode(texture, text_area_config.blend_mode);
  SDL_SetTextureColorMod(texture, text_area_config.color.red,
      text_area_config.color.green, text_area_config.color.blue);
  return Text_display {std::move(font), texture, text_area_config};
}
} // namespace remotemo
