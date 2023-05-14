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


void Text_display::cursor_pos(const SDL_Point& pos)
{
  if (m_cursor_pos.x != pos.x || m_cursor_pos.y != pos.y) {
    show_char_hidden_by_cursor();
    m_cursor_pos = pos;
    // TODO Keep track of whole console content, not just under cursor
    // The following is just dummy content:
    m_char_at_cursor = ' ';
    m_inversed_at_cursor = m_is_output_inversed;
  }
  // TODO When implementing blinking cursor, set cursor to being shown and
  // reset timer.
  m_is_cursor_updated = false;
}

void Text_display::update_cursor()
{
  if (m_is_cursor_updated) {
    return;
  }
  if (m_is_cursor_visible) {
    display_char_at(cursor_symbol, m_is_output_inversed, m_cursor_pos);
  } else {
    show_char_hidden_by_cursor();
  }
  m_is_cursor_updated = true;
}

void Text_display::show_char_hidden_by_cursor()
{
  // TODO Keep track of whole console content, not just under cursor
  display_char_at(m_char_at_cursor, m_inversed_at_cursor, m_cursor_pos);
}

void Text_display::set_char_at_cursor(int character)
{
  if (character < 0 || character > max_ascii_value) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "The character \'%c\' (%d) is not an ASCII character.\n", character,
        character);
    character = not_ascii_symbol;
  }
  // TODO Keep track of whole console content, not just under cursor
  m_char_at_cursor = character;
  m_inversed_at_cursor = m_is_output_inversed;
}

void Text_display::scroll_up_one_line()
{
  SDL_SetRenderTarget(m_renderer, res());
  SDL_SetTextureBlendMode(res(), SDL_BLENDMODE_NONE);
  SDL_SetTextureColorMod(res(), 255, 255, 255);

  int line_length = texture_size().x - 2;
  int line_height = m_font.char_height();
  int scrolling_lines_height = texture_size().y - 2 - line_height;
  SDL_Rect area_to_be_moved = {
      1, 1 + line_height, line_length, scrolling_lines_height};
  SDL_Rect target_area = {1, 1, line_length, scrolling_lines_height};
  SDL_RenderCopy(m_renderer, res(), &area_to_be_moved, &target_area);

  SDL_Rect bottom_line = {
      1, 1 + scrolling_lines_height, line_length, line_height};
  SDL_Rect empty_pixel = {0, 0, 1, 1};
  SDL_RenderCopy(m_renderer, res(), &empty_pixel, &bottom_line);

  SDL_SetRenderTarget(m_renderer, nullptr);
  SDL_SetTextureBlendMode(res(), m_blend_to_screen_mode);
  SDL_SetTextureColorMod(res(), m_text_to_screen_color.red,
      m_text_to_screen_color.green, m_text_to_screen_color.blue);
}

void Text_display::display_char_at(
    int character, bool is_output_inversed, const SDL_Point& pos)
{
  SDL_SetRenderTarget(m_renderer, res());
  SDL_Rect display_target_area {1 + pos.x * m_font.char_width(),
      1 + pos.y * m_font.char_height(), m_font.char_width(),
      m_font.char_height()};
  int bitmap_char_column = character % bitmap_char_per_line;
  int bitmap_char_line = character / bitmap_char_per_line;
  if (is_output_inversed) {
    bitmap_char_line += bitmap_lines_per_mode;
  }
  SDL_Rect bitmap_char_area {bitmap_char_column * m_font.char_width(),
      bitmap_char_line * m_font.char_height(), m_font.char_width(),
      m_font.char_height()};
  SDL_RenderCopy(
      m_renderer, m_font.res(), &bitmap_char_area, &display_target_area);
  SDL_SetRenderTarget(m_renderer, nullptr);
}

} // namespace remotemo
