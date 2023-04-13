#ifndef REMOTEMO_CONFIG_HPP
#define REMOTEMO_CONFIG_HPP

#include <optional>
#include <functional>
#include <string>

#include "remotemo/common_types.hpp"

#include <SDL.h>

namespace remotemo {
using std::string_literals::operator""s;

class Engine;
class Texture;

struct Texture_config {
  SDL_Texture* raw_sdl;
  std::string file_path;
};

class Config {
  friend Engine;
  friend Texture;

public:
  constexpr Config() = default;

  Config& cleanup_all(bool cleanup_all);
  [[nodiscard]] const bool& cleanup_all() const { return m_cleanup_all; }

  Config& window(SDL_Window* window);
  Config& window_title(const std::string& title);
  Config& window_size(int width, int height);
  Config& window_size(const SDL_Point& size);
  Config& window_position(int x, int y);
  Config& window_position(const SDL_Point& pos);
  Config& window_resizable(bool is_resizable);
  Config& window_fullscreen(bool is_fullscreen);

  Config& key_fullscreen();
  Config& key_fullscreen(Mod_keys modifier_keys, F_key key);
  Config& key_fullscreen(Mod_keys_strict modifier_keys, Key key);

  Config& key_close_window();
  Config& key_close_window(Mod_keys modifier_keys, F_key key);
  Config& key_close_window(Mod_keys_strict modifier_keys, Key key);

  Config& key_quit();
  Config& key_quit(Mod_keys modifier_keys, F_key key);
  Config& key_quit(Mod_keys_strict modifier_keys, Key key);

  Config& closing_same_as_quit(bool is_closing_same_as_quit);
  Config& function_pre_close(const std::function<bool()>& func);
  Config& function_pre_quit(const std::function<bool()>& func);

  Config& background(SDL_Texture* background);
  Config& background_file_path(const std::string& file_path);
  Config& background_min_area(int x, int y, int w, int h);
  Config& background_text_area(float x, float y, float w, float h);

  Config& font_bitmap(SDL_Texture* font_bitmap);
  Config& font_bitmap_file_path(const std::string& file_path);
  Config& font_size(int width, int height);
  Config& font_size(const SDL_Point& size);

  Config& text_area_size(int columns, int lines);
  Config& text_area_size(const SDL_Point& size);
  Config& text_blend_mode(SDL_BlendMode mode);
  Config& text_color(Uint8 red, Uint8 green, Uint8 blue);
  Config& text_color(const Color& color);

private:
  bool m_cleanup_all {true};
  SDL_Window* m_window {nullptr};
  std::string m_window_title {"Retro Monochrome Text Monitor"s};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  int m_window_width {1280}, m_window_height {720};
  int m_window_pos_x {SDL_WINDOWPOS_UNDEFINED};
  int m_window_pos_y {SDL_WINDOWPOS_UNDEFINED};
  bool m_window_is_resizable {true};
  bool m_window_is_fullscreen {false};
  std::optional<Key_combo> m_key_fullscreen {
      std::in_place, Mod_keys::None, F_key::F11};
  std::optional<Key_combo> m_key_close_window {
      std::in_place, Mod_keys_strict::Ctrl, Key::K_w};
  std::optional<Key_combo> m_key_quit {
      std::in_place, Mod_keys_strict::Ctrl, Key::K_q};
  bool m_is_closing_same_as_quit {true};
  std::function<bool()> m_pre_close_function {[]() -> bool { return true; }};
  std::function<bool()> m_pre_quit_function {[]() -> bool { return true; }};
  Texture_config m_background {nullptr, "res/img/terminal_screen.png"s};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  SDL_Rect m_background_min_area {118, 95, 700, 540};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  SDL_FRect m_background_text_area {188.25f, 149.25f, 560.0f, 432.0f};
  Texture_config m_font_bitmap {nullptr, "res/img/font_bitmap.png"s};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  int m_font_width {7}, m_font_height {18};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  int m_text_area_columns {40}, m_text_area_lines {24};
  SDL_BlendMode m_text_blend_mode {SDL_BLENDMODE_ADD};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  Color m_text_color {89, 221, 0};
};
} // namespace remotemo
#endif // REMOTEMO_CONFIG_HPP
