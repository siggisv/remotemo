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
  Text_display(Font&& font, SDL_Texture* texture, SDL_Renderer* renderer,
      const Text_area_config& text_area_config) noexcept
      : Texture(texture, true), m_renderer(renderer), m_font(std::move(font)),
        m_columns(text_area_config.columns), m_lines(text_area_config.lines)
  {}

  static std::optional<Text_display> create(Font&& font,
      const Text_area_config& text_area_config, SDL_Renderer* renderer);
  [[nodiscard]] const Font& font() const { return m_font; }
  [[nodiscard]] int columns() const { return m_columns; }
  [[nodiscard]] int lines() const { return m_lines; }
  [[nodiscard]] SDL_Point cursor_pos() const { return m_cursor_pos; }
  void cursor_pos(const SDL_Point& pos) { m_cursor_pos = pos; }
  void hide_cursor();
  void show_cursor();
  void is_output_inversed(bool inverse) { m_is_output_inversed = inverse; }
  [[nodiscard]] bool is_output_inversed() const
  {
    return m_is_output_inversed;
  }
  void char_at_cursor(int character);

private:
  SDL_Renderer* m_renderer;
  Font m_font;
  int m_columns;
  int m_lines;
  SDL_Point m_cursor_pos {0, 0};
  bool m_is_cursor_visible {true};
  bool m_is_output_inversed {false};
  static constexpr int max_ascii_value {127};
  static constexpr int bitmap_char_per_line {16};
  static constexpr int bitmap_lines_per_mode {8};
  static constexpr char not_ascii_symbol {1};
  static constexpr char cursor_symbol {0};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_TEXT_DISPLAY_HPP
