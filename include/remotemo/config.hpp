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

////////////////////////////////////////////////////////////////////////

/** \struct Window_config
 * \brief Used for the config of the window
 *
 * \sa Config::window()
 * \sa Config::window_title()
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

////////////////////////////////////////////////////////////////////////

/** \struct Texture_config
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
 * \sa Config::background()
 * \sa Config::font_bitmap()
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
 * \sa Config::background_file_path()
 * \sa Config::font_bitmap_file_path()
 */
struct Texture_config {
  SDL_Texture* raw_sdl;
  std::string file_path;
};

////////////////////////////////////////////////////////////////////////

/** \struct Backgr_config
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
 * The texture will be displayed as big as possible while still fitting this
 * part of it within the window.
 *
 * Meaning that:
 * - if the window's ratio is wider that the \c min_area, then that area
 *   will be centered horizontally while filling up the height of the
 *   window.
 * - if the window's ratio is narrower that the \c min_area, then that area
 *   will be centered vertically while filling up the width of the window.
 * - Only those part of the texture that don't fit into the window will get
 *   clipped away.
 *
 * \sa Rect
 *
 * \var Backgr_config::text_area
 * \brief The area of this texture, over which the text should be drawn.
 *
 * \sa Rect
 */
struct Backgr_config : Texture_config {
  Rect<int> min_area;
  Rect<float> text_area;
};

////////////////////////////////////////////////////////////////////////

/** \struct Font_config
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

////////////////////////////////////////////////////////////////////////

/** \struct Text_area_config
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

////////////////////////////////////////////////////////////////////////

/** \brief Class containing the config used when creating a \c
 * remotemo::Remotemo object
 *
 * An object of this class will initially contain the default configuration,
 * sparing the user having to set more than just the parts that needs to
 * deviate from the defaults.
 *
 * All the setters of this class can be chained.
 *
 * Those few getters that are there are probably of no use to anyone except
 * those writing unit tests for the library itself.
 *
 * \note
 * This should be obvious to all those familiar with how returned constant
 * references works, but anyway:
 * \note
 * Regarding most of those getters, the returned struct is constant in the
 * sence that \b you can not change it but being a reference to a part of the
 * \c Config object, it can still be changed through the setters of said
 * object.
 * \note
 * Also, being a reference to a part of that object, it is only valid as long
 * as the object itself.
 * \note
 * Of course, if you copy that struct:
 * \code{.cpp}
 * auto backgr_conf = conf.background();
 * \endcode
 * instead of creating a reference to it:
 * \code{.cpp}
 * auto& backgr_conf = conf.background();
 * \endcode
 * then that copy is neither a constant nor a reference and therefore just
 * contains the status of that part of the config as it was at the point in
 * time when it was copied.
 */
class Config {
  friend Engine;

public:
  /// \brief The default constructor.
  constexpr Config() = default;

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the \c cleanup_all property.
   *
   * - (\b default) If set to \c true, then the destructor of the
   *   remotemo::Remotemo object will handle cleaning up all its resources and
   *   then call \c SDL_Quit() to clean up all SDL subsystems.
   * - If set to \c false, then it will only clean up those resources it did
   *   create and close those SDL subsystems that were not already
   *   initialized. You must then handle cleaning up any other resources and
   *   quit SDL afterwards yourself.
   *
   * \warning
   * If set to \c true, it will call \c SDL_Quit(), which should delete \b ALL
   * active SDL subsystems along with all its windows, renderer, etc.
   * \warning
   * Look at \c Remotemo::~Remotemo() for more.
   *
   * \param cleanup_all New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Remotemo::~Remotemo()
   */
  Config& cleanup_all(bool cleanup_all);

  //////////////////////////////////////////////////////////////////////

