#include "text_display.hpp"

namespace remotemo {
std::optional<Text_display> Text_display::create(Font&& font,
    const Text_area_config& text_area_config, SDL_Renderer* renderer)
{
  // + 2 pixels to have an empty, 1 pixel wide, border around the content.
  // That seems to be needed so that when stretching the content to the
  // screen, then the outer border of the content gets the same look as the
  // rest of the content:
  Size area_size {(font.char_width() * text_area_config.columns) + 2,
      (font.char_height() * (text_area_config.lines)) + 2};
  auto* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
      SDL_TEXTUREACCESS_TARGET, area_size.width, area_size.height);
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


char Text_display::char_at(const Point& pos) const
{
  return m_display_content[pos.y][pos.x].character;
}

bool Text_display::is_inverse_at(const Point& pos) const
{
  return m_display_content[pos.y][pos.x].is_inversed;
}

void Text_display::cursor_pos(const Point& pos)
{
  if (m_cursor_pos.x != pos.x || m_cursor_pos.y != pos.y) {
    show_char_hidden_by_cursor();
    m_cursor_pos = pos;
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
  m_is_cursor_updated = true;
  if (m_is_cursor_visible) {
    display_char_at(cursor_symbol, m_is_output_inversed, m_cursor_pos);
  } else {
    show_char_hidden_by_cursor();
  }
}

void Text_display::show_char_hidden_by_cursor()
{
  if (m_cursor_pos.x >= m_columns || m_cursor_pos.y >= m_lines) {
    return;
  }
  auto& content_at_cursor = m_display_content[m_cursor_pos.y][m_cursor_pos.x];
  display_char_at(content_at_cursor.character, content_at_cursor.is_inversed,
      m_cursor_pos);
}

void Text_display::set_char_at_cursor(int character)
{
  if (m_cursor_pos.x >= m_columns || m_cursor_pos.y >= m_lines) {
    return;
  }
  if (character < 0 || character > max_ascii_value) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "The character \'%c\' (%d) is not an ASCII character.\n", character,
        character);
    character = not_ascii_symbol;
  }
  auto& content_at_cursor = m_display_content[m_cursor_pos.y][m_cursor_pos.x];
  content_at_cursor.character = static_cast<char>(character);
  content_at_cursor.is_inversed = m_is_output_inversed;
}

void Text_display::clear_line(int line)
{
  SDL_SetRenderTarget(m_renderer, res());

  int line_length = texture_size().width - 2;
  int line_height = m_font.char_height();
  // NOTE Following can easily be changed to take 'is_inverse' into account:
  SDL_Rect space_bitmap = {space_position.x * m_font.char_width(),
      space_position.y * m_font.char_height(), m_font.char_width(),
      m_font.char_height()};
  SDL_Rect target_area = {
      1, 1 + (line * line_height), line_length, line_height};
  SDL_RenderCopy(m_renderer, m_font.res(), &space_bitmap, &target_area);

  m_display_content[line] = m_empty_line;
  m_has_texture_changed = true;
  if (line == m_cursor_pos.y) {
    m_is_cursor_updated = false;
  }
  SDL_SetRenderTarget(m_renderer, nullptr);
}

void Text_display::scroll_up_one_line()
{
  m_display_content.push_back(m_empty_line);
  m_display_content.pop_front();
  m_is_texture_refresh_needed = true;
}

void Text_display::refresh_texture()
{
  for (int line = 0; line < m_lines; line++) {
    for (int column = 0; column < m_columns; column++) {
      auto& content = m_display_content[line][column];
      display_char_at(
          content.character, content.is_inversed, Point {column, line});
    }
  }
  m_is_texture_refresh_needed = false;
  m_is_cursor_updated = false;
  //update_cursor();
}

void Text_display::display_char_at(
    int character, bool is_output_inversed, const Point& pos)
{
  if (pos.x >= m_columns || pos.y >= m_lines) {
    return;
  }
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
  m_has_texture_changed = true;
  SDL_SetRenderTarget(m_renderer, nullptr);
}

} // namespace remotemo
