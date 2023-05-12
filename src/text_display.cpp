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
  Text_display text_display {
      std::move(font), texture, renderer, text_area_config};
  text_display.texture_size(area_size);
  return text_display;
}
void Text_display::hide_cursor()
{
  // TODO update display to show character at cursor position
  m_is_cursor_visible = false;
}

void Text_display::show_cursor()
{
  m_is_cursor_visible = true;
  char_at_cursor(cursor_symbol);
}
void Text_display::char_at_cursor(int character)
{
  // TODO Keep track of console content
  if (character < 0 || character > max_ascii_value) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "The character \'%c\' (%d) is not an ASCII character.\n", character,
        character);
    character = not_ascii_symbol;
  }
  int bitmap_char_column = character % bitmap_char_per_line;
  int bitmap_char_line = character / bitmap_char_per_line;
  if (m_is_output_inversed) {
    bitmap_char_line += bitmap_lines_per_mode;
  }
  SDL_SetRenderTarget(m_renderer, res());
  SDL_Rect display_target_area {1 + m_cursor_pos.x * m_font.char_width(),
      1 + m_cursor_pos.y * m_font.char_height(), m_font.char_width(),
      m_font.char_height()};
  SDL_Rect bitmap_char_area {bitmap_char_column * m_font.char_width(),
      bitmap_char_line * m_font.char_height(), m_font.char_width(),
      m_font.char_height()};
  float tmp_scale_x = NAN;
  float tmp_scale_y = NAN;
  SDL_RenderGetScale(m_renderer, &tmp_scale_x, &tmp_scale_y);
  SDL_RenderSetScale(m_renderer, 1, 1);
  SDL_RenderCopy(
      m_renderer, m_font.res(), &bitmap_char_area, &display_target_area);
  SDL_RenderSetScale(m_renderer, tmp_scale_x, tmp_scale_y);
  SDL_SetRenderTarget(m_renderer, nullptr);
}

} // namespace remotemo
