#ifndef REMOTEMO_CONFIG_HPP
#define REMOTEMO_CONFIG_HPP

#include <functional>
#include <string>

#include "remotemo/common_types.hpp"

#include <SDL.h>

namespace remoTemo {
using std::string_literals::operator""s;

class Temo;

class Config {
  friend Temo;

public:
  constexpr Config() = default;

  Config& cleanup_all(bool cleanup_all)
  {
    m_cleanup_all = cleanup_all;
    return *this;
  }

  Config& the_window(SDL_Window* window)
  {
    m_the_window = window;
    return *this;
  }
  Config& window_title(const std::string& title)
  {
    m_window_title = title;
    return *this;
  }
  Config& window_size(int width, int height)
  {
    m_window_width = width;
    m_window_height = height;
    return *this;
  }
  Config& window_size(const SDL_Point& size)
  {
    return window_size(size.x, size.y);
  }
  Config& window_position(int x, int y)
  {
    m_window_pos_x = x;
    m_window_pos_y = y;
    return *this;
  }
  Config& window_position(const SDL_Point& pos)
  {
    return window_position(pos.x, pos.y);
  }
  Config& window_resizable(bool is_resizable)
  {
    m_window_is_resizable = is_resizable;
    return *this;
  }
  Config& window_fullscreen(bool is_fullscreen)
  {
    m_window_is_fullscreen = is_fullscreen;
    return *this;
  }

  Config& key_fullscreen()
  {
    m_key_fullscreen = std::nullopt;
    return *this;
  }
  Config& key_fullscreen(Mod_keys modifier_keys, F_key key)
  {
    if (m_key_fullscreen) {
      m_key_fullscreen->set(modifier_keys, key);
    } else {
      m_key_fullscreen.emplace(modifier_keys, key);
    }
    return *this;
  }
  Config& key_fullscreen(Mod_keys_strict modifier_keys, Key key)
  {
    if (m_key_fullscreen) {
      m_key_fullscreen->set(modifier_keys, key);
    } else {
      m_key_fullscreen.emplace(modifier_keys, key);
    }
    return *this;
  }

  Config& key_close_window()
  {
    m_key_close_window = std::nullopt;
    return *this;
  }
  Config& key_close_window(Mod_keys modifier_keys, F_key key)
  {
    if (m_key_close_window) {
      m_key_close_window->set(modifier_keys, key);
    } else {
      m_key_close_window.emplace(modifier_keys, key);
    }
    return *this;
  }
  Config& key_close_window(Mod_keys_strict modifier_keys, Key key)
  {
    if (m_key_close_window) {
      m_key_close_window->set(modifier_keys, key);
    } else {
      m_key_close_window.emplace(modifier_keys, key);
    }
    return *this;
  }

  Config& key_quit()
  {
    m_key_quit = std::nullopt;
    return *this;
  }
  Config& key_quit(Mod_keys modifier_keys, F_key key)
  {
    if (m_key_quit) {
      m_key_quit->set(modifier_keys, key);
    } else {
      m_key_quit.emplace(modifier_keys, key);
    }
    return *this;
  }
  Config& key_quit(Mod_keys_strict modifier_keys, Key key)
  {
    if (m_key_quit) {
      m_key_quit->set(modifier_keys, key);
    } else {
      m_key_quit.emplace(modifier_keys, key);
    }
    return *this;
  }

  Config& closing_same_as_quit(bool is_closing_same_as_quit)
  {
    m_is_closing_same_as_quit = is_closing_same_as_quit;
    return *this;
  }
  Config& function_pre_close(const std::function<bool()>& func)
  {
    if (func == nullptr) {
      m_pre_close_function = []() -> bool { return true; };
    } else {
      m_pre_close_function = func;
    }
    return *this;
  }
  Config& function_pre_quit(const std::function<bool()>& func)
  {
    if (func == nullptr) {
      m_pre_quit_function = []() -> bool { return true; };
    } else {
      m_pre_quit_function = func;
    }
    return *this;
  }

  Config& background(SDL_Texture* background)
  {
    m_background = background;
    return *this;
  }
  Config& background_file_path(const std::string& file_path)
  {
    m_background_file_path = file_path;
    return *this;
  }
  Config& background_min_area(int x, int y, int w, int h)
  {
    m_background_min_area.x = x;
    m_background_min_area.y = y;
    m_background_min_area.w = w;
    m_background_min_area.h = h;
    return *this;
  }
  Config& background_text_area(float x, float y, float w, float h)
  {
    m_background_text_area.x = x;
    m_background_text_area.y = y;
    m_background_text_area.w = w;
    m_background_text_area.h = h;
    return *this;
  }

  Config& font_bitmap(SDL_Texture* font_bitmap)
  {
    m_font_bitmap = font_bitmap;
    return *this;
  }
  Config& font_bitmap_file_path(const std::string& file_path)
  {
    m_font_bitmap_file_path = file_path;
    return *this;
  }
  Config& font_size(int width, int height)
  {
    m_font_width = width;
    m_font_height = height;
    return *this;
  }
  Config& font_size(const SDL_Point& size)
  {
    return font_size(size.x, size.y);
  }

  Config& text_area_size(int columns, int lines)
  {
    m_text_area_columns = columns;
    m_text_area_lines = lines;
    return *this;
  }
  Config& text_area_size(const SDL_Point& size)
  {
    return text_area_size(size.x, size.y);
  }
  Config& text_blend_mode(SDL_BlendMode mode)
  {
    m_text_blend_mode = mode;
    return *this;
  }
  Config& text_color(Uint8 red, Uint8 green, Uint8 blue)
  {
    m_text_color.red = red;
    m_text_color.green = green;
    m_text_color.blue = blue;
    return *this;
  }
  Config& text_color(const Color& color)
  {
    return text_color(color.red, color.green, color.blue);
  }

private:
  bool m_cleanup_all {true};
  SDL_Window* m_the_window {nullptr};
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
  SDL_Texture* m_background {nullptr};
  std::string m_background_file_path {"res/img/terminal_screen.png"s};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  SDL_Rect m_background_min_area {118, 95, 700, 540};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  SDL_FRect m_background_text_area {188.25f, 149.25f, 560.0f, 432.0f};
  SDL_Texture* m_font_bitmap {nullptr};
  std::string m_font_bitmap_file_path {"res/img/font_bitmap.png"s};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  int m_font_width {7}, m_font_height {18};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  int m_text_area_columns {40}, m_text_area_lines {24};
  SDL_BlendMode m_text_blend_mode {SDL_BLENDMODE_ADD};
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  Color m_text_color {89, 221, 0};
};
} // namespace remoTemo
#endif // REMOTEMO_CONFIG_HPP