  /** \brief Get the setting of the \c cleanup_all property
   *
   * \return The \c cleanup_all property
   *
   * \sa Config::cleanup_all(bool)
   */
  [[nodiscard]] bool cleanup_all() const { return m_cleanup_all; }


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the raw C-style pointer to an optional window
   *
   * - (\b default) If \c nullptr then a new window will be created.
   * - If pointing to a valid \c SDL_Window, then that one will be used.
   *
   * \note
   * For more, see: \c Window_config::raw_sdl
   *
   * \param *window New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::raw_sdl
   */
  Config& window(SDL_Window* window);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the title of the window to be created
   *
   * \b Default: \c "Retro Monochrome Text Monitor"
   *
   * \param title New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::title
   */
  Config& window_title(const std::string& title);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the original size of the window to be created
   *
   * - **Default width:** \c 1280 pixels
   * - **Default height:** \c 720 pixels
   *
   * \param width New setting of the property
   * \param height New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::width
   * \sa Window_config::height
   */
  Config& window_size(int width, int height);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * \param size New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::width
   * \sa Window_config::height
   */
  Config& window_size(const Size& size);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the original position of the window to be created
   *
   * - **Default x position:** \c SDL_WINDOWPOS_UNDEFINED
   * - **Default y position:** \c SDL_WINDOWPOS_UNDEFINED
   *
   * \param x New setting of the property
   * \param y New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::pos_x
   * \sa Window_config::pos_y
   */
  Config& window_position(int x, int y);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * \param pos New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::pos_x
   * \sa Window_config::pos_y
   */
  Config& window_position(const Point& pos);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets whether to make the window resizable or not
   *
   * \b Default: \c true
   *
   * \param is_resizable New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::is_resizable
   */
  Config& window_resizable(bool is_resizable);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets whether to make the window fullscreen or not
   *
   * \b Default: \c false
   *
   * \note
   * If fullscreen, then the size and position attributes are ignored.
   *
   * \param is_fullscreen New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_config::is_fullscreen
   */
  Config& window_fullscreen(bool is_fullscreen);

  //////////////////////////////////////////////////////////////////////

