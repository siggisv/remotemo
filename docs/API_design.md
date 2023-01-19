# remoTemo API design
_2nd draft_

> **Warning**
>
> **Both** the design and the implementation are work-in-progress.

> **TODO?** Make thread safe? Or just explain that just like most of the
> underlying SDL2 framework, this object should only be used from within the
> thread where SDL was initialized?

## Table of contents
- [Initialization, cleanup and config](#initialization-cleanup-and-config)
  - [Initialization](#initialization)
  - [Cleanup](#cleanup)
  - [Configuration and default settings](#configuration-and-default-settings)
- [Using remoTemo](#using-remotemo)
  - [Interact with the monitor](#interact-with-the-monitor)
  - [Change text output behaviour](#change-text-output-behaviour)
  - [Change text area settings](#change-text-area-settings)
  - [Change window settings](#change-window-settings)

<sup>[Back to top](#remotemo-api-design)</sup>
## Initialization, cleanup and config
### Initialization

```C++
std::optional<remoTemo::Temo> remoTemo::create();
std::optional<remoTemo::Temo> remoTemo::create(
        const remoTemo::Config& config);
```

This non-member function will create and initialize an object representing
a text monitor, the window showing it, an ASCII-only keyboard and their
properties (along with the needed properties for the underlying SDL2-library).

- If successful, that function will return a `std::optional<remoTemo::Temo>`
  containing the object. It will also create an internal list of the resources
  that its destructor should take care of cleaning up.
  > **Note** `remoTemo::Temo` is movable but not copyable. It is therefore
  > recommended to pass it along by reference or to use a smart pointer.

- On failure, it will log an error using `SDL_Log()`, free/release all
  resources it had succeeded setting up and then return an empty
  `std::optional` object.

  > **Warning**
  > On failure, it will clean up all the resources that the `remoTemo::Temo`'s
  > destructor is supposed to handle. Meaning that by default it would also
  > clean up all those resources handled to it.

When invoked without any parameters, it will set up the underlying
SDL2-library and create a window for you with the default settings (including
loading a background texture and the font-bitmap from their default
locations). Most of those settings can not be changed after creation.

To allow you to initialize using other settings than the default ones, the
[Named Parameter
Idiom](https://isocpp.org/wiki/faq/ctors#named-parameter-idiom) is followed,
providing you with a "config" object that when created, will contains the
default initialization settings:

```C++
remoTemo::Config::Config();

```

_The constructor that creates a `remoTemo::Config` object._

You can then pick which settings to change by calling its setters. Then call
`remoTemo::create(const remoTemo::Config& config)` to create and initialize
everything with the settings you wanted.

- You can store that `remoTemo::Config` object in a variable, work on it and
  then pass it to `remoTemo::create()`:

  ```C++
  remoTemo::Config my_config;
  if (some_condition) {
      my_config.window_size(1920, 1080);
  }
  my_config.background_file("background.png")
          .background_min_area(200, 120, 1000, 700);
  auto text_monitor = remoTemo::create(my_config);
  ```
  
- Or you could simply call `remoTemo::create()` directly with a temporary
  `remoTemo::Config` object:
  
  ```C++
  auto text_monitor = remoTemo::create(
          remoTemo::Config()
                  .window_size(1920, 1080)
                  .background_file("background.png")
                  .background_min_area(200, 120, 1000, 700));
  ```

<sup>[Back to top](#remotemo-api-design)</sup>
### Cleanup

```C++
remoTemo::Temo::~Temo();
```

_The destructor, (by default) in charge of deleting/closing textures, the
renderer, the window and quitting SDL._

- By default, the destructor of the `remoTemo::Temo` object will handle cleaning
  up all SDL-resources it used (even those it did not create itself) and then
  call `SDL_Quit()` to clean up all SDL subsystems.

- When created, the `remoTemo::Temo` object can be set not to have its
  destructor handle cleaning up **all** resources (e.g. if you wanted to create
  the window yourself to display some menu before and after having remoTemo
  taking control of the window).  In that case you have the responsibility of
  cleaning up those resources and to call `SDL_Quit()`.
  > **Note**
  > Even when set to not clean up everything, it will still clean up those
  > resources it did create itself.

If, for some reason, you want to clean up sooner, you could call:

```C++
void remoTemo::Temo::quit();
```
> **Warning**
> Doing so will leave the `remoTemo::Temo` object invalid. If you really have
> to do that, make sure not to touch any of the object's functions after that.

<sup>[Back to top](#remotemo-api-design)</sup>
### Configuration and default settings

The following are the default settings when creating an `remoTemo::Temo`
object, with a short description of the different options for each setting and
which of `remoTemo::Config`'s setters can be used to change that setting.

Unless noted, the settings can not be changed after the `remoTemo::Temo`
object has been created:

- Cleanup all: `true`
  ```C++
  remoTemo::Config& remoTemo::Config::cleanup_all(bool cleanup_all);
  ```
  - If `true`, then the destructor of the `remoTemo::Temo` object will handle
    cleaning up all SDL-resources it used and then call `SDL_Quit()` to clean
    up all SDL subsystems.

    > **Warning**
    > It will clean up any window, renderer and texture it used, no matter if
    > it did created them itself or they were passed to it.

  - If `false`, then it will only clean up those resources it did create (but
    none of those you passed to it). You must then handle cleaning up those
    and quit SDL afterwards yourself.

- The window: `nullptr`
  ```C++
  remoTemo::Config& remoTemo::Config::the_window(SDL_Window* window);
  ```
  - If set to `nullptr` then `remoTemo::create()` will create the window with
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
    remoTemo::Config& remoTemo::Config::window_title(const std::string& title);
    remoTemo::Config& remoTemo::Config::window_size(int width, int height);
    remoTemo::Config& remoTemo::Config::window_size(const SDL_Point& size);
    remoTemo::Config& remoTemo::Config::window_position(int x, int y);
    remoTemo::Config& remoTemo::Config::window_position(const SDL_Point& pos);
    remoTemo::Config& remoTemo::Config::window_resizable(bool is_resizable);
    remoTemo::Config& remoTemo::Config::window_fullscreen(bool is_fullscreen);
    ```

  - If NOT set to `nullptr` then 'the window' must be a pointer to a valid
    `SDL_Window`.

    > **Warning** It will be your responsability to keep that pointed window
    > valid while `remoTemo::Temo` is alive.
    > 
    > You also have to take care of closing it afterward (unless 'Cleanup all'
    > is set to `true`, in which case `remoTemo::Temo`'s destructor will
    > handle that).

    > **Warning** If that window has a renderer associated with it, then that
    > renderer must have the `SDL_RENDERER_TARGETTEXTURE` flag set. Otherwise
    > `remoTemo::create()` will fail.

  No mater if 'the window' is set to `nullptr` or not, the following
  properties tied to the window will be set (with the following being the
  default):
  - Key to switch between fullscreen and windowed mode: `F11` _(can be changed
    later)_
    ```C++
    remoTemo::Config& remoTemo::Config::key_fullscreen(
            remoTemo::F_key key);
    remoTemo::Config& remoTemo::Config::key_fullscreen(
            SDL_Keymod modifier_keys, remoTemo::F_key key);
    remoTemo::Config& remoTemo::Config::key_fullscreen(
            SDL_Keymod modifier_keys, remoTemo::Key key);
    ```
  - Key combination to close the window: `Ctrl-w` _(can be changed later)_
    ```C++
    remoTemo::Config& remoTemo::Config::key_close_window(
            remoTemo::F_key key);
    remoTemo::Config& remoTemo::Config::key_close_window(
            SDL_Keymod modifier_keys, remoTemo::F_key key);
    remoTemo::Config& remoTemo::Config::key_close_window(
            SDL_Keymod modifier_keys, remoTemo::Key key);
    ```
  - Key combination to quit the application: `Ctrl-q` _(can be changed later)_
    ```C++
    remoTemo::Config& remoTemo::Config::key_quit(
            remoTemo::F_key key);
    remoTemo::Config& remoTemo::Config::key_quit(
            SDL_Keymod modifier_keys, remoTemo::F_key key);
    remoTemo::Config& remoTemo::Config::key_quit(
            SDL_Keymod modifier_keys, remoTemo::Key key);
    ```
  - Closing window is the same as quitting: `true` _(can be changed
    later)_
    ```C++
    remoTemo::Config& remoTemo::Config::closing_same_as_quit(
            bool is_closing_same_as_quit);
    ```
    > **Note** Not recommended to change that setting to `false` unless you
    > either really want the application to continue running without a window
    > or you have taken care of opening another window yourself.
  - Function to call before closing window: `nullptr` _(can be changed later)_
    ```C++
    remoTemo::Config& remoTemo::Config::function_pre_quit(
            std::function<bool()> func);
    remoTemo::Config& remoTemo::Config::function_pre_quit(
            std::function<bool(remoTemo::Temo*)> func);
    ```
    If set, that function can be used to e.g. offer the user the option to
    cancel that action or to save before continuing. That function must return
    a `bool`. Returning `false` will cancel closing the window.

  Finally, if 'the window' was set to `nullptr` **OR** if it is pointing to a
  `SDL_Window` that doesn't have a renderer then a `SDL_Renderer` will be
  created (either tied to that newly created window or to the one pointed to)
  and that renderer set as being owned by this object (meaning the destructor
  will be in charge of destroying it).

- Background: `nullptr`
  ```C++
  remoTemo::Config& remoTemo::Config::background(SDL_Texture* background);
  ```

  - If set to `nullptr` then the background texture will be loaded from the
    following file and set as being owned by this object (meaning the
    destructor will be in charge of destroying it):
    - Background file path: `"res/img/terminal_screen.png"`
      (or `"res\img\terminal_screen.png"` depending on the operating system).
      ```C++
      remoTemo::Config& remoTemo::Config::background_file_path(
              const std::string& file_path);
      ```

      When changing this setting, you could use the constant
      `remoTemo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
      operating system) for the path separator when creating the path, if you
      care about it being cross-platform.


      > **Note** If this setting is set to a string starting with the path
      > separator (or a drive letter, if on Windows. E.g. "D:") then the path
      > is an absolute path.
      >
      > Otherwise the path is relative to your executable program file.

  - If NOT set to `nullptr` then 'background' must point to an `SDL_Texture`
    containing the desired background image.

    > **Warning** It will be your responsability to keep that pointed texture
    > valid while `remoTemo::Temo` is alive.
    > 
    > You also have to take care of closing it afterward (unless 'Cleanup all'
    > is set to `true`, in which case `remoTemo::Temo`'s destructor will
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
  remoTemo::Config& remoTemo::Config::background_min_area(int x, int y,
          int w, int h);
  remoTemo::Config& remoTemo::Config::background_text_area(float x, float y,
          float width, float height);
  ```
  > **Note** x and y are counted from the top-left corner, both starting at
  > zero.

- Font-bitmap: `nullptr`
  ```C++
  remoTemo::Config& remoTemo::Config::font_bitmap(SDL_Texture* font_bitmap);
  ```
  - If set to `nullptr` then the font-bitmap texture will be loaded from the
    following file and set as being owned by this object (meaning the
    destructor will be in charge of destroying it):
    - Font-bitmap path: `"res/img/font_bitmap.png"`
      (or `"res\img\font_bitmap.png"` depending on the operating system).
      ```C++
      remoTemo::Config& remoTemo::Config::font_bitmap_file_path(
              const std::string& file_path);
      ```

      When changing this setting, you could use the constant
      `remoTemo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
      operating system) for the path separator when creating the path, if you
      care about it being cross-platform.

      > **Note** If this setting is set to a string starting with the path
      > separator (or a drive letter, if on Windows. E.g. "D:") then the path
      > is an absolute path.
      >
      > Otherwise the path is relative to your executable program file.

  - If NOT set to `nullptr` then 'font-bitmap' must point to an `SDL_Texture`
    containing the desired font-bitmap image.

    > **Warning** It will be your responsability to keep that pointed texture
    > valid while `remoTemo::Temo` is alive.
    > 
    > You also have to take care of closing it afterward (unless 'Cleanup all'
    > is set to `true`, in which case `remoTemo::Temo`'s destructor will
    > handle that).

  No mater if 'font-bitmap' is set to `nullptr` or not, the following setting
  (with the following default) will be used:
  - Font size: `width: 7, height: 18`
    \
    This is the size, in pixels, of each character as they are drawn in the
    font-bitmap file.
    ```C++
    remoTemo::Config& remoTemo::Config::font_size(int width, int height);
    remoTemo::Config& remoTemo::Config::font_size(const SDL_Point& size);
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
  remoTemo::Config& remoTemo::Config::text_area_size(int columns, int lines);
  remoTemo::Config& remoTemo::Config::text_area_size(const SDL_Point& size);
  remoTemo::Config& remoTemo::Config::text_blend_mode(SDL_BlendMode mode);
  remoTemo::Config& remoTemo::Config::text_color(Uint8 red, Uint8 green,
          Uint8 blue);
  remoTemo::Config& remoTemo::Config::text_color(const remoTemo::Color& color);
  ```

The following properties can not be changed before creating the
`remoTemo::Temo` object. They control the behaviour of the text output and
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
- text wrapping: `off`
  \
  This can be set to `off` (text printed beyond the right border gets lost),
  `char` (text wraps to next line. This might happen in the middle of a
  word) or `word` (text wraps to the next line, if possible at the last
  whitespace before getting to the right border).

<sup>[Back to top](#remotemo-api-design)</sup>
## Using remoTemo
### Interact with the monitor

```C++
int remoTemo::Temo::move_cursor(int x, int y);
int remoTemo::Temo::move_cursor(const SDL_Point& move);
int remoTemo::Temo::set_cursor(int column, int line);
int remoTemo::Temo::set_cursor(const SDL_Point& position);
int remoTemo::Temo::set_cursor_column(int column);
int remoTemo::Temo::set_cursor_line(int line);
int remoTemo::Temo::set_cursor_x(int column);
int remoTemo::Temo::set_cursor_y(int line);
SDL_Point remoTemo::Temo::get_cursor_position();

int remoTemo::Temo::pause(int pause);
int remoTemo::Temo::clear();
remoTemo::Key remoTemo::Temo::get_key();
std::string remoTemo::Temo::get_input(int max_length);

int remoTemo::Temo::print(conts std::string& text);
int remoTemo::Temo::print_at(int column, int line, const std::string& text);
int remoTemo::Temo::print_at(const SDL_Point& position,
        const std::string& text);
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change text output behaviour

```C++
enum class remoTemo::Wrapping {off, char, word};

int remoTemo::Temo::set_text_delay(int delay);
int remoTemo::Temo::set_text_speed(int speed);
int remoTemo::Temo::set_scrolling(bool scrolling);
int remoTemo::Temo::set_wrapping(remoTemo::Wrapping wrapping);
int remoTemo::Temo::set_inverse(bool inverse);

int remoTemo::Temo::get_text_delay();
int remoTemo::Temo::get_text_speed();
bool remoTemo::Temo::get_scrolling();
remoTemo::Wrapping remoTemo::Temo::get_wrapping();
bool remoTemo::Temo::get_inverse();
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change text area settings

```C++
struct remoTemo::Color {Uint8 red; Uint8 green; Uint8 blue;};

int remoTemo::Temo::set_text_area_size(int columns, int lines);
int remoTemo::Temo::set_text_area_size(const SDL_Point& size);
int remoTemo::Temo::get_text_area_columns();
int remoTemo::Temo::get_text_area_lines();
SDL_Point remoTemo::Temo::get_text_area_size();

int remoTemo::Temo::set_text_blend_mode(SDL_BlendMode mode);
SDL_BlendMode remoTemo::Temo::get_text_blend_mode();

int remoTemo::Temo::set_text_color(Uint8 red, Uint8 green, Uint8 blue);
int remoTemo::Temo::set_text_color(const remoTemo::Color& color);
remoTemo::Color remoTemo::Temo::get_text_color();
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change window settings

```C++
enum class remoTemo::F_key {
    none, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12};
enum class remoTemo::Key {
    K_esc, K_backquote, K_1, K_2, ... , K_0, K_minus, K_equal, K_backspace,
    K_tab, K_q, K_w, ... K_p, K_left_bracket, K_right_bracket, K_backslash,
    ...
    ...
    K_space};
    /* Basically all the keys on a regular keyboard, except F-keys, the
       keypad and modifier keys */

int remoTemo::Temo::set_window_title(const std::string& title);
std::string remoTemo::Temo::get_window_title();
int remoTemo::Temo::set_window_size(int width, int height);
int remoTemo::Temo::set_window_size(const SDL_Point& size);
SDL_Point remoTemo::Temo::get_window_size();
int remoTemo::Temo::set_window_position(int x, int y);
int remoTemo::Temo::set_window_position(const SDL_Point& position);
SDL_Point remoTemo::Temo::get_window_position();
int remoTemo::Temo::set_window_resizable(bool is_resizable);
bool remoTemo::Temo::get_window_resizable();
int remoTemo::Temo::set_window_fullscreen(bool is_fullscreen);
bool remoTemo::Temo::get_window_fullscreen();

int remoTemo::Temo::set_key_fullscreen(SDL_Keymod modifier_keys,
        remoTemo::F_key key);
int remoTemo::Temo::set_key_fullscreen(SDL_Keymod modifier_keys, remoTemo::Key key);
std::tuple<SDL_Keymod, remoTemo::F_key, remoTemo::Key>
        remoTemo::Temo::get_key_fullscreen();
int remoTemo::Temo::set_key_close_window(SDL_Keymod modifier_keys,
        remoTemo::F_key key);
int remoTemo::Temo::set_key_close_window(SDL_Keymod modifier_keys, remoTemo::Key key);
std::tuple<SDL_Keymod, remoTemo::F_key, remoTemo::Key>
        remoTemo::Temo::get_key_close_window();
int remoTemo::Temo::set_key_quit(SDL_Keymod modifier_keys, remoTemo::F_key key);
int remoTemo::Temo::set_key_quit(SDL_Keymod modifier_keys, remoTemo::Key key);
std::tuple<SDL_Keymod, remoTemo::F_key, remoTemo::Key>
        remoTemo::Temo::get_key_quit();

int remoTemo::Temo::set_closing_same_as_quit(bool is_closing_same_as_quit);
bool remoTemo::Temo::get_closing_same_as_quit();
int remoTemo::Temo::set_function_pre_quit(std::function<bool()> func);
int remoTemo::Temo::set_function_pre_quit(
        std::function<bool(remoTemo::Temo*)> func);
```

<sup>[Back to top](#remotemo-api-design)</sup>
