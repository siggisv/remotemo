#ifndef REMOTEMO_SRC_TEXT_DISPLAY_HPP
#define REMOTEMO_SRC_TEXT_DISPLAY_HPP

#include <string>
#include <utility>
#include <optional>
#include <deque>
#include <vector>

#include "remotemo/config.hpp"
#include "texture.hpp"
#include "font.hpp"
#include <SDL.h>

namespace remotemo {
struct Display_square {
  char character {' '};
  bool is_inversed {false};
};

class Text_display : public Texture {
public:
  Text_display(Font&& font, SDL_Texture* texture, SDL_Renderer* renderer,
      const Text_area_config& text_area_config) noexcept
      : Texture(texture, true), m_renderer(renderer), m_font(std::move(font)),
        m_columns(text_area_config.columns), m_lines(text_area_config.lines),
        m_blend_to_screen_mode(text_area_config.blend_mode),
        m_text_to_screen_color(text_area_config.color),
        m_empty_line(m_columns), m_display_content(m_lines, m_empty_line)
  {}

  static std::optional<Text_display> create(Font&& font,
      const Text_area_config& text_area_config, SDL_Renderer* renderer);
  [[nodiscard]] const Font& font() const { return m_font; }
  [[nodiscard]] int columns() const { return m_columns; }
  [[nodiscard]] int lines() const { return m_lines; }
  [[nodiscard]] Point cursor_pos() const { return m_cursor_pos; }
  [[nodiscard]] char char_at(const Point& pos) const;
  [[nodiscard]] bool is_inverse_at(const Point& pos) const;
  void cursor_pos(const Point& pos);
  void update_cursor();
  void show_char_hidden_by_cursor();
  void is_output_inversed(bool inverse) { m_is_output_inversed = inverse; }
  [[nodiscard]] bool is_output_inversed() const
  {
    return m_is_output_inversed;
  }
  void set_char_at_cursor(int character);
  void scroll_up_one_line();
  void clear_line(int line);
  void refresh_display();

private:
  void display_char_at(
      int character, bool is_output_inverse, const Point& pos);

  SDL_Renderer* m_renderer;
  Font m_font;
  int m_columns;
  int m_lines;
  SDL_BlendMode m_blend_to_screen_mode;
  Color m_text_to_screen_color;
  std::vector<Display_square> m_empty_line;
  std::deque<std::vector<Display_square>> m_display_content;
  Point m_cursor_pos {0, 0};
  bool m_is_cursor_visible {true};
  bool m_is_cursor_updated {false};
  bool m_is_output_inversed {false};
  static constexpr int max_ascii_value {127};
  static constexpr int bitmap_char_per_line {16};
  static constexpr Point space_position {0, 2};
  static constexpr int bitmap_lines_per_mode {8};
  static constexpr char not_ascii_symbol {1};
  static constexpr char cursor_symbol {0};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_TEXT_DISPLAY_HPP
