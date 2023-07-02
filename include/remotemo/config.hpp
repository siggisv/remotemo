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

struct Texture_config {
  SDL_Texture* raw_sdl;
  std::string file_path;
};

struct Backgr_config : Texture_config {
  Rect<int> min_area;
  Rect<float> text_area;
};

struct Font_config : Texture_config {
  int width;
  int height;
};

struct Text_area_config {
  int columns;
  int lines;
  SDL_BlendMode blend_mode;
  Color color;
};

struct Window_config {
  SDL_Window* raw_sdl;
  std::string title;
  int width;
  int height;
  int pos_x;
  int pos_y;
  bool is_resizable;
  bool is_fullscreen;
};

class Config {
  friend Engine;

public:
  constexpr Config() = default;

  Config& cleanup_all(bool cleanup_all);
  [[nodiscard]] bool cleanup_all() const { return m_cleanup_all; }

  Config& window(SDL_Window* window);
  Config& window_title(const std::string& title);
  Config& window_size(int width, int height);
  Config& window_size(const Size& size);
  Config& window_position(int x, int y);
  Config& window_position(const Point& pos);
  Config& window_resizable(bool is_resizable);
  Config& window_fullscreen(bool is_fullscreen);
  [[nodiscard]] const Window_config& window() const { return m_window; }

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
  Config& background_min_area(const Rect<int>& area);
  Config& background_text_area(float x, float y, float w, float h);
  Config& background_text_area(const Rect<float>& area);
  [[nodiscard]] const Backgr_config& background() const
  {
    return m_background;
  }

  Config& font_bitmap(SDL_Texture* font_bitmap);
  Config& font_bitmap_file_path(const std::string& file_path);
  Config& font_size(int width, int height);
  Config& font_size(const Size& size);
  [[nodiscard]] const Font_config& font() const { return m_font; }

  Config& text_area_size(int columns, int lines);
  Config& text_area_size(const Size& size);
  Config& text_blend_mode(SDL_BlendMode mode);
  Config& text_color(Uint8 red, Uint8 green, Uint8 blue);
  Config& text_color(const Color& color);
  [[nodiscard]] const Text_area_config& text_area() const
  {
    return m_text_area;
  }

private:
  static bool validate_texture(SDL_Texture* texture, SDL_Renderer* renderer,
      const std::string& texture_name);
  bool validate(SDL_Renderer* renderer) const;

  bool m_cleanup_all {true};
  Window_config m_window {nullptr, "Retro Monochrome Text Monitor"s,
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
      1280, 720, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, true,
      false};
  std::optional<Key_combo> m_key_fullscreen {
      std::in_place, Mod_keys::None, F_key::F11};
  std::optional<Key_combo> m_key_close_window {
      std::in_place, Mod_keys_strict::Ctrl, Key::K_w};
  std::optional<Key_combo> m_key_quit {
      std::in_place, Mod_keys_strict::Ctrl, Key::K_q};
  bool m_is_closing_same_as_quit {false};
  std::function<bool()> m_pre_close_function {[]() -> bool { return true; }};
  std::function<bool()> m_pre_quit_function {[]() -> bool { return true; }};
  Backgr_config m_background {{nullptr, "res/img/terminal_screen.png"s},
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
      Rect<int> {118, 95, 700, 540},
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
      Rect<float> {188.25f, 149.25f, 560.0f, 432.0f}};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  Font_config m_font {{nullptr, "res/img/font_bitmap.png"s}, 7, 18};
  Text_area_config m_text_area {
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
      40, 24, SDL_BLENDMODE_ADD, Color {89, 221, 0}};
};
} // namespace remotemo
#endif // REMOTEMO_CONFIG_HPP
