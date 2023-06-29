#include "remotemo/config.hpp"

namespace remotemo {
Config& Config::cleanup_all(bool cleanup_all)
{
  m_cleanup_all = cleanup_all;
  return *this;
}

Config& Config::window(SDL_Window* window)
{
  m_window.raw_sdl = window;
  return *this;
}
Config& Config::window_title(const std::string& title)
{
  m_window.title = title;
  return *this;
}
Config& Config::window_size(int width, int height)
{
  m_window.width = width;
  m_window.height = height;
  return *this;
}
Config& Config::window_size(const Size& size)
{
  return window_size(size.width, size.height);
}
Config& Config::window_position(int x, int y)
{
  m_window.pos_x = x;
  m_window.pos_y = y;
  return *this;
}
Config& Config::window_position(const Point& pos)
{
  return window_position(pos.x, pos.y);
}
Config& Config::window_resizable(bool is_resizable)
{
  m_window.is_resizable = is_resizable;
  return *this;
}
Config& Config::window_fullscreen(bool is_fullscreen)
{
  m_window.is_fullscreen = is_fullscreen;
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
  return background_min_area(SDL_Rect {x, y, w, h});
}
Config& Config::background_min_area(const SDL_Rect& area)
{
  m_background.min_area = area;
  return *this;
}
Config& Config::background_text_area(float x, float y, float w, float h)
{
  return background_text_area(SDL_FRect {x, y, w, h});
}
Config& Config::background_text_area(const SDL_FRect& area)
{
  m_background.text_area = area;
  return *this;
}

Config& Config::font_bitmap(SDL_Texture* font_bitmap)
{
  m_font.raw_sdl = font_bitmap;
  return *this;
}
Config& Config::font_bitmap_file_path(const std::string& file_path)
{
  m_font.file_path = file_path;
  return *this;
}
Config& Config::font_size(int width, int height)
{
  m_font.width = width;
  m_font.height = height;
  return *this;
}
Config& Config::font_size(const Size& size)
{
  return font_size(size.width, size.height);
}

Config& Config::text_area_size(int columns, int lines)
{
  m_text_area.columns = columns;
  m_text_area.lines = lines;
  return *this;
}
Config& Config::text_area_size(const Size& size)
{
  return text_area_size(size.width, size.height);
}
Config& Config::text_blend_mode(SDL_BlendMode mode)
{
  m_text_area.blend_mode = mode;
  return *this;
}
Config& Config::text_color(Uint8 red, Uint8 green, Uint8 blue)
{
  return text_color(Color {red, green, blue});
}
Config& Config::text_color(const Color& color)
{
  m_text_area.color = color;
  return *this;
}

bool Config::validate_texture(SDL_Texture* texture, SDL_Renderer* renderer,
    const std::string& texture_name)
{
  if (texture == nullptr) {
    return true;
  }
  // Have not found a more direct way to check if the texture has got the
  // correct renderer:
  ::SDL_Rect noop_rect {-3, -3, 1, 1}; // Definetly not inside window
  if (::SDL_RenderCopy(renderer, texture, nullptr, &noop_rect) != 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "The %s texture that was handed to remotemo::create() "
        "was created with a different renderer than the window.\n",
        texture_name.c_str());
    return false;
  }
  return true;
}

bool Config::validate(SDL_Renderer* renderer) const
{
  if (m_window.raw_sdl != nullptr &&
      ::SDL_GetWindowID(m_window.raw_sdl) == 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "Window in config is invalid: %s\n", ::SDL_GetError());
    return false;
  }
  if (renderer == nullptr) {
    if (m_background.raw_sdl != nullptr || m_font.raw_sdl != nullptr) {
      return false;
    }
  } else {
    ::SDL_RendererInfo info;
    if (::SDL_GetRendererInfo(renderer, &info) != 0) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_GetRendererInfo() failed when trying to get info on the "
          "renderer of the window handed to remotemo::create(): %s\n",
          ::SDL_GetError());
      return false;
    }
    if ((info.flags & SDL_RENDERER_TARGETTEXTURE) !=
        SDL_RENDERER_TARGETTEXTURE) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "The renderer of the window handed to remotemo::create() did not "
          "have the correct flags (SDL_RENDERER_TARGETTEXTURE missing).\n");
      return false;
    }
    if (!validate_texture(m_font.raw_sdl, renderer, "font bitmap"s)) {
      return false;
    }
    if (!validate_texture(m_background.raw_sdl, renderer, "background"s)) {
      return false;
    }
  }
  return true;
}
} // namespace remotemo
