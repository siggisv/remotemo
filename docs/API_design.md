# remoTemo API design
_1st draft_

## Warning! Work in progress

**Both** the design and the implementation are work-in-progress.

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
**TODO?** Make thread safe? Or just explain that just like most of the
underlying SDL2 framework, this object should only be used from within the
thread where it was initialized?

<sup>[Back to top](#remotemo-api-design)</sup>
### Initialization

```C++
std::pair<remoTemo::Temo, bool> remoTemo::create();
std::pair<remoTemo::Temo, bool> remoTemo::create(
        const remoTemo::InitConfig &config);
```

This is a static method that will create and initialize an object representing
a text monitor, the window showing it and their properties (along with the
needed properties of the underlying SDL2-library).

- If successful, it will return a `std::pair<remoTemo::Temo, bool>` containing
  the object and the boolean set to `true`. It will also create an internal
  list of the resources that it did set up so that whenever/however the
  `remoTemo::Temo` object gets deleted, its destructor can take care of cleaning
  them up.
- If unsuccessful, it will log an error using `SDL_Log()`, free/release all
  resources it had succeeded setting up and then return a `std::pair`
  containing an uninitialized `remoTemo::Temo` object and the boolean set to
  `false`.
\
\
**TODO?** On failure return a `remoTemo::Noop` object, a subclass of
`remoTemo::Temo`, where all methods are overwritten to do nothing except log
an error - just in case you didn't check the status of the returned boolean.

When invoked without any parameters, it will set up the underlying
SDL2-library and create a window for you with the default settings (including
loading a background texture and the font-bitmap from their default
locations). Most of the settings can not be changed after creation.

To initialize with other settings than the default ones, first create a
`remoTemo::InitConfig` object:

```C++
remoTemo::InitConfig::InitConfig();

```

_The constructor that creates a `remoTemo::InitConfig` object._

When created, it will contains the default initialization settings. You can
then pick which settings to change by calling its setters. Then call
`remoTemo::create(const remoTemo::InitConfig &config)` to create and initialize
everything with the changed settings.

~~The setters can be chained, e.g.:~~ Nope! At least some (if not all) of them
need to be able to gracefully report back that an error occurred.

```C++
remoTemo::InitConfig myconfig;
if (myconfig.setWindowSize(1920, 1080) == false) { /* handle error */ }
if (myconfig.setBackgroundFile("background.png") == false) {
    /* handle error */
}
if (myconfig.setBackgroundMinArea(200, 120, 1000, 700) == false) {
    /* handle error */
}
std::pair<remoTemo::Temo, bool> tM = remoTemo::create(myconfig);
```

```C++
int remoTemo::InitConfig::setIsSDLinitialized(bool SDLisInitialized);
int remoTemo::InitConfig::setTheWindow(const SDL_Window const *window);
int remoTemo::InitConfig::setWindowTitle(const std::string &title);
int remoTemo::InitConfig::setWindowSize(int width, int height);
int remoTemo::InitConfig::setWindowSize(const remoTemo::Point &size);
int remoTemo::InitConfig::setWindowPosition(int x, int y);
int remoTemo::InitConfig::setWindowPosition(const remoTemo::Point &position);
int remoTemo::InitConfig::setWindowResizable(bool resizable);
int remoTemo::InitConfig::setWindowFullscreen(bool fullscreen);
int remoTemo::InitConfig::setKeyFullscreen(remoTemo::ModKeys modKey,
        remoTemo::FKey key);
int remoTemo::InitConfig::setKeyFullscreen(remoTemo::ModKeys modKey, char key);
int remoTemo::InitConfig::setKeyCloseWindow(remoTemo::ModKeys modKey,
        remoTemo::FKey key);
int remoTemo::InitConfig::setKeyCloseWindow(remoTemo::ModKeys modKey, char key);
int remoTemo::InitConfig::setKeyQuit(remoTemo::ModKeys modKey,
        remoTemo::FKey key);
int remoTemo::InitConfig::setKeyQuit(remoTemo::ModKeys modKey, char key);
int remoTemo::InitConfig::setClosingSameAsQuit(bool closingSameAsQuit);
int remoTemo::InitConfig::setFunctionPreQuit(std::function<bool()> func);
int remoTemo::InitConfig::setFunctionPreQuit(
        std::function<bool(remoTemo::Temo*)> func);
int remoTemo::InitConfig::setBackground(const SDL_Texture const *background);
int remoTemo::InitConfig::setBackgroundFilePath(const std::string &filePath);
int remoTemo::InitConfig::setBackgroundMinArea(int x, int y, int w, int h);
int remoTemo::InitConfig::setBackgroundTextArea(float x, float y, float width,
        float height);
int remoTemo::InitConfig::setFontBitmap(const SDL_Texture const *fontBitmap);
int remoTemo::InitConfig::setFontBitmapFilePath(const std::string &filePath);
int remoTemo::InitConfig::setFontSize(int width, int height);
int remoTemo::InitConfig::setFontSize(const remoTemo::Point &size);
int remoTemo::InitConfig::setTextAreaSize(int columns, int lines);
int remoTemo::InitConfig::setTextAreaSize(const remoTemo::Point &size);
int remoTemo::InitConfig::setTextBlendMode(remoTemo::BlendMode mode);
int remoTemo::InitConfig::setTextColor(int red, int green, int blue);
int remoTemo::InitConfig::setTextColor(const remoTemo::Color &color);
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Default settings

The following are the default settings with a short explanation for each one.
Unless noted, they can not be changed after the `remoTemo::Temo` object has been
created:
- Is SDL initialized: `false`

  If `false`, then SDL will be initialized when calling the `create()` method
  AND the destructor of the `remoTemo::Temo` object will handle cleaning it up
  (meaning that the destructor will be in charge of calling `SDL_Quit();`).

  If `true`, then you must handle both setting up SDL before calling the
  `create()` method AND also cleaning up SDL afterwards.
- The window: `nullptr`

  If NOT set to `nullptr` then it must be a pointer to a valid `SDL_Window`.
  Note that it will then be your responsability to both keep that pointed window
  valid until you quit remoTemo and to clean up after use.
  \
  Also, if that window has a renderer associated with it, then that renderer
  must have the `SDL_RENDERER_TARGETTEXTURE` flag set.
  
  Note that when changing this setting from being `nullptr` then 'Is SDL
  initialized' **will** at the same time be changed to `true` (since you would
  not have been able to create a valid `SDL_window` unless you had already set
  up SDL). Also, you will have to handle closing that window yourself.

  If set to `nullptr` then the `create()` method will create the window with
  the following settings and set it as being owned by this object (meaning the
  destructor will be in charge of destroying it).
  \
  Note that although those settings can be changed later, usually it should be
  better to just let the library handle that (which would happen e.g. when the
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
  the desired background image.  Note that it will then be your responsability
  to both keep that pointed texture valid until you quit remoTemo and to clean
  up after use.

  If set to `nullptr` then the background texture will be loaded from the
  following file and set as being owned by this object (meaning the destructor
  will be in charge of destroying it):
  - Background file path: `res/img/terminal_screen.png`
    (or `res\img\terminal_screen.png` depending on the operating system).

    When changing this setting, you could use the constant
    `remoTemo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
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
  the desired font-bitmap image.  Note that it will then be your
  responsability to both keep that pointed texture valid until you quit
  remoTemo and to clean up after use.

  If set to `nullptr` then the font-bitmap texture will be loaded from the
  following file and set as being owned by this object (meaning the destructor
  will be in charge of destroying it):
  - Font-bitmap path: `res/img/font_bitmap.png`
    (or `res\img\font_bitmap.png` depending on the operating system).

    When changing this setting, you could use the constant
    `remoTemo::PATH_SEP` (which is set to `'\'` or `'/'` depending on the
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
  - blend mode: `remoTemo::BlendMode::add` _(can be changed later)_
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

Just make sure not to touch any of the object's methods after that.

<sup>[Back to top](#remotemo-api-design)</sup>
## Using remoTemo
### Interact with the monitor

```C++
struct remoTemo::Point {int x; int y;};

int remoTemo::Temo::moveCursor(int x, int y);
int remoTemo::Temo::moveCursor(const remoTemo::Point &move);
int remoTemo::Temo::setCursor(int column, int line);
int remoTemo::Temo::setCursor(const remoTemo::Point &position);
int remoTemo::Temo::setCursorColumn(int column);
int remoTemo::Temo::setCursorLine(int line);
int remoTemo::Temo::setCursorX(int column);
int remoTemo::Temo::setCursorY(int line);
remoTemo::Point remoTemo::Temo::getCursorPosition();

int remoTemo::Temo::pause(int pause);
int remoTemo::Temo::clear();
char remoTemo::Temo::getKey();
std::string remoTemo::Temo::getInput(int maxLength);

int remoTemo::Temo::print(conts std::string &text);
int remoTemo::Temo::printAt(int column, int line, const std::string &text);
int remoTemo::Temo::printAt(const remoTemo::Point &position,
        const std::string &text);
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change text output behaviour

```C++
enum class remoTemo::Wrapping {off, char, word};

int remoTemo::Temo::setTextDelay(int delay);
int remoTemo::Temo::setTextSpeed(int speed);
int remoTemo::Temo::setScrolling(bool scrolling);
int remoTemo::Temo::setWrapping(remoTemo::Wrapping wrapping);
int remoTemo::Temo::setInverse(bool inverse);

int remoTemo::Temo::getTextDelay();
int remoTemo::Temo::getTextSpeed();
bool remoTemo::Temo::getScrolling();
remoTemo::Wrapping remoTemo::Temo::getWrapping();
bool remoTemo::Temo::getInverse();
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change text area settings

```C++
enum class remoTemo::BlendMode {none, blend, add, mod};
struct remoTemo::Color {int red; int green; int blue;};

int remoTemo::Temo::setTextAreaSize(int columns, int lines);
int remoTemo::Temo::setTextAreaSize(const remoTemo::Point &size);
int remoTemo::Temo::getTextAreaColumns();
int remoTemo::Temo::getTextAreaLines();
remoTemo::Point remoTemo::Temo::getTextAreaSize();

int remoTemo::Temo::setTextBlendMode(remoTemo::BlendMode mode);
remoTemo::BlendMode remoTemo::Temo::getTextBlendMode();

int remoTemo::Temo::setTextColor(int red, int green, int blue);
int remoTemo::Temo::setTextColor(const remoTemo::Color &color);
remoTemo::Color remoTemo::Temo::getTextColor();
```

<sup>[Back to top](#remotemo-api-design)</sup>
### Change window settings

```C++
enum class remoTemo::FKey {
        none, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12};
enum class remoTemo::ModKeys {
        none = 0x0000, shift = 0x0001, ctrl = 0x0002, alt = 0x0004};

int remoTemo::Temo::setWindowTitle(const std::string &title);
std::string remoTemo::Temo::getWindowTitle();
int remoTemo::Temo::setWindowSize(int width, int height);
int remoTemo::Temo::setWindowSize(const remoTemo::Point &size);
remoTemo::Point remoTemo::Temo::getWindowSize();
int remoTemo::Temo::setWindowPosition(int x, int y);
int remoTemo::Temo::setWindowPosition(const remoTemo::Point &position);
remoTemo::Point remoTemo::Temo::getWindowPosition();
int remoTemo::Temo::setWindowResizable(bool resizable);
bool remoTemo::Temo::getWindowResizable();
int remoTemo::Temo::setWindowFullscreen(bool fullscreen);
bool remoTemo::Temo::getWindowFullscreen();

int remoTemo::Temo::setKeyFullscreen(remoTemo::ModKeys modKey,
        remoTemo::FKey key);
int remoTemo::Temo::setKeyFullscreen(remoTemo::ModKeys modKey, char key);
std::tuple<remoTemo::ModKey, remoTemo::FKey, char>
        remoTemo::Temo::getKeyFullscreen();
int remoTemo::Temo::setKeyCloseWindow(remoTemo::ModKeys modKey,
        remoTemo::FKey key);
int remoTemo::Temo::setKeyCloseWindow(remoTemo::ModKeys modKey, char key);
std::tuple<remoTemo::ModKey, remoTemo::FKey, char>
        remoTemo::Temo::getKeyCloseWindow();
int remoTemo::Temo::setKeyQuit(remoTemo::ModKeys modKey, remoTemo::FKey key);
int remoTemo::Temo::setKeyQuit(remoTemo::ModKeys modKey, char key);
std::tuple<remoTemo::ModKey, remoTemo::FKey, char> remoTemo::Temo::getKeyQuit();

int remoTemo::Temo::setClosingSameAsQuit(bool closingSameAsQuit);
bool remoTemo::Temo::getClosingSameAsQuit();
int remoTemo::Temo::setFunctionPreQuit(std::function<bool()> func);
int remoTemo::Temo::setFunctionPreQuit(
        std::function<bool(remoTemo::Temo*)> func);
```

<sup>[Back to top](#remotemo-api-design)</sup>

