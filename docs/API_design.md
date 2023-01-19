# remoTemo API design
_2nd draft_

> **Warning**
>
> **Both** the design and the implementation are work-in-progress.

## Table of contents
- [Initialization, settings and cleanup](#initialization-settings-and-cleanup)
  - [Initialization](#initialization)
  - [Default settings](#default-settings)
  - [Cleanup](#cleanup)
- [Using remoTemo](#using-remotemo)
  - [Interact with the monitor](#interact-with-the-monitor)
  - [Change text output behaviour](#change-text-output-behaviour)
  - [Change text area settings](#change-text-area-settings)
  - [Change window settings](#change-window-settings)

<sup>[Back to top](#remotemo-api-design)</sup>
## Initialization, settings and cleanup
> **TODO?** Make thread safe? Or just explain that just like most of the
> underlying SDL2 framework, this object should only be used from within the
> thread where it was initialized?

<sup>[Back to top](#remotemo-api-design)</sup>
### Initialization

```C++
std::optional<remoTemo::Temo> remoTemo::create();
std::optional<remoTemo::Temo> remoTemo::create(
        const remoTemo::Config& config);
```

This non-member function will create and initialize an object representing
a text monitor, the window showing it and their properties (along with the
needed properties for the underlying SDL2-library).

- If successful, it will return a `std::optional<remoTemo::Temo>` containing
  the object. It will also create an internal list of the resources that its
  destructor should take care of cleaning up.
- On failure, it will log an error using `SDL_Log()`, free/release all
  resources it had succeeded setting up and then return an empty
  `std::optional` object.

  > **Warning**
  >
  > On failure, it will clean up all the resources that the `remoTemo::Temo`'s
  > destructor is supposed to handle. Meaning that:
  > - If 'Handle SDL_Quit' was set to `true`, it will clean up all the SDL
  >   subsystems and all resources (both those created by `remoTemo::Create`
  >   and **also** those passed to it.
  > - If 'Handle SDL_Quit' was set to `false`, it will **only** clean up
  >   those resources it did create itself, leaving to you to clean up
  >   anything you passed to it.

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

- You can create a `remoTemo::Config` object first, then after using the
  setters of your choice, pass it to `remoTemo::create()`:

  ```C++
  remoTemo::Config my_config;
  my_config.window_size(1920, 1080)
          .background_file("background.png")
          .background_min_area(200, 120, 1000, 700);
  auto text_monitor = remoTemo::create(my_config);
  ```
  
- Or you could call `remoTemo::create()` directly with a temporary
  `remoTemo::Config` object:
  
  ```C++
  auto text_monitor = remoTemo::create(
          remoTemo::Config()
                  .window_size(1920, 1080)
                  .background_file("background.png")
                  .background_min_area(200, 120, 1000, 700));
  ```

> ~~**TODO** Consider using `std::string_view` instead of `std::string const&`
> in parameters (See: [stackoverflow answer about
> std::string_view](https://stackoverflow.com/a/40129198/4766261)).~~
>
> **Nope.** Quote: "[...] what you lose by using `std::string_view`. [...] You
> lose implicit null termination [...]" and we absolutely need that when
> passing those strings along to the underlying SDL-library that is writen in
> C.

List of `remoTemo::Config`'s setters:

```C++
remoTemo::Config& remoTemo::Config::handle_SDL_Quit(bool handle_SDL_Quit);
remoTemo::Config& remoTemo::Config::the_window(SDL_Window* window);
remoTemo::Config& remoTemo::Config::window_title(const std::string& title);
remoTemo::Config& remoTemo::Config::window_size(int width, int height);
remoTemo::Config& remoTemo::Config::window_size(const SDL_Point& size);
remoTemo::Config& remoTemo::Config::window_position(int x, int y);
remoTemo::Config& remoTemo::Config::window_position(const SDL_Point& position);
remoTemo::Config& remoTemo::Config::window_resizable(bool is_resizable);
remoTemo::Config& remoTemo::Config::window_fullscreen(bool is_fullscreen);
remoTemo::Config& remoTemo::Config::key_fullscreen(
        SDL_Keymod modifier_keys, remoTemo::F_key key);
remoTemo::Config& remoTemo::Config::key_fullscreen(
        SDL_Keymod modifier_keys, remoTemo::Key key);
remoTemo::Config& remoTemo::Config::key_close_window(
        SDL_Keymod modifier_keys, remoTemo::F_key key);
remoTemo::Config& remoTemo::Config::key_close_window(
        SDL_Keymod modifier_keys, remoTemo::Key key);
remoTemo::Config& remoTemo::Config::key_quit(
        SDL_Keymod modifier_keys, remoTemo::F_key key);
remoTemo::Config& remoTemo::Config::key_quit(
        SDL_Keymod modifier_keys, remoTemo::Key key);
remoTemo::Config& remoTemo::Config::closing_same_as_quit(
        bool is_closing_same_as_quit);
remoTemo::Config& remoTemo::Config::function_pre_quit(
        std::function<bool()> func);
remoTemo::Config& remoTemo::Config::function_pre_quit(
        std::function<bool(remoTemo::Temo*)> func);
remoTemo::Config& remoTemo::Config::background(SDL_Texture* background);
remoTemo::Config& remoTemo::Config::background_file_path(
        const std::string& file_path);
remoTemo::Config& remoTemo::Config::background_min_area(int x, int y,
        int w, int h);
remoTemo::Config& remoTemo::Config::background_text_area(float x, float y,
        float width, float height);
remoTemo::Config& remoTemo::Config::font_bitmap(SDL_Texture* font_bitmap);
remoTemo::Config& remoTemo::Config::font_bitmap_file_path(
        const std::string& file_path);
remoTemo::Config& remoTemo::Config::font_size(int width, int height);
remoTemo::Config& remoTemo::Config::font_size(const SDL_Point& size);
remoTemo::Config& remoTemo::Config::text_area_size(int columns, int lines);
remoTemo::Config& remoTemo::Config::text_area_size(const SDL_Point& size);
remoTemo::Config& remoTemo::Config::text_blend_mode(SDL_BlendMode mode);
remoTemo::Config& remoTemo::Config::text_color(Uint8 red, Uint8 green,
        Uint8 blue);
remoTemo::Config& remoTemo::Config::text_color(const remoTemo::Color& color);
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Default settings

The following are the default settings with a short explanation for each one.
Unless noted, they can not be changed after the `remoTemo::Temo` object has
been created:

- Handle SDL_Quit: `true`
  - If `true`, then the destructor of the `remoTemo::Temo` object will handle
    cleaning up all SDL subsystems by calling `SDL_Quit()`.
    > **Warning**
    > It will also handle cleaning up any window, renderer and texture it used
    > (no mater if it did created them itself or they were passed to it).
  - If `false`, then you must handle cleaning up SDL afterwards yourself.
    > **Note** that the destructor of the `remoTemo::Temo` object will still
    > clean up any window, renderer and/or texture it did create (but none of
    > those passed to it).

- The window: `nullptr`
  - If set to `nullptr` then `remoTemo::create()` will create the window with
    the following settings and set it as being owned by this object (meaning
    the destructor will be in charge of destroying it).
    - title: `Retro Monochrome Text Monitor` _(can be changed later)_
    - width: `1280` _(can be changed later)_
    - height: `720` _(can be changed later)_
    - position, x: `SDL_WINDOWPOS_UNDEFINED` _(can be changed later)_
    - position, y: `SDL_WINDOWPOS_UNDEFINED` _(can be changed later)_
    - resizable: `true` _(can be changed later)_
    - fullscreen: `false` _(can be changed later)_

  - If NOT set to `nullptr` then it must be a pointer to a valid `SDL_Window`.

    > **Warning** It will be your responsability to keep that pointed window
    > valid while `remoTemo::Temo` is alive.
    > 
    > You also have to take care of closing it afterward (unless 'Handle
    > SDL_Quit' is set to `true`, in which case `remoTemo::Temo`'s destructor
    > will handle that).

    > **Warning** If that window has a renderer associated with it, then that
    > renderer must have the `SDL_RENDERER_TARGETTEXTURE` flag set. Otherwise
    > `remoTemo::create()` will fail.

  No mater if 'the window' is set to `nullptr` or not, the following
  properties tied to the window will be set with the following being the
  default:
  - Key to switch between fullscreen and windowed mode: `F11` _(can be changed
    later)_
  - Key combination to close the window: `Ctrl-w` _(can be changed later)_
  - Key combination to quit the application: `Ctrl-q` _(can be changed later)_
  - Closing window is the same as quitting: `true` _(can be changed
    later)_
    > **Note** Not recommended to change that setting to `false` unless you
    > either really want the application to continue running without a window
    > or you have taken care of opening another window yourself.
  - Function to call before closing window: `nullptr` _(can be changed later)_
    \
    If set, that function can be used to e.g. offer the user the option to
    cancel that action or to save before continuing. That function must return
    a `bool`. Returning `false` will cancel closing the window.

  Finally, if 'the window' was set to `nullptr` **OR** if it is pointing to a
  `SDL_Window` that doesn't have a renderer then a `SDL_Renderer` will be
  created (either tied to that newly created window or to the one pointed to)
  and that renderer set as being owned by this object (meaning the destructor
  will be in charge of destroying it).

- Background texture: `nullptr`

  - If set to `nullptr` then the background texture will be loaded from the
    following file and set as being owned by this object (meaning the
    destructor will be in charge of destroying it):
    - Background file path: `res/img/terminal_screen.png`
      (or `res\img\terminal_screen.png` depending on the operating system).

      When changing this setting, you could use the constant
      `remoTemo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
      operating system) for the path separator when creating the path, if you
      care about it being cross-platform.

      If this setting is set to a string starting with the path separator (or
      a drive letter, e.g. "D:", if on Windows) then the path is an absolute
      path.

      Otherwise the path is relative to your executable program file.

  - If NOT set to `nullptr` then it must point to an `SDL_Texture` containing
    the desired background image.

    > **Warning** It will be your responsability to keep that pointed texture
    > valid while `remoTemo::Temo` is alive.
    > 
    > You also have to take care of closing it afterward (unless 'Handle
    > SDL_Quit' is set to `true`, in which case `remoTemo::Temo`'s destructor
    > will handle that).

  No mater if set to `nullptr` or not, the following settings will be used,
  with those defaults:
  - minimum area to be shown:
    `x: 118, y: 95, width: 700, height: 540`
    \
    This setting allows for the background texture to include borders around
    the main area. That border can then either be shown to the sides of that
    area (if the ratio of the window is wider) or above and below that area
    (if the ratio of the window is narrower).
  - the area of the background where the text is to be drawn:
    `x: 188.25f, y: 149.25f, w: 560.0f, h: 432.0f`
  > **Note** x and y are counted from the top-left corner, both starting at
  > zero.

- Font-bitmap texture: `nullptr`

  - If set to `nullptr` then the font-bitmap texture will be loaded from the
    following file and set as being owned by this object (meaning the
    destructor will be in charge of destroying it):
    - Font-bitmap path: `res/img/font_bitmap.png`
      \
      (or `res\img\font_bitmap.png` depending on the operating system).

      When changing this setting, you could use the constant
      `remoTemo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
      operating system) for the path separator when creating the path, if you
      care about it being cross-platform.

      > **Note** If this setting is set to a string starting with the path
      > separator (or a drive letter, e.g. "D:", if on Windows) then the path
      > is an absolute path.
      >
      > Otherwise the path is relative to your executable program file.

  - If NOT set to `nullptr` then it must point to an `SDL_Texture` containing
    the desired font-bitmap image.

    > **Warning** It will be your responsability to keep that pointed texture
    > valid while `remoTemo::Temo` is alive.
    > 
    > You also have to take care of closing it afterward (unless 'Handle
    > SDL_Quit' is set to `true`, in which case `remoTemo::Temo`'s destructor
    > will handle that).

  No mater if set to `nullptr` or not, the following settings will be used,
  with those defaults:
  - Font size: `width: 7, height: 18`
    \
    This is the size, in pixels, of each character as they are drawn in the
    font-bitmap file.

- The following are the default setting used to create the texture where the
  text will be drawn before being rendered on top of the background:
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
### Cleanup

remoTemo follows the philosophy that it handles cleaning up those resources
that it did set up but leaves to you the responsibility of cleaning up those
resources that you passed to it.

For most uses, allowing the destructor to handle cleaning up those resources
when the `remoTemo::Temo` object gets destroyed (whether manually or
automatically e.g. when going out of scope).

```C++
remoTemo::Temo::~Temo();
```

The destructor, in charge of deleting/closing textures, the renderer, the
window and quitting SDL (unless the `remoTemo::Temo` object was configured to
not handle some of those, e.g. if you wanted to create the window yourself so
it can display some menu before and after having remoTemo taking control of
the window).

If, for some reason, you want to clean up sooner, you can call:

```C++
void remoTemo::Temo::quit();
```

Just make sure not to touch any of the object's functions after that.

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

