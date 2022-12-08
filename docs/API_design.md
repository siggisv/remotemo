# ReMoTeMo API design
_1st draft_

## Warning! Work in progress

**Both** the design and the implementation are work-in-progress.

## Table of contents
- [Initialization, settings and cleanup](#initialization-settings-and-cleanup)
  - [Initialization](#initialization)
  - [Default settings](#default-settings)
  - [Cleanup](#cleanup)
- [Using ReMoTeMo](#using-remotemo)
  - [Interact with the monitor](#interact-with-the-monitor)
  - [Change text output behaviour](#change-text-output-behaviour)
  - [Change text area settings](#change-text-area-settings)
  - [Change window settings](#change-window-settings)

<sup>[Back to top](#remotemo-api-design)</sup>
## Initialization, settings and cleanup
**TODO?** Make thread safe? Or just explain that just like most of the
underlying SDL2 framework, this object should only be used from within the
thread where it was initialized?

<sup>[Back to top](#remotemo-api-design)</sup>
### Initialization

```C++
std::pair<remo::Temo, bool> remo::Temo::create();
std::pair<remo::Temo, bool> remo::Temo::create(const remo::InitConfig &config);
```

This is a static method that will create and initialize an object representing
a text monitor, the window showing it and their properties (along with the
needed properties of the underlying SDL2-library).

- If successful, it will return a `std::pair<remo::Temo, bool>` containing the
  object and the boolean set to `true`. It will also create an internal list
  of the resources that it did set up so that whenever/however the `remo::Temo`
  object gets deleted, its destructor can take care of cleaning them up.
- If unsuccessful, it will log an error using `SDL_Log()`, free/release all
  resources it had succeeded setting up and then return a `std::pair`
  containing an uninitialized `remo::Temo` object and the boolean set to
  `false`.
\
\
**TODO?** On failure return a `remo::Noop` object, a subclass of `remo::Temo`,
where all methods are overwritten to do nothing except log an error - just in
case you didn't check the status of the returned boolean.

When invoked without any parameters, it will set up the underlying
SDL2-library and create a window for you with the default settings (including
loading a background texture and the font-bitmap from their default
locations). Most of the settings can not be changed after creation.

To initialize with other settings than the default ones, first create a
`remo::InitConfig` object:

```C++
remo::InitConfig::InitConfig();

```

_The constructor that creates a `remo::InitConfig` object._

When created, it will contains the default initialization settings. You can
then pick which settings to change by calling its setters. Then call
`remo::Temo::create(const remo::InitConfig &config)` to create and initialize
everything with the changed settings.

The setters can be chained, e.g.:

```C++
remo::InitConfig myconfig;
myconfig.setWindowWidth(1920).setWindowHeight(1080)
        .setBackgroundFile("background.png")
        .setBackgroundMinArea(200, 120, 1000, 700);
std::pair<remo::Temo, bool> tM = remo::Temo::create(myconfig);
```

**TODO?** List all the setters of the config class.

<sup>[Back to top](#remotemo-api-design)</sup>
### Default settings

The following are the default settings with a short explanation for each one.
Unless noted, they can not be changed after the `remo::Temo` object has been
created:
- Is SDL initialized: `false`

  If `false`, then SDL will be initialized when calling the `create()` method
  AND the destructor of the `remo::Temo` object will handle cleaning it up
  (meaning that the destructor will be in charge of calling `SDL_Quit();`).

  If `true`, then you must handle both setting up SDL before calling the
  `create()` method AND also cleaning up SDL afterwards.
- The window: `nullptr`

  If NOT set to `nullptr` then it must be a pointer to a valid `SDL_Window`.
  Also, if that window has a renderer associated with it, then that renderer
  must have the `SDL_RENDERER_TARGETTEXTURE` flag.
  
  Note that when changing this setting from being `nullptr` then 'Is SDL
  initialized' **will** at the same time be changed to `true` (since you would
  not have been able to create a valid `SDL_window` unless you had already set
  up SDL). Also, you will have to handle closing that window yourself.

  If set to `nullptr` then the `create()` method will create the window with
  the following settings and set it as being owned by this object (meaning the
  destructor will be in charge of destroying it).
  \
  Note that although those settings can be changed later, usually it should be
  better to just let the library handle them (which would happen e.g. when the
  user drags or resizes the window):
  - title: `Retro Monochrome Text Monitor` _(can be changed later)_
  - width: `1280` _(can be changed later)_
  - height: `720` _(can be changed later)_
  - position: `undefined` _(can be changed later)_
  - resizable: `true` _(can be changed later)_
  - fullscreen: `false` _(can be changed later)_

  No mater if _the window_ is set to `nullptr` or not, the following
  properties tied to the window will be set with the following being the
  default:
  - Key to switch between fullscreen and windowed mode: `F11` _(can be changed
    later)_
  - Key combination to close the window: `Ctrl-w` _(can be changed later)_
  - Key combination to quit the application: `Ctrl-q` _(can be changed later)_
  - Closing window is the same as quitting: `true` _(can be changed
    later)_
    \
    Not recommended to change that setting to `false` unless you either really
    want the application to continue running without a window or you have
    taken care of opening another window yourself.
  - Function to call before closing window: `nullptr` _(can be changed later)_
    \
    If set, that function can be used to e.g. offer the user the option to
    cancel that action or to save before continuing. That function must return
    a `bool`. Returning `false` will cancel closing the window.

  Finally, if _the window_ was set to `nullptr` **OR** if it is pointing to a
  `SDL_Window` that doesn't have a renderer then a `SDL_Renderer` will be
  created (either tied to that newly created window or to the one pointed to)
  and that renderer set as being owned by this object (meaning the destructor
  will be in charge of destroying it).
- Background texture: `nullptr`

  If not set to `nullptr` then it must point to an `SDL_Texture` containing
  the desired background image.

  If set to `nullptr` then the background texture will be loaded from the
  following file and set as being owned by this object (meaning the destructor
  will be in charge of destroying it):
  - Background file path: `res/img/terminal_screen.png`
    (or `res\img\terminal_screen.png` depending on the operating system).

    When changing this setting, you could use the constant
    `remo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
    operating system) for the path separator when creating the path, if you
    care about it being cross-platform.

    If this setting is set to a string starting with the path separator (or a
    drive letter, e.g. "D:", if on Windows) then the path is an absolute path.

    Otherwise the path is relative to your executable program file.

  No mater if set to `nullptr` or not, the following settings will be used
  (Note that x and y are counted from the top-left corner, both starting at
  zero):
  - minimum area to be shown:
    `x: 118, y: 95, width: 700, height: 540`
    \
    This setting allows for the background texture to include borders around
    the main area. That border can then either be shown to the sides of that
    area (if the ratio of the window is wider) or above and below that area
    (if the ratio of the window is narrower).
  - the area of the background where the text is to be drawn:
    `x: 188.25f, y: 149.25f, w: 560.0f, h: 432.0f`
- Font-bitmap texture: `nullptr`

  If not set to `nullptr` then it must point to an `SDL_Texture` containing
  the desired font-bitmap image.

  If set to `nullptr` then the font-bitmap texture will be loaded from the
  following file and set as being owned by this object (meaning the destructor
  will be in charge of destroying it):
  - Font-bitmap path: `res/img/font_bitmap.png`
    (or `res\img\font_bitmap.png` depending on the operating system).

    When changing this setting, you could use the constant
    `remo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
    operating system) for the path separator when creating the path, if you
    care about it being cross-platform.

    If this setting is set to a string starting with the path separator (or a
    drive letter, e.g. "D:", if on Windows) then the path is an absolute path.

    Otherwise the path is relative to your executable program file.

  No mater if set to `nullptr` or not, the following settings will be used:
  - Font size: `width: 7, height: 18`
    \
    This is the size, in pixels, of each character as they are in the
    font-bitmap file.
- A texture where the text will be drawn is created with the following settings:
  - text area size (in characters):
    `columns: 40, lines: 24` _(can be changed later)_
    \
    The size of the texture, in pixels, is calculated from the font size and
    the text area size in characters (The texture will then be scaled to fit
    the area of the background where the text is to be drawn).
  - blend mode: `remo::BlendMode::add` _(can be changed later)_
  - color mod: `red: 89, green: 221, blue: 0` _(can be changed later)_
    \
    Note that this setting controls the color of **all** the text, both the
    text that has already been printed to the screen and the text that is
    going to be printed to the screen.
- The following properties control the behaviour of the text output and will
  have the initial values shown here _(those can all be changed later)_:
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

ReMoTeMo follows the philosophy that it handles cleaning up those resources
that it did set up but leaves to you the responsibility of cleaning up those
resources that you passed to it.

For most uses, allowing the destructor to handle cleaning up those resources
when the `remo::Temo` object gets destroyed (whether manually or automatically
e.g. when going out of scope).

```C++
remo::Temo::~Temo();
```

The destructor, in charge of deleting/closing textures, the renderer, the
window and quitting SDL (unless the `remo::Temo` object was configured to
not handle some of those, e.g. if you wanted to create the window yourself so
it can display some menu before and after having ReMoTeMo taking control of
the window).

If, for some reason, you want to clean up sooner, you can call:

```C++
void remo::Temo::quit();
```

Just make sure not to touch any of the object's methods after that.

<sup>[Back to top](#remotemo-api-design)</sup>
## Using ReMoTeMo
### Interact with the monitor

```C++
struct remo::Point {int x; int y;};

remo::Temo &remo::Temo::moveCursor(int x, int y);
remo::Temo &remo::Temo::moveCursor(const remo::Point &move);
remo::Temo &remo::Temo::setCursor(int column, int line);
remo::Temo &remo::Temo::setCursor(const remo::Point &position);
remo::Temo &remo::Temo::setCursorColumn(int column);
remo::Temo &remo::Temo::setCursorLine(int line);
remo::Temo &remo::Temo::setCursorX(int column);
remo::Temo &remo::Temo::setCursorY(int line);
remo::Point remo::Temo::getCursorPosition();
remo::Temo &remo::Temo::pause(int pause);
remo::Temo &remo::Temo::clear();
char remo::Temo::getKey();
std::string remo::Temo::getInput(int maxLength);
remo::Temo &remo::Temo::print(conts std::string &text);
remo::Temo &remo::Temo::printAt(int column, int line,
        const std::string &text);
remo::Temo &remo::Temo::printAt(const remo::Point &position,
        const std::string &text);
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change text output behaviour

```C++
enum class remo::Wrapping {off, char, word};

remo::Temo &remo::Temo::setTextDelay(int delay);
remo::Temo &remo::Temo::setTextSpeed(int speed);
remo::Temo &remo::Temo::setScrolling(bool scrolling);
remo::Temo &remo::Temo::setWrapping(remo::Wrapping wrapping);
remo::Temo &remo::Temo::setInverse(bool inverse);

int remo::Temo::getTextDelay();
int remo::Temo::getTextSpeed();
bool remo::Temo::getScrolling();
remo::Wrapping remo::Temo::getWrapping();
bool remo::Temo::getInverse();
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change text area settings

```C++
enum class remo::BlendMode {none, blend, add, mod};
struct remo::Color {int red; int green; int blue;};

remo::Temo &remo::Temo::setTextAreaSize(int columns, int lines);
remo::Temo &remo::Temo::setTextAreaSize(const remo::Point &size);
int remo::Temo::getTextAreaColumns();
int remo::Temo::getTextAreaLines();
remo::Point remo::Temo::getTextAreaSize();
remo::Temo &remo::Temo::setTextBlendMode(remo::BlendMode mode);
remo::BlendMode remo::Temo::getTextBlendMode();
remo::Temo &remo::Temo::setTextColor(int red, int green, int blue);
remo::Temo &remo::Temo::setTextColor(const remo::Color &color);
remo::Color remo::Temo::getTextColor();
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change window settings

```C++
enum class remo::FKey {
        none, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12};
enum class remo::ModKeys {
        none = 0x0000, shift = 0x0001, ctrl = 0x0002, alt = 0x0004};
remo::Temo &remo::Temo::setWindowTitle(const std::string &title);
std::string remo::Temo::getWindowTitle();
remo::Temo &remo::Temo::setWindowSize(int width, int height);
remo::Temo &remo::Temo::setWindowSize(const remo::Point &size);
remo::Point remo::Temo::getWindowSize();
remo::Temo &remo::Temo::setWindowPosition(int x, int y);
remo::Temo &remo::Temo::setWindowPosition(const remo::Point &position);
remo::Point remo::Temo::getWindowPosition();
remo::Temo &remo::Temo::setWindowResizable(bool resizable);
bool remo::Temo::getWindowResizable();
remo::Temo &remo::Temo::setWindowFullscreen(bool fullscreen);
bool remo::Temo::getWindowFullscreen();
remo::Temo &remo::Temo::setKeyFullscreen(remo::ModKeys modKey, remo::FKey key);
remo::Temo &remo::Temo::setKeyFullscreen(remo::ModKeys modKey, char key);
std::tuple<remo::ModKey, remo::FKey, char> remo::Temo::getKeyFullscreen();
remo::Temo &remo::Temo::setKeyCloseWindow(remo::ModKeys modKey, remo::FKey key);
remo::Temo &remo::Temo::setKeyCloseWindow(remo::ModKeys modKey, char key);
std::tuple<remo::ModKey, remo::FKey, char> remo::Temo::getKeyCloseWindow();
remo::Temo &remo::Temo::setKeyQuit(remo::ModKeys modKey, remo::FKey key);
remo::Temo &remo::Temo::setKeyQuit(remo::ModKeys modKey, char key);
std::tuple<remo::ModKey, remo::FKey, char> remo::Temo::getKeyQuit();
remo::Temo &remo::Temo::setClosingSameAsQuit(bool closingSameAsQuit);
bool remo::Temo::getClosingSameAsQuit();
remo::Temo &remo::Temo::setFunctionPreQuit(std::function<bool()> func);
remo::Temo &remo::Temo::setFunctionPreQuit(std::function<bool(
        remo::Temo*)> func);
```

  <!-- TODO continue working on text from here -->
<sup>[Back to top](#remotemo-api-design)</sup>

