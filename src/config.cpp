#include "remotemo/config.hpp"

namespace remotemo {
Config& Config::cleanup_all(bool cleanup_all)
{
  m_cleanup_all = cleanup_all;
  return *this;
}

Config& Config::window(SDL_Window* window)
{
  m_window = window;
  return *this;
}
Config& Config::window_title(const std::string& title)
{
  m_window_title = title;
  return *this;
}
Config& Config::window_size(int width, int height)
{
  m_window_width = width;
  m_window_height = height;
  return *this;
}
Config& Config::window_size(const SDL_Point& size)
{
  return window_size(size.x, size.y);
}
Config& Config::window_position(int x, int y)
{
  m_window_pos_x = x;
  m_window_pos_y = y;
  return *this;
}
Config& Config::window_position(const SDL_Point& pos)
{
  return window_position(pos.x, pos.y);
}
Config& Config::window_resizable(bool is_resizable)
{
  m_window_is_resizable = is_resizable;
  return *this;
}
Config& Config::window_fullscreen(bool is_fullscreen)
{
  m_window_is_fullscreen = is_fullscreen;
  return *this;
}

Config& Config::key_fullscreen()
{
  m_key_fullscreen = std::nullopt;
  return *this;
}
Config& Config::key_fullscreen(Mod_keys modifier_keys, F_key key)
{
  if (m_key_fullscreen) {
    m_key_fullscreen->set(modifier_keys, key);
  } else {
    m_key_fullscreen.emplace(modifier_keys, key);
  }
  return *this;
}
Config& Config::key_fullscreen(Mod_keys_strict modifier_keys, Key key)
{
  if (m_key_fullscreen) {
    m_key_fullscreen->set(modifier_keys, key);
  } else {
    m_key_fullscreen.emplace(modifier_keys, key);
  }
  return *this;
}

Config& Config::key_close_window()
{
  m_key_close_window = std::nullopt;
  return *this;
}
Config& Config::key_close_window(Mod_keys modifier_keys, F_key key)
{
  if (m_key_close_window) {
    m_key_close_window->set(modifier_keys, key);
  } else {
    m_key_close_window.emplace(modifier_keys, key);
  }
  return *this;
}
Config& Config::key_close_window(Mod_keys_strict modifier_keys, Key key)
{
  if (m_key_close_window) {
    m_key_close_window->set(modifier_keys, key);
  } else {
    m_key_close_window.emplace(modifier_keys, key);
  }
  return *this;
}

Config& Config::key_quit()
{
  m_key_quit = std::nullopt;
  return *this;
}
Config& Config::key_quit(Mod_keys modifier_keys, F_key key)
{
  if (m_key_quit) {
    m_key_quit->set(modifier_keys, key);
  } else {
    m_key_quit.emplace(modifier_keys, key);
  }
  return *this;
}
Config& Config::key_quit(Mod_keys_strict modifier_keys, Key key)
{
  if (m_key_quit) {
    m_key_quit->set(modifier_keys, key);
  } else {
    m_key_quit.emplace(modifier_keys, key);
  }
  return *this;
}

Config& Config::closing_same_as_quit(bool is_closing_same_as_quit)
{
  m_is_closing_same_as_quit = is_closing_same_as_quit;
  return *this;
}
Config& Config::function_pre_close(const std::function<bool()>& func)
{
  if (func == nullptr) {
    m_pre_close_function = []() -> bool { return true; };
  } else {
    m_pre_close_function = func;
  }
  return *this;
}
Config& Config::function_pre_quit(const std::function<bool()>& func)
{
  if (func == nullptr) {
    m_pre_quit_function = []() -> bool { return true; };
  } else {
    m_pre_quit_function = func;
  }
  return *this;
}

Config& Config::background(SDL_Texture* background)
{
  m_background.raw_sdl = background;
  return *this;
}
Config& Config::background_file_path(const std::string& file_path)
{
  m_background.file_path = file_path;
  return *this;
}
Config& Config::background_min_area(int x, int y, int w, int h)
{
  m_background_min_area.x = x;
  m_background_min_area.y = y;
  m_background_min_area.w = w;
  m_background_min_area.h = h;
  return *this;
}
Config& Config::background_text_area(float x, float y, float w, float h)
{
  m_background_text_area.x = x;
  m_background_text_area.y = y;
  m_background_text_area.w = w;
  m_background_text_area.h = h;
  return *this;
}

Config& Config::font_bitmap(SDL_Texture* font_bitmap)
{
  m_font_bitmap.raw_sdl = font_bitmap;
  return *this;
}
Config& Config::font_bitmap_file_path(const std::string& file_path)
{
  m_font_bitmap.file_path = file_path;
  return *this;
}
Config& Config::font_size(int width, int height)
{
  m_font_width = width;
  m_font_height = height;
  return *this;
}
Config& Config::font_size(const SDL_Point& size)
{
  return font_size(size.x, size.y);
}

Config& Config::text_area_size(int columns, int lines)
{
  m_text_area_columns = columns;
  m_text_area_lines = lines;
  return *this;
}
Config& Config::text_area_size(const SDL_Point& size)
{
  return text_area_size(size.x, size.y);
}
Config& Config::text_blend_mode(SDL_BlendMode mode)
{
  m_text_blend_mode = mode;
  return *this;
}
Config& Config::text_color(Uint8 red, Uint8 green, Uint8 blue)
{
  m_text_color.red = red;
  m_text_color.green = green;
  m_text_color.blue = blue;
  return *this;
}
Config& Config::text_color(const Color& color)
{
  return text_color(color.red, color.green, color.blue);
}
} // namespace remotemo