  /** \brief Get the current config for the creation of the window
   *
   * \return Constant reference to the config for the window
   *
   * \sa Window_config
   */
  [[nodiscard]] const Window_config& window() const { return m_window; }


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the keyboard shortcut to switch to/from fullscreen
   *
   * With no parameters the keyboard shortcut is set to none.
   *
   * \b Default: \c F11
   *
   * \return The object itself (to allow chaining of setters).
   */
  Config& key_fullscreen();

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * Accepts an F-key and any combination of modifier keys (including no
   * modifier keys).
   *
   * \param modifier_keys New setting of the property
   * \param key New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Mod_keys
   * \sa F_key
   */
  Config& key_fullscreen(Mod_keys modifier_keys, F_key key);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * Accepts any "normal" key and a strict subset of the modifier key
   * combinations.
   *
   * \param modifier_keys New setting of the property
   * \param key New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Mod_keys_strict
   * \sa Key
   */
  Config& key_fullscreen(Mod_keys_strict modifier_keys, Key key);


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the keyboard shortcut to close the window.
   *
   * With no parameters the keyboard shortcut is set to none.
   *
   * \b Default: \c Ctrl-w
   *
   * By default, pressing the chosen key combination (or using the mouse
   * pointer to press the window's close button provided by the operating
   * system), will close the window, clean up its resources and throw an \c
   * remotemo::Window_is_closed_exception.
   *
   * How you want to handle that exception is up to you.
   *
   * Almost all functions will throw that same exception if you try calling
   * them after the window has been closed.
   *
   * \warning
   * Not catching that exception will terminate your program without making
   * sure all resources are cleaned up.
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Window_is_closed_exception
   * \sa Config::closing_same_as_quit()
   * \sa Config::function_pre_close()
   */
  Config& key_close_window();

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * Accepts an F-key and any combination of modifier keys (including no
   * modifier keys).
   *
   * \param modifier_keys New setting of the property
   * \param key New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Mod_keys
   * \sa F_key
   * \sa Window_is_closed_exception
   * \sa Config::closing_same_as_quit()
   * \sa Config::function_pre_close()
   */
  Config& key_close_window(Mod_keys modifier_keys, F_key key);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * Accepts any "normal" key and a strict subset of the modifier key
   * combinations.
   *
   * \param modifier_keys New setting of the property
   * \param key New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Mod_keys_strict
   * \sa Key
   * \sa Window_is_closed_exception
   * \sa Config::closing_same_as_quit()
   * \sa Config::function_pre_close()
   */
  Config& key_close_window(Mod_keys_strict modifier_keys, Key key);


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the keyboard shortcut to quit the application.
   *
   * With no parameters the keyboard shortcut is set to none.
   *
   * \b Default: \c Ctrl-q
   *
   * By default, pressing the chosen key combination will throw an
   * remotemo::User_quit_exception without closing the window nor cleaning up
   * anything.
   *
   * How you want to handle that exception is up to you, but it would be
   * pretty normal to respect their request and close down the application.
   *
   * \warning
   * Not catching that exception will terminate your program without making
   * sure all resources are cleaned up.
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa User_quit_exception
   * \sa Config::closing_same_as_quit()
   * \sa Config::function_pre_quit()
   */
  Config& key_quit();

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * Accepts an F-key and any combination of modifier keys (including no
   * modifier keys).
   *
   * \param modifier_keys New setting of the property
   * \param key New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Mod_keys
   * \sa F_key
   * \sa User_quit_exception
   * \sa Config::closing_same_as_quit()
   * \sa Config::function_pre_quit()
   */
  Config& key_quit(Mod_keys modifier_keys, F_key key);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * Accepts any "normal" key and a strict subset of the modifier key
   * combinations.
   *
   * \param modifier_keys New setting of the property
   * \param key New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Mod_keys_strict
   * \sa Key
   * \sa User_quit_exception
   * \sa Config::closing_same_as_quit()
   * \sa Config::function_pre_quit()
   */
  Config& key_quit(Mod_keys_strict modifier_keys, Key key);


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets whether closing the window and quit are the same
   *
   * - (\b Default) If set to \c false, closing the window or quitting do not
   *   behave in the same way. Each one does as described in \c
   *   Config::key_close_window() and in \c Config::key_quit().
   * - Setting this to \c true will change the behaviour of both the end-user
   *   closing the window and of pressing the quit-key-combo, so that doing
   *   any one of them will:
   *   - Close the window (and clean up its resources).
   *   - Then throw an \c remotemo::User_quit_exception.
   *
   * \note
   * No matter if set to \c true or \c false :
   *   - almost all functions will throw \c
   *     remotemo::Window_is_closed_exception if you try calling them after
   *     the window has been closed.
   *   - choosing to close the window will still call the
   *     function-to-call-before-closing-window while choosing to quit will
   *     call the function-to-call-before-quitting (just in case you might
   *     still want that difference in behavour between the two actions. If
   *     not, set them both to point to the same function).
   *
   *     See \c Config::function_pre_quit() and \c
   *     Config::function_pre_close() ).
   *
   * \param is_closing_same_as_quit New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Config::key_close_window()
   * \sa Config::key_quit()
   * \sa Config::function_pre_close()
   * \sa Config::function_pre_quit()
   */
  Config& closing_same_as_quit(bool is_closing_same_as_quit);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets function to call before closing window
   *
   * That function must return a \c bool. If it returns \c false, closing the
   * window will be canceled.
   *
   * That function can be used to e.g. offer the user the option to cancel
   * that action or to save before closing.
   *
   * \note
   * Trying to set this to \c nullptr will result in it being set to the
   * default value instead.
   *
   * \b Default:
   * \code{.cpp}
   * []() -> bool { return true; }
   * \endcode
   *
   * \param func New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Config::key_close_window()
   * \sa Config::closing_same_as_quit()
   */
  Config& function_pre_close(const std::function<bool()>& func);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets function to call before quitting
   *
   * That function must return a \c bool. If it returns \c false, quitting
   * will be canceled.
   *
   * That function can be used to e.g. offer the user the option to cancel
   * that action, to automatically save and/or do any needed cleanup before
   * quitting.
   *
   * \note
   * Trying to set this to \c nullptr will result in it being set to the
   * default value instead.
   *
   * \b Default:
   * \code{.cpp}
   * []() -> bool { return true; }
   * \endcode
   *
   * \param func New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Config::key_quit()
   * \sa Config::closing_same_as_quit()
   */
  Config& function_pre_quit(const std::function<bool()>& func);


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the raw C-style pointer to an optional background texture.
   *
   * - (\b default) If \c nullptr then the texture will be loaded from file.
   * - If pointing to a valid \c SDL_Texture, then that one will be used.
   *
   * \note
   * For more, see: \c Texture_config::raw_sdl
   *
   * \param *background New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Texture_config::raw_sdl
   */
  Config& background(SDL_Texture* background);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the path to use if loading the background texture.
   *
   * \b Default: \c "res/img/terminal_screen.png"
   *
   * \note
   * For more, see: \c Texture_config::file_path
   *
   * \param file_path New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Texture_config::file_path
   */
  Config& background_file_path(const std::string& file_path);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the area of the background that should always be visible.
   *
   * \b Default:
   * - \p x = 118
   * - \p y = 95
   * - \p w = 700
   * - \p h = 540
   *
   * \note
   * For more, see: \c Backgr_config::min_area
   *
   * \param x the x location of the area's upper left corner
   * \param y the y location of the area's upper left corner
   * \param w the width of the area
   * \param h the height of the area
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Backgr_config::min_area
   */
  Config& background_min_area(int x, int y, int w, int h);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * \param area New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Backgr_config::min_area
   * \sa Rect
   */
  Config& background_min_area(const Rect<int>& area);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the area of the background where the text is to be drawn
   *
   * \b Default:
   * - \p x = 188.25f
   * - \p y = 149.25f
   * - \p w = 560.0f
   * - \p h = 432.0f
   *
   * \note
   * The default values were chosen to fit the default background texture and
   * then moved 1/4 of a pixel to the right and down to get a nice blur
   * without needing to write any shaders (the main reason for skipping
   * shaders was because it's no longer an option to just use OpenGL
   * on all three main platforms since Apple stopped supporting it).
   *
   * \note
   * It is recommended that if you change this setting to fit your background,
   * then you should also move the location of the area by a fraction of a
   * pixel (unless you don't want that nice blur).
   *
   * \param x the x location of the area's upper left corner
   * \param y the y location of the area's upper left corner
   * \param w the width of the area
   * \param h the height of the area
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Backgr_config::text_area
   */
  Config& background_text_area(float x, float y, float w, float h);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * \param area New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Backgr_config::text_area
   * \sa Rect
   */
  Config& background_text_area(const Rect<float>& area);

  //////////////////////////////////////////////////////////////////////

  /** \brief Get the current config for the creation of the background texture
   *
   * \return Constant reference to the config for the background texture
   *
   * \sa Backgr_config
   * \sa Texture_config
   */
  [[nodiscard]] const Backgr_config& background() const
  {
    return m_background;
  }


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the raw C-style pointer to an optional font-bitmap texture.
   *
   * - (\b default) If \c nullptr then the texture will be loaded from file.
   * - If pointing to a valid \c SDL_Texture, then that one will be used.
   *
   * \param *font_bitmap New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Texture_config::raw_sdl
   */
  Config& font_bitmap(SDL_Texture* font_bitmap);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the path to use if loading the font-bitmap texture.
   *
   * \b Default: \c "res/img/font_bitmap.png"
   *
   * \param file_path New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Texture_config::file_path
   */
  Config& font_bitmap_file_path(const std::string& file_path);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the size of each character as they are in the font-bitmap
   *
   * \b Default:
   * - \p width = 7
   * - \p height = 18
   *
   * \param width New setting of the property
   * \param height New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Font_config::width
   * \sa Font_config::height
   */
  Config& font_size(int width, int height);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * \param size New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Font_config::width
   * \sa Font_config::height
   * \sa Size
   */
  Config& font_size(const Size& size);

  //////////////////////////////////////////////////////////////////////

  /** \brief Get the config for the creation of the font-bitmap texture
   *
   * \return Constant reference to the config for the font-bitmap texture
   *
   * \sa Font_config
   * \sa Texture_config
   */
  [[nodiscard]] const Font_config& font() const { return m_font; }


  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the size (in characters) of the text area
   *
   * The content of the text area will be drawn to a texture that will then be
   * rendered on top of the background. The size of that texture, in pixels,
   * is calculated from the font size and the text area size in characters.
   *
   * When rendered to the window, the texture will be scaled to fit the area
   * of the background where the text is to be drawn.
   *
   * \param columns New setting of the property
   * \param lines New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Text_area_config::columns
   * \sa Text_area_config::lines
   */
  Config& text_area_size(int columns, int lines);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * \param size New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Text_area_config::columns
   * \sa Text_area_config::lines
   * \sa Size
   */
  Config& text_area_size(const Size& size);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the blend mode used to render the text onto the background
   *
   * \param mode New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Text_area_config::blend_mode
   */
  Config& text_blend_mode(SDL_BlendMode mode);

  //////////////////////////////////////////////////////////////////////

  /** \brief Sets the color used when rendering the text onto the background
   *
   * \param red New setting of the property
   * \param green New setting of the property
   * \param blue New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Text_area_config::color
   * \sa Color
   */
  Config& text_color(Uint8 red, Uint8 green, Uint8 blue);

  //////////////////////////////////////////////////////////////////////

  /** \overload
   *
   * \param color New setting of the property
   *
   * \return The object itself (to allow chaining of setters).
   *
   * \sa Color
   */
  Config& text_color(const Color& color);

  //////////////////////////////////////////////////////////////////////

  /** \brief Get the current config for the setup of the text area
   *
   * \return Constant reference to the config for the text area
   *
   * \sa Text_area_config
   */
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
