# remoTemo API design
_8th draft_

> **Warning**
>
> **Both** the design and the implementation are work-in-progress.

## Table of contents
- [Version number](#version-number)
- [Initialization, cleanup and config](#initialization-cleanup-and-config)
  - [Initialization](#initialization)
  - [Cleanup](#cleanup)
  - [Configuration and default settings](#configuration-and-default-settings)
- [Using remoTemo](#using-remotemo)
  - [Structs and enums](#structs-and-enums)
  - [Text input/output](#text-inputoutput)
  - [Text output behaviour](#text-output-behaviour)
  - [Text area settings](#text-area-settings)
  - [Window settings](#window-settings)

<sup>[Back to top](#remotemo-api-design)</sup>
## Version number

The version number of this library is assigned and incremented according to
the rules set by [Semantic Versioning
2.0.0](https://semver.org/spec/v2.0.0.html), using the optional pre-release
label for alpha and beta versions.

In short:

- The format of the version number is `Major.Minor.Patch` for release versions
  (e.g. 1.0.3) and `Major.Minor.Patch-PreReleaseLabel` for pre-release
  versions (e.g. 1.0.3-alpha.3 or 1.0.3-beta.1).
- The format of `PreReleaseLabel` is "alpha", "beta" or "rc", followed by a
  dot and an incrementing number (that is reset to zero when going from alpha
  to beta and from beta to release-candidate).
- `Major` is increased when incompatible changes are made to the public API.
- `Minor` is increased when backward compatible changes are made to the public
  API (i.e. new functionality is added).
- `Patch` is increased when backward compatible bug fixes are made.
- `Major` is zero while in initial development. At that time anything might
  change when `Minor` is increased.
- The `Major.Minor.Patch`-part of pre-release versions denotes the release
  version that will be released, not the one that is built on.

The library provides a few functions that returns its name and its version:

```C++
std::string remotemo::full_name();
std::string remotemo::version_full();
int remotemo::version_major();
int remotemo::version_minor();
int remotemo::version_patch();
std::string remotemo::version_pre_release_label();
bool remotemo::version_is_pre_release();
```

Assuming the version to be `1.0.3`, they would return the following:

- `remotemo::full_name()` will return the string `"remoTemo v1.0.3"`.
- `remotemo::version_full()` will return the string `"1.0.3"`.
- `remotemo::version_major()` will return the int `1`.
- `remotemo::version_minor()` will return the int `0`.
- `remotemo::version_patch()` will return the int `3`.
- `remotemo::version_pre_release_label()` will return the string `""`.
- `remotemo::version_is_pre_release()` will return the bool `false`.

Assuming the version to be `1.1.0-beta.2`, they would return the following:

- `remotemo::full_name()` will return the string `"remoTemo v1.1.0-beta.2"`.
- `remotemo::version_full()` will return the string `"1.1.0-beta.2"`.
- `remotemo::version_major()` will return the int `1`.
- `remotemo::version_minor()` will return the int `1`.
- `remotemo::version_patch()` will return the int `0`.
- `remotemo::version_pre_release_label()` will return the string `"beta.2"`.
- `remotemo::version_is_pre_release()` will return the bool `true`.

<sup>[Back to top](#remotemo-api-design)</sup>
## Initialization, cleanup and config
### Initialization

```C++
std::optional<remotemo::Remotemo> remotemo::create();
std::optional<remotemo::Remotemo> remotemo::create(
        const remotemo::Config& config);
```

This non-member function will create and initialize an object representing
an ASCII-only text monitor with an US-ANSI keyboard and their properties. That
object will also contain the window showing it (along with the needed textures
and properties for the underlying SDL2-library).

- If successful, that function will return a
  `std::optional<remotemo::Remotemo>` containing the object. It will also
  create an internal list of resources that its destructor should take care of
  cleaning up.
  > **Note** `remotemo::Remotemo` is movable but not copyable. It is therefore
  > recommended to pass it along by reference or to use a smart pointer.

- On failure, it will log an error using `SDL_Log()`, free/release all
  resources it had succeeded setting up and then return an empty
  `std::optional` object.

  > **Warning**
  > On failure, it will clean up all the resources that the
  > `remotemo::Remotemo`'s destructor is supposed to handle. Meaning that:
  > - by **default** it would **not only** clean up those resources it had
  >   succeeded setting up, but **also all** those resources handed over to
  >   it. **And** it will call `SDL_Quit()`.
  > - if `Config::cleanup_all` was set to `false`, it will **only** clean up
  >   those resources it had succeeded setting up itself. It will quit those
  >   SDL subsystems it did set up but will **not** call `SDL_Quit()`.

When invoked without any parameters, it will set up the underlying
SDL2-library and create a window for you with the default settings (including
loading a background texture and the font-bitmap from their default
locations). Most of those settings can not be changed after creation.

  > **Warning**
  > If SDL2 has already been initialized before calling this function, then it
  > must be done from the same thread.

-----

To allow you to initialize using other settings than the default ones, the
[Named Parameter
Idiom](https://isocpp.org/wiki/faq/ctors#named-parameter-idiom) is followed,
providing you with a "config" object that when created, will contains the
default initialization settings:

```C++
remotemo::Config::Config();
```

_The constructor that creates a `remotemo::Config` object._

You can then pick which settings to change by calling its setters. Then call
`remotemo::create(const remotemo::Config& config)` to create and initialize
everything with the settings you wanted.

- You can store that `remotemo::Config` object in a variable, work on it and
  then pass it to `remotemo::create()`:

  ```C++
  remotemo::Config my_config;
  if (some_condition) {
      my_config.window_size(1920, 1080);
  }
  my_config.background_file("background.png")
          .background_min_area(200, 120, 1000, 700);
  auto text_monitor = remotemo::create(my_config);
  ```
  
- Or you could simply call `remotemo::create()` directly with a temporary
  `remotemo::Config` object:
  
  ```C++
  auto text_monitor = remotemo::create(
          remotemo::Config()
                  .window_size(1920, 1080)
                  .background_file("background.png")
                  .background_min_area(200, 120, 1000, 700));
  ```

<sup>[Back to top](#remotemo-api-design)</sup>
### Cleanup

```C++
remotemo::Remotemo::~Remotemo();
```

_The destructor, (by default) in charge of deleting/closing textures, the
renderer, the window and quitting SDL._

- By default, the destructor of the `remotemo::Remotemo` object will handle
  cleaning up all its resources (both those it did create itself and those
  passed to it) and then call `SDL_Quit()` to clean up all SDL subsystems.

- When created, the `remotemo::Remotemo` object can be set to not have its
  destructor handle cleaning up **all** its resources.

  In that case it will **only** clean up those resources it did create itself,
  leaving to you the responsibility of cleaning up all other resources and to
  call `SDL_Quit()` when you're done.

  This can be usefull e.g. if you already created a window yourself to display
  some splash screen and/or menu before the text monitor is shown and would
  like that window to remain open afterward to be able to display the menu
  again and/or some afterwords.

If, for some reason, you want to clean up sooner, you could call:

```C++
void remotemo::Remotemo::quit();
```
This will just close the window and clean up its resources.

Almost all functions will throw an `remotemo::Window_is_closed_exception` if
you try calling them after the window has been closed.

<sup>[Back to top](#remotemo-api-design)</sup>
### Configuration and default settings

The following are the default settings when creating an `remotemo::Remotemo`
object, with a short description of the different options for each setting and
which of `remotemo::Config`'s setters can be used to change that setting.

Unless noted, the settings can not be changed after the `remotemo::Remotemo`
object has been created:

- Cleanup all: `true`
  ```C++
  remotemo::Config& remotemo::Config::cleanup_all(bool cleanup_all);
  ```
  - If `true`, then the destructor of the `remotemo::Remotemo` object will
    handle cleaning up all its resources and then call `SDL_Quit()` to clean
    up all SDL subsystems.

    > **Warning**
    > It will clean up (i.e close, remove and/or delete) any window, renderer
    > and texture it used, no matter if it did created them itself or they
    > were passed to it.

  - If `false`, then it will only clean up those resources it did create (but
    none of those you passed to it). You must then handle cleaning up those
    and quit SDL afterwards yourself.

- Window: `nullptr`
  ```C++
  remotemo::Config& remotemo::Config::window(SDL_Window* window);
  ```
  - If set to `nullptr` then `remotemo::create()` will create the window with
    the following settings and set it as being owned by this object (meaning
    the destructor will be in charge of destroying it).
    - title: `"Retro Monochrome Text Monitor"` _(can be changed later)_
    - width: `1280` _(can be changed later)_
    - height: `720` _(can be changed later)_
    - position, x: `SDL_WINDOWPOS_UNDEFINED` _(can be changed later)_
    - position, y: `SDL_WINDOWPOS_UNDEFINED` _(can be changed later)_
    - resizable: `true` _(can be changed later)_
    - fullscreen: `false` _(can be changed later)_

    ```C++
    remotemo::Config& remotemo::Config::window_title(const std::string& title);
    remotemo::Config& remotemo::Config::window_size(int width, int height);
    remotemo::Config& remotemo::Config::window_size(const SDL_Point& size);
    remotemo::Config& remotemo::Config::window_position(int x, int y);
    remotemo::Config& remotemo::Config::window_position(const SDL_Point& pos);
    remotemo::Config& remotemo::Config::window_resizable(bool is_resizable);
    remotemo::Config& remotemo::Config::window_fullscreen(bool is_fullscreen);
    ```

  - If NOT set to `nullptr` then 'the window' must be a pointer to a valid
    `SDL_Window`.

    > **Warning** It will be your responsability to keep that pointed window
    > valid while `remotemo::Remotemo` is alive.
    >
    > You also have to take care of closing it afterward (unless 'Cleanup all'
    > is set to `true`, in which case `remotemo::Remotemo`'s destructor will
    > handle that).

    [](ignored)

    > **Warning** If that window has a renderer associated with it, then that
    > renderer must have the `SDL_RENDERER_TARGETTEXTURE` flag set. Otherwise
    > `remotemo::create()` will fail.

  No mater if 'the window' is set to `nullptr` or not, the following
  properties tied to the window will be set (with the following being the
  default):
  - Key to switch between fullscreen and windowed mode: `F11` _(can be changed
    later)_
    ```C++
    remotemo::Config& remotemo::Config::key_fullscreen(); // Set to none
    remotemo::Config& remotemo::Config::key_fullscreen(
            remotemo::Mod_keys modifier_keys, remotemo::F_key key);
    remotemo::Config& remotemo::Config::key_fullscreen(
            remotemo::Mod_keys_strict modifier_keys, remotemo::Key key);
    ```
  - Key combination to close the window: `Ctrl-w` _(can be changed later)_
    ```C++
    remotemo::Config& remotemo::Config::key_close_window(); // Set to none
    remotemo::Config& remotemo::Config::key_close_window(
            remotemo::Mod_keys modifier_keys, remotemo::F_key key);
    remotemo::Config& remotemo::Config::key_close_window(
            remotemo::Mod_keys_strict modifier_keys, remotemo::Key key);
    ```
    By default, pressing the chosen key combination (or using the mouse
    pointer to press the window's close button provided by the operating
    system), will close the window, clean up its resources and throw an
    `remotemo::Window_is_closed_exception`.

    How you want to handle that exception is up to you.

    Almost all functions will throw that same exception if you try calling them
    after the window has been closed.

  - Key combination to quit the application: `Ctrl-q` _(can be changed later)_
    ```C++
    remotemo::Config& remotemo::Config::key_quit(); // Set to none
    remotemo::Config& remotemo::Config::key_quit(
            remotemo::Mod_keys modifier_keys, remotemo::F_key key);
    remotemo::Config& remotemo::Config::key_quit(
            remotemo::Mod_keys_strict modifier_keys, remotemo::Key key);
    ```
    By default, pressing the chosen key combination will throw an
    `remotemo::User_quit_exception` without closing the window nor cleaning up
    anything.

    How you want to handle that exception is up to you, but it would be pretty
    normal to respect their request and close down the application.

    > **Warning** Not catching that exception will terminate your program
    > **without** making sure all resources are cleaned up.

  - Closing window is the same as quitting: `false` _(can be changed later)_
    ```C++
    remotemo::Config& remotemo::Config::closing_same_as_quit(
            bool is_closing_same_as_quit);
    ```
    Setting this to `true` will change the behaviour of both choosing to close
    the window and of choosing to quit so that doing any one of them will:
    - Close the window (and clean up its resources).
    - Then throw an `remotemo::User_quit_exception`.

    Note that as before (when this is set to `false`):
    - almost all functions will throw `remotemo::Window_is_closed_exception`
      if you try calling them after the window has been closed.
    - choosing to close the window will still call the
      function-to-call-before-closing-window while choosing to quit will call
      the function-to-call-before-quitting.
  - Function to call before closing window: `[]() -> bool { return true; }`
    _(can be changed later)_
    ```C++
    remotemo::Config& remotemo::Config::function_pre_close(
            const std::function<bool()>& func);
    ```
    That function must return a `bool`. If it returns `false`, closing the
    window will be canceled.

    That function can be used to e.g. offer the user the option to cancel that
    action or to save before closing.
    > **Note** Trying to set this to `nullptr` will result in it being set to
    > the default value instead.
  - Function to call before quitting: `[]() -> bool { return true; }`
    _(can be changed later)_
    ```C++
    remotemo::Config& remotemo::Config::function_pre_quit(
            const std::function<bool()>& func);
    ```
    That function must return a `bool`. If it returns `false`, quitting will
    be canceled.

    That function can be used to e.g. automatically save and/or do any needed
    cleanup before quitting.
    > **Note** Trying to set this to `nullptr` will result in it being set to
    > the default value instead.

  Finally, if 'the window' was set to `nullptr` **OR** if it is pointing to a
  `SDL_Window` that doesn't have a renderer then a `SDL_Renderer` will be
  created (either tied to that newly created window or to the one pointed to)
  and that renderer set as being owned by this object (meaning the destructor
  will be in charge of destroying it).

- Background: `nullptr`
  ```C++
  remotemo::Config& remotemo::Config::background(SDL_Texture* background);
  ```

  - If set to `nullptr` then the background texture will be loaded from the
    following file and set as being owned by this object (meaning the
    destructor will be in charge of destroying it):
    - Background file path: `"res/img/terminal_screen.png"` (which will be
      converted to `"res\\img\\terminal_screen.png"` on operating systems that
      have `\` as a path separator).
      ```C++
      remotemo::Config& remotemo::Config::background_file_path(
              const std::string& file_path);
      ```

      When changing this setting, you should use `'/'` as a path separator
      since it's more portable (it will be converted to the preferred one of
      the operating system for you).

      > **Note**
      > - On Linux/Unix/macOS/etc, if this setting is set to a string starting
      >   with the path separator (e.g. "/example/file") then the path is an
      >   absolute path.
      >
      >   Otherwise the path is relative to your executable program file.
      > - On windows, if it's set to a string starting with:
      >   - a volume or drive letter, followed by the volume separator and the
      >     path separator (e.g. "D:\\example\\file") then the path is an
      >     absolute path.
      >   - a volume or drive letter, followed by the volume separator but not
      >     the path separator (e.g. "D:example\\file") then the path is
      >     relative to the current directory of that drive (probably not what
      >     you intended).
      >   - the path separator (e.g. "\\example\file") then the path is
      >     relative to the root of the current drive (are you sure that is
      >     what you intended?).
      >
      >   Otherwise the path is relative to your executable program file.

  - If NOT set to `nullptr` then 'background' must point to an `SDL_Texture`
    containing the desired background image.

    > **Warning** The texture **MUST** have been created with the renderer
    > tied to the window (which also means that you can not hand over a
    > texture without also handing over the correct window).

    [](ignored)

    > **Warning** It will be your responsability to keep that pointed texture
    > valid while `remotemo::Remotemo` is alive.
    >
    > You also have to take care of closing it afterward (unless 'Cleanup all'
    > is set to `true`, in which case `remotemo::Remotemo`'s destructor will
    > handle that).

  No mater if 'background' is set to `nullptr` or not, the following settings
  (with the following defaults) will be used:
  - minimum area to be shown:
    `x: 118, y: 95, width: 700, height: 540`
    \
    This setting allows for the background texture to include borders around
    the main area. That border can then either be shown to the sides of that
    area (if the ratio of the window is wider) or above and below that area
    (if the ratio of the window is narrower).
  - the area of the background where the text is to be drawn:
    `x: 188.25f, y: 149.25f, w: 560.0f, h: 432.0f`

  ```C++
  remotemo::Config& remotemo::Config::background_min_area(int x, int y,
          int w, int h);
  remotemo::Config& remotemo::Config::background_min_area(SDL_Rect area);
  remotemo::Config& remotemo::Config::background_text_area(float x, float y,
          float width, float height);
  remotemo::Config& remotemo::Config::background_text_area(SDL_FRect area);
  ```
  > **Note** x and y are counted from the top-left corner, both starting at
  > zero.

- Font-bitmap: `nullptr`
  ```C++
  remotemo::Config& remotemo::Config::font_bitmap(SDL_Texture* font_bitmap);
  ```
  - If set to `nullptr` then the font-bitmap texture will be loaded from the
    following file and set as being owned by this object (meaning the
    destructor will be in charge of destroying it):
    - Font-bitmap file path: `"res/img/font_bitmap.png"` (which will be
      converted to `"res\\img\\font_bitmap.png"` on operating systems that
      have `\` as a path separator).
      ```C++
      remotemo::Config& remotemo::Config::font_bitmap_file_path(
              const std::string& file_path);
      ```

      When changing this setting, you should use `'/'` as a path separator
      since it's more portable (it will be converted to the preferred one of
      the operating system for you).

      > **Note**
      > See note above in background setting regarding paths being relative
      > or absolute.

  - If NOT set to `nullptr` then 'font-bitmap' must point to an `SDL_Texture`
    containing the desired font-bitmap image.

    > **Warning** The texture **MUST** have been created with the renderer
    > tied to the window (which also means that you can not hand over a
    > texture without also handing over the correct window).

    [](ignored)

    > **Warning** It will be your responsability to keep that pointed texture
    > valid while `remotemo::Remotemo` is alive.
    >
    > You also have to take care of closing it afterward (unless 'Cleanup all'
    > is set to `true`, in which case `remotemo::Remotemo`'s destructor will
    > handle that).

  No mater if 'font-bitmap' is set to `nullptr` or not, the following setting
  (with the following default) will be used:
  - Font size: `width: 7, height: 18`
    \
    This is the size, in pixels, of each character as they are drawn in the
    font-bitmap file.
    ```C++
    remotemo::Config& remotemo::Config::font_size(int width, int height);
    remotemo::Config& remotemo::Config::font_size(const SDL_Point& size);
    ```

- The following are the default setting used to create a texture buffer where
  the text will be drawn before being rendered on top of the background:
  - text area size (in characters):
    `columns: 40, lines: 24` _(can be changed later)_
    \
    The size of the texture, in pixels, is calculated from the font size and
    the text area size in characters (The texture will then be scaled to fit
    the area of the background where the text is to be drawn).

  - blend mode: `SDL_BLENDMODE_ADD` _(can be changed later)_
  - color mod: `red: 89, green: 221, blue: 0` _(can be changed later)_
    \
    > **Note** This setting controls the color of **all** the text, both the
    > text that has already been printed to the screen and the text that is
    > going to be printed to the screen.
  ```C++
  remotemo::Config& remotemo::Config::text_area_size(int columns, int lines);
  remotemo::Config& remotemo::Config::text_area_size(const SDL_Point& size);
  remotemo::Config& remotemo::Config::text_blend_mode(SDL_BlendMode mode);
  remotemo::Config& remotemo::Config::text_color(Uint8 red, Uint8 green,
          Uint8 blue);
  remotemo::Config& remotemo::Config::text_color(const remotemo::Color& color);
  ```

The following properties can not be changed before creating the
`remotemo::Remotemo` object. They control the behaviour of the text output and
will have the initial values shown here _(those can all be changed later)_:
- delay between chars: `60` (milliseconds)
  \
  Whenever text is printed to the monitor, it gets displayed one character
  at the time with a slight delay between.
- inverse: `false`
  \
  Set this to `true` to print text with the background color and the
  foreground color switched.
  \
  Changing this doesn't affect text that has already been printed to the
  screen.
- cursor position, counted from the top-left corner:
  `column: 0, line: 0`
- scrolling of text: `true`
  \
  If set to `true`, the content of the screen scrolls up one line when the
  cursor goes below the bottom line.
- text wrapping: `character`
  \
  This can be set to `off` (text printed beyond the right border gets lost),
  `character` (text wraps to next line. This might happen in the middle of a
  word) or `word` (text wraps to the next line, if possible at the last
  whitespace before getting to the right border).

<sup>[Back to top](#remotemo-api-design)</sup>
## Using remoTemo

In single thread mode (which currently is not only the default mode, it is the
only one being implemented until maybe later) there is no main loop constantly
updating the window.

Instead, after the `Remotemo`-object has been created, control is given back
to your program. Then each time one of the I/O-functions is called
(`get_cursor_position()` not included), that function calls (at least) once a
function that handles events and updating the window, before returning to your
program. How often they call that updating-function depend on how long they
need to run. E.g. `set_cursor(position)` just does it often enough for the
window to show the new position of the cursor, while `pause()` does it
repeatly until the requested time has passed and `get_input()` does it until
a key has been pressed.

This puts (at least) two restrictions on your code:
- Those functions should only be called from the thread where SDL was
  initialized. Unless you initialized SDL yourself, that would be the thread
  where `remotemo::create()` was called (if you **did** initialize SDL
  yourself, then as stated in the [initialization chapter](#initialization),
  that should have happened in the same thread).
- Your code should call some of those functions often enough to keep the
  window responsive.

The following functions all throw an exception if the window is closed
before or while being called.

### Structs and enums

```C++
enum class remotemo::Wrapping {
  off, character, word
};

struct remotemo::Color {
  Uint8 red, green, blue;
};

enum class Mod_keys_strict {
  Ctrl, Alt, Alt_shift, Ctrl_shift, Ctrl_alt, Ctrl_alt_shift
};

enum class Mod_keys {
  None, Shift, Ctrl, Alt, Alt_shift, Ctrl_shift, Ctrl_alt, Ctrl_alt_shift
};

enum class remotemo::F_key {
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

enum class remotemo::Key {
    K_esc, K_backquote, K_1, K_2, ... , K_0, K_minus, K_equal, K_backspace,
    K_tab, K_q, K_w, ... K_p, K_left_bracket, K_right_bracket, K_backslash,
    ...
    ...
    K_space, K_up, K_down, K_left, K_right};
    /* Basically all the keys on a regular keyboard, except F-keys, the
       keypad and modifier keys */

enum class remotemo::Do_reset {none, cursor, inverse, all};
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Text input/output

As mentioned before, the I/O functions (except `get_cursor_position()`) also
update the window while running.

> **Note**
> One thing to note is that when it comes to the cursor position, the text
> area is considered to have one extra, empty, hidden line below the bottom
> one shown on screen. The cursor can be moved there (either directly or
> through text wrapping) but as soon as you try to display anything there then
> (depending on the scrolling settings) either all lines scroll up or the
> output gets lost.

```C++
int remotemo::Remotemo::move_cursor(int x, int y);
int remotemo::Remotemo::move_cursor(const SDL_Point& move);
```

Moves the cursor `x` columns to the right (negative `x` moves it to the left)
and `y` lines down (negative `y` moves it up).

- If successful, returns `0`.
- If trying to move the cursor past the edges of the text area, it will stop
  just inside the area and return:
  - `-1` if trying to go past right edge.
  - `-2` if trying to go past bottom edge.
  - `-4` if trying to go past left edge.
  - `-8` if trying to go past top edge.
  - `-3` (-1 + -2) if trying to go past right and bottom edges.
  - `-9` (-1 + -8) if trying to go past right and top edges.
  - `-6` (-4 + -2) if trying to go past left and bottom edges.
  - `-12` (-4 + -8) if trying to go past left and top edges.

```C++
int remotemo::Remotemo::set_cursor(int column, int line);
int remotemo::Remotemo::set_cursor(const SDL_Point& position);
int remotemo::Remotemo::set_cursor_column(int column);
int remotemo::Remotemo::set_cursor_line(int line);
```

Moves the cursor to the given position.

- If successful, returns `0`.
- If trying to move the cursor past the edges of the text area, it stay where
  it was and the function will return `-1`.

```C++
SDL_Point remotemo::Remotemo::get_cursor_position();
```

(Obviously) returns the current position of the cursor. As stated elsewhere,
does not update the window.

```C++
int remotemo::Remotemo::pause(int pause_in_ms);
```

Wait for the given time (in milliseconds), checking for events and updating
the window as needed, before returning:

- `0` on success.
- `-1` on error (e.g. the given time is negative).

```C++
void remotemo::Remotemo::clear(
  remotemo::Do_reset do_reset = remotemo::Do_reset::all);
```

Clears all the text area at once.

- If `do_reset` is `cursor` or `all`, it also resets the position of the
  cursor to `(0, 0)`, otherwise leaving it at its current position.
- If `do_reset` is `inverse` or `all`, it also resets 'inverse' to `false`.

No matter how 'inverse' is set, clearing the text area fills it all with the
background color.

```C++
remotemo::Key remotemo::Remotemo::get_key();
```

Waits for a key being pressed and returns it (without displaying it on the
screen). As noted regarding the enum class `remotemo::Key`, F-keys (e.g.
`F1`), the keypad and modifier keys are not included.

```C++
std::string remotemo::Remotemo::get_input(int max_length);
```

Allows the user to enter some text. It gets displayed onto the screen as the
user enters it, starting where the cursor was. The text can be edited, using
`Backspace` or the left- and right-arrow keys, until `Return` is pressed, at
which time a string containing the text is returned.

`max_length` not only restrict the length of the string being returned. It
also restrict the lenght of the text being entered on the screen.

- If wrapping is set to `off` then the length is also restricted by the
  distance to the right border of the screen.
- If wrapping is set to `character` (or `word`, which give same behaviour as
  `character` when getting input), then:
  - having scrolling set to `false` will restrict the length by what can fit
    from the current cursor position, down to the bottom-right corner of the
    screen.
  - having scrolling set to `true` will only restrict the length by what can
    fit on the screen (i.e. the text being entered can not scroll up further
    than the top border).

> **Note** No matter what keyboard layout the user has, this function will
> behave as if it was an `US-ANSI` layout.

```C++
int remotemo::Remotemo::print(const std::string& text);
```

Display the given string, starting at the current position of the cursor, one
character at the time with a slight delay inbetween (as set by the 'delay
between chars' property). With the exception of special characters like the
`backspace` character and the `newline` character, each character displayed
will move the cursor one space to the right (if already at the right border of
the text area, it will stay put or wrap to next line depending on the wrapping
settings. If wrapping is set to `word` it might wrap even sooner).

- Returns `0` on success.
- Returns `-2` if some of the text could not get displayed (e.g. reached end
  of line while wrapping was set to `off`).
- The `newline` character just moves the cursor to the beginning of the next
  line.
- If the inverse property is set to `true`, the string will be displayed with
  the foreground and background colors switched.
- The `backspace` character will:
  - make the function stop and return -2 if the cursor was already at the left
    border. This happens even when wrapping is on.
  - move the cursor one space to the left and overwrite the content there with
    a single `space` character (using the current inverse property).

> **Warning** Any non-ASCII character will be displayed as `�`. This will not
> count as "text could not get displayed".

#### Wrapping
- If set to `off` then text printed beyond the right border gets lost and the
  cursor stops just inside the border.
- If set to `character` then text wraps to the beginning of the next line when
  reaching the right border, possibly splitting a word in the process.
- If set to `word` then text wraps to the beginning of the next line, at the
  last whitespace before getting to the right border. Except if there is no
  whitespace in the current line, in which case this line wraps at the right
  border.

#### Scrolling
If the cursor is moved down (because of wrapping or the newline character)
while it's already at the bottom visible line, it will move down to the extra,
hidden line below the visible area.

Then when trying to print any text to the hidden line, depending on the
'scrolling' settings:

- If set to `true` then the whole content of the screen moves up one line
  (moving the cursor up to the bottom visible line at the same time) before
  continuing printing the rest of the text.
- If set to `false` then the rest of the string is lost.

```C++
int remotemo::Remotemo::print_at(int column, int line,
        const std::string& text);
int remotemo::Remotemo::print_at(const SDL_Point& position,
        const std::string& text);
```

Does the same thing as calling first `set_cursor()` and then `print()`.

- Returns `0` on success.
- Returns `-1` without moving the cursor nor printing anything if the given
  position is outside the text area.
- Returns `-2` if some of the text could not get displayed (e.g. reached end
  of line while wrapping was set to `off`).

```C++
char remotemo::Remotemo::get_char_at(int column, int line);
char remotemo::Remotemo::get_char_at(const SDL_Point& pos);
```

- Returns the character that is at the given position (no matter if it was
  printed directly at that position or if it was printed at some line below
  and then was moved up by scrolling).
- If the given position is outside the text area, it returns `'\0'`.

```C++
bool remotemo::Remotemo::is_inverse_at(int column, int line);
bool remotemo::Remotemo::is_inverse_at(const SDL_Point& pos);
```
- If the character at the given position was printed with the foreground and
  background colors inversed, it returns `true`.
- If the character at the given position was printed with the foreground and
  background colors unchanged, it return `false`.
- If the given position is outside the text area, it returns `false`.

<sup>[Back to top](#remotemo-api-design)</sup>
### Text output behaviour

```C++
bool remotemo::Remotemo::set_text_delay(int delay_in_ms);
bool remotemo::Remotemo::set_text_speed(double char_per_second);
int remotemo::Remotemo::get_text_delay();
double remotemo::Remotemo::get_text_speed();
```
Sets and gets the speed at which text gets displayed.

The setters return `true` on success and `false` on failure (e.g. negative
numbers or trying to set the speed to zero).

Don't expect this to be anything precise as it sets the delay between
characters being displayed, in **whole** milliseconds, without taking into
account the time it takes to actually display each character (nor the time
that might go into handling any possible events that might happens at the same
time).

Also note that when converting from `char per seconds` to `delay`, then some
rounding might happen (e.g. any speed above 1000 char per second will probably
be rounded down to a delay of 0 ms).

```C++
void remotemo::Remotemo::set_scrolling(bool scrolling);
bool remotemo::Remotemo::get_scrolling();
```

Sets and gets the 'scrolling' property. See [Scrolling](#scrolling) above for
more.

```C++
void remotemo::Remotemo::set_wrapping(remotemo::Wrapping wrapping);
remotemo::Wrapping remotemo::Remotemo::get_wrapping();
```

Sets and gets the 'wrapping' property. See [Wrapping](#wrapping) above for
more.

```C++
void remotemo::Remotemo::set_inverse(bool inverse);
bool remotemo::Remotemo::get_inverse();
```

Sets and gets the 'inverse' property.

While set to `true`, printing to the screen is done with the foreground and
background color switched. Changing this property does not affect text that
had already been printed to the screen.

<sup>[Back to top](#remotemo-api-design)</sup>
### Text area settings

```C++
int remotemo::Remotemo::set_text_area_size(int columns, int lines);
int remotemo::Remotemo::set_text_area_size(const SDL_Point& size);
int remotemo::Remotemo::get_text_area_columns();
int remotemo::Remotemo::get_text_area_lines();
SDL_Point remotemo::Remotemo::get_text_area_size();

int remotemo::Remotemo::set_text_blend_mode(SDL_BlendMode mode);
SDL_BlendMode remotemo::Remotemo::get_text_blend_mode();

int remotemo::Remotemo::set_text_color(Uint8 red, Uint8 green, Uint8 blue);
int remotemo::Remotemo::set_text_color(const remotemo::Color& color);
remotemo::Color remotemo::Remotemo::get_text_color();
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Window settings

```C++
int remotemo::Remotemo::set_window_title(const std::string& title);
std::string remotemo::Remotemo::get_window_title();
int remotemo::Remotemo::set_window_size(int width, int height);
int remotemo::Remotemo::set_window_size(const SDL_Point& size);
SDL_Point remotemo::Remotemo::get_window_size();
int remotemo::Remotemo::set_window_position(int x, int y);
int remotemo::Remotemo::set_window_position(const SDL_Point& position);
SDL_Point remotemo::Remotemo::get_window_position();
int remotemo::Remotemo::set_window_resizable(bool is_resizable);
bool remotemo::Remotemo::get_window_resizable();
int remotemo::Remotemo::set_window_fullscreen(bool is_fullscreen);
bool remotemo::Remotemo::get_window_fullscreen();

int remotemo::Remotemo::set_key_fullscreen(); // Set to none
int remotemo::Remotemo::set_key_fullscreen(remotemo::Mod_keys modifier_keys,
        remotemo::F_key key);
int remotemo::Remotemo::set_key_fullscreen(
        remotemo::Mod_keys_strict modifier_keys, remotemo::Key key);
std::optional<remotemo::Key_combo> remotemo::Remotemo::get_key_fullscreen();

int remotemo::Remotemo::set_key_close_window(); // Set to none
int remotemo::Remotemo::set_key_close_window(remotemo::Mod_keys modifier_keys,
        remotemo::F_key key);
int remotemo::Remotemo::set_key_close_window(
        remotemo::Mod_keys_strict modifier_keys, remotemo::Key key);
std::optional<remotemo::Key_combo> remotemo::Remotemo::get_key_close_window();

int remotemo::Remotemo::set_key_quit(); // Set to none
int remotemo::Remotemo::set_key_quit(remotemo::Mod_keys modifier_keys,
        remotemo::F_key key);
int remotemo::Remotemo::set_key_quit(remotemo::Mod_keys_strict modifier_keys,
        remotemo::Key key);
std::optional<remotemo::Key_combo> remotemo::Remotemo::get_key_quit();

int remotemo::Remotemo::set_closing_same_as_quit(bool is_closing_same_as_quit);
bool remotemo::Remotemo::get_closing_same_as_quit();
int remotemo::Remotemo::set_function_pre_close(std::function<bool()> func);
int remotemo::Remotemo::set_function_pre_quit(std::function<bool()> func);
```

<sup>[Back to top](#remotemo-api-design)</sup>
