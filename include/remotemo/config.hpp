/**
 * \file
 * \brief Header file for the configuration of \c remoTemo
 */

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

/**
 * \struct Window_config
 * \brief Used for the config of the window
 *
 * \sa Config::window()
 * \sa Config::window_size()
 * \sa Config::window_position()
 * \sa Config::window_resizable()
 * \sa Config::window_fullscreen()
 *
 * \var Window_config::raw_sdl
 * \brief Raw C-style pointer to an optional window
 *
 * Used by \c Config in case the user of the library wants to pass in a
 * window that had already been created using SDL. Leave it set to \c nullptr
 * if you want the library to take care of creating the texture.
 *
 * If \c Config::cleanup_all is set to \c true then the library takes
 * ownership of resources passed in (meaning that it takes care of cleaning
 * them up afterwards). Otherwise they are considered to be borrowed (leaving
 * the responsability of cleaning them up afterwards to the user of the
 * library).
 *
 * \warning If passing in a window, it must be a valid one.
 * \warning Deleting it while this library is still using it might have
 * unexpected consequences.
 * \warning If that window has a renderer, then that renderer \b must have the
 * \c SDL_RENDERER_TARGETTEXTURE flag set. Otherwise creating an \c Remotemo
 * object with that config will fail.
 *
 * \note If not set to \c nullptr, the other attributes of this struct will be
 * ignored.
 *
 * \var Window_config::title
 * \brief Title of the window if creating a new one.
 *
 * \var Window_config::width
 * \brief Width (in screen coordinates) of the window if creating a new one.
 *
 * \var Window_config::height
 * \brief Height (in screen coordinates) of the window if creating a new one.
 *
 * \var Window_config::pos_x
 * \brief The x position of the window if creating a new one.
 *
 * \var Window_config::pos_y
 * \brief The y position of the window if creating a new one.
 *
 * \var Window_config::is_resizable
 * \brief Whether to allow the window to be resizable, if creating a new one.
 *
 * \var Window_config::is_fullscreen
 * \brief Whether the window should be fullscreen, if creating a new one.
 *
 * \note If creating a fullscreen window, the size and position attributes are
 * ignored.
 */
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

/**
 * \struct Texture_config
 * \brief Base struct for configuration of textures
 *
 * \sa Backgr_config
 * \sa Font_config
 *
 * \var Texture_config::raw_sdl
 * \brief Raw C-style pointer to an optional texture
 *
 * Used by \c Config in case the user of the library wants to pass in an
 * already created texture. Leave it set to \c nullptr if you want the library
 * to take care of creating the texture.
 *
 * If \c Config::cleanup_all is set to \c true then the library takes
 * ownership of resources passed in (meaning that it takes care of cleaning
 * them up afterwards). Otherwise they are considered to be borrowed (leaving
 * the responsability of cleaning them up afterwards to the user of the
 * library).
 *
 * \warning Passing in a texture without also passing in the \c SDL_Renderer
 * it is tied to (by passing in the \c SDL_Window containing it) will result
 * in a failure to create an \c Remotemo object.
 * \warning The texture must be a valid one.
 * \warning Deleting it while this library is still using it might have
 * unexpected consequences.
 *
 * \var Texture_config::file_path
 * \brief Path of the file that will be loaded as the texture
 *
 * This setting is ignored if \c Texture_config::raw_sdl is not \c nullptr.
 *
 * Otherwise the library uses the file to create the texture, taking care of
 * cleaning up afterwards.
 *
 * \note
 * - On Linux/Unix/macOS/etc, if this setting is set to a string starting
 *   with the path separator (e.g. \c "/example/file") then the path is an
 *   absolute path.
 *
 * - On windows, if it's set to a string starting with:
 *   - a volume or drive letter, followed by the volume separator and the
 *     path separator (e.g. \c "D:\example\file") then the path is an absolute
 *     path.
 *   - a volume or drive letter, followed by the volume separator but not
 *     the path separator (e.g. \c "D:example\file") then the path is relative
 *     to the current directory of that drive (probably not what you
 *     intended).
 *   - the path separator (e.g. \c "\example\file") then the path is relative
 *     to the root of the current drive (are you sure that is what you
 *     intended?).
 *
 * - Otherwise the path is relative to your executable program file.
 *
 */
struct Texture_config {
  SDL_Texture* raw_sdl;
  std::string file_path;
};

/**
 * \struct Backgr_config
 * \brief Used for the config of the background texture
 *
 * \sa Texture_config
 * \sa Config::background()
 * \sa Config::background_file_path()
 * \sa Config::background_min_area()
 * \sa Config::background_text_area()
 *
 * \var Backgr_config::min_area
 * \brief The area of this texture that should always be visible.
 *
 * At the same time, the texture should be displayed as big as possible.
 * Meaning that:
 * - if the window's ratio is wider that the \c min_area, then that area
 *   should be centered horizontally while filling up the height of the
 *   window.
 * - if the window's ratio is narrower that the \c min_area, then that area
 *   should be centered vertically while filling up the width of the window.
 * - Only those part of the texture that don't fit into the window will get
 *   clipped away.
 *
 * \var Backgr_config::text_area
 * \brief The area of this texture, over which the text should be drawn.
 */
struct Backgr_config : Texture_config {
  Rect<int> min_area;
  Rect<float> text_area;
};

/**
 * \struct Font_config
 * \brief Used for the config of the font-bitmap texture
 *
 * \sa Texture_config
 * \sa Config::font_bitmap()
 * \sa Config::font_bitmap_file_path()
 * \sa Config::font_size()
 *
 * \var Font_config::width
 * \brief Width in pixels of each character as they are drawn in the
 * font-bitmap file.
 *
 * \var Font_config::height
 * \brief Height in pixels of each character as they are drawn in the
 * font-bitmap file.
 */
struct Font_config : Texture_config {
  int width;
  int height;
};

/**
 * \struct Text_area_config
 * \brief Used for the config of the text area
 *
 * \sa Config::text_area_size()
 * \sa Config::text_blend_mode()
 * \sa Config::text_color()
 *
 * \var Text_area_config::columns
 * \brief Width of the text area, in characters.
 *
 * \var Text_area_config::lines
 * \brief Height of the text area, in characters.
 *
 * \note
 * Does not include the extra "hidden" row that the cursor can move down to.
 *
 * \var Text_area_config::blend_mode
 * \brief Blend mode used to render the text onto the background.
 *
 * See the documentation of the \c SDL2 library for \c SDL_BlendMode on how
 * the different modes affect the rendering of one texture on top of another.
 *
 * \var Text_area_config::color
 * \brief Color the text will be rendered with
 *
 * See the documentation of the \c SDL2 library for \c SDL_SetTextureColorMod
 * on how the color affects the rendering of the text on top of the
 * background texture.
 */
struct Text_area_config {
  int columns;
  int lines;
  SDL_BlendMode blend_mode;
  Color color;
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
