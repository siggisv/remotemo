# ReMoTeMo API design
_1st draft_

## Warning! Work in progress

**Both** the design and the implementation are work-in-progress.

## Table of contents
- [Initialization, settings and cleanup](#initialization-settings-and-cleanup)
  - [Initialization](#initialization)
  - [Default settings](#default-settings)
  - [Cleanup](#cleanup)


###### [Back to top](#remotemo-api-design)
## Initialization, settings and cleanup
***TODO?*** Make thread safe? Or just explain that just like most of the
underlying SDL2 framework, this object should only be used from within the
thread where it was initialized?

###### [Back to top](#remotemo-api-design)
### Initialization

```C++
std::pair<remotemo::Temo, bool> remotemo::Temo::create();
std::pair<remotemo::Temo, bool> remotemo::Temo::create(const remotemo::Config &config);
```

This is a static method that will create and initialize an object representing
a text monitor, the window showing it and their properties (along with the
needed properties of the underlying SDL-library).

- If successful, it will return a `std::pair<remotemo::Temo, bool>` containing
  the object and the boolean set to `true`. It will also create an internal
  list of the resources that it did set up so that whenever/however the
  `remotemo::Temo` object gets deleted, its destructor can take care of
  cleaning them up.
- If unsuccessful, it will log an error using `SDL_Log()`, free/release all
  resources it had succeeded setting up and then return a `std::pair`
  containing an uninitialized `remotemo::Temo` object and the boolean set to
  `false`.
\
\
***TODO?*** On failure return a `remotemo::NoopTemo` object, a subclass of
`remotemo::Temo`, where all methods are overwritten to do nothing except log
an error - just in case you didn't check the status of the returned boolean.

When invoked without any parameters, it will set up the underlying
SDL2-library and create a window for you with the default settings (including
loading a background texture and the font-bitmap from their default
locations). Most of the settings can not be changed after creation.

To initialize with other settings than the default ones, first create a
`remotemo::Config` object:

```C++
remotemo::Config::Config();

```

_The constructor that creates a `remotemo::Config` object._

When created, it will contains the default settings. You can then pick which
settings to change by calling its setters. Then call `remotemo::Temo::create(const
remotemo::Config &config)` to create and initialize everything with the
changed settings.

The setters can be chained, e.g.:

```C++
remotemo::Config myconfig;
myconfig.setWindowWidth(1920).setWindowHeight(1080)
        .setBackgroundFile("background.png")
        .setBackgroundMinArea(200, 120, 1000, 700);
std::pair<remotemo::Temo, bool> tM = remotemo::Temo::create(myconfig);
```

***TODO?*** List all the setters of the config class.

###### [Back to top](#remotemo-api-design)
### Default settings

The following are the default settings with a short explanation for each one.
Unless noted, they can not be changed after the `remotemo::Temo` object has
been created:
- Is SDL initialized: `false`

  If `false`, then SDL will be initialized when calling the `create()` method
  AND the destructor of the `remotemo::Temo` object will handle cleaning it up
  (meaning that the destructor will be in charge of calling `SDL_Quit();`).

  If `true`, then you must handle both setting up SDL before calling the
  `create()` method AND also cleaning up SDL afterwards.
- The window: `nullptr`

  If NOT set to `nullptr` then it must be a pointer to a valid `SDL_Window`.
  
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
  - width: 1280 _(can be changed later)_
  - height: 720 _(can be changed later)_
  - position: undefined _(can be changed later)_
  - resizable: true _(can be changed later)_
  - fullscreen: false _(can be changed later)_

  No mater if that is set to `nullptr` or not, the following properties tied
  to the window will be set with the following being the default:
  - set `F11` as being the key used to switch between fullscreen and windowed
    mode _(can be changed later)_
  - set `Ctrl-w` as being the key combination used to close the window _(can
    be changed later)_
  - set `Ctrl-q` as being the key combination used to quit the application
    _(can be changed later)_
  - set closing window to be the same as quitting to `true` _(can be changed
    later)_
    \
    Not recommended to change that setting to `false` unless you either really
    want the application to continue running without a window or you have
    taken care of opening another window yourself.
  - set function-to-call-before-closing-window to `nullptr` _(can and
    **should** be changed later)_
    \
    If set, that function can be used to e.g. offer the user the option to
    cancel that action or to save before continuing.

  Finally, if that was set to `nullptr` **OR** if it is pointing to a
  `SDL_Window` that doesn't have a renderer then a `SDL_Renderer` will be
  created (either tied to that newly created window or to the one pointed to)
  and that renderer set as being owned by this object (meaning the destructor
  will be in charge of destroying it).
  <!-- TODO continue working on text from here -->
- Sets up the background texture with the following settings (Note that x and
  y are counted from the top-left corner, both starting at zero):
  - load from the default file (`res/img/terminal_screen.png`)
  - minimum area to be shown:
    [x: 118,
    y: 95,
    width: 700,
    height: 540]
    \
    This setting allows for the background texture to include borders around
    the main area. That border can then either be shown to the sides of that
    area (if the ratio of the window is wider) or above and below that area
    (if the ratio of the window is narrower).
  - the area of the background where the text is to be drawn:
    [x: 188.25f,
    y: 149.25f,
    w: 560.0f,
    h: 432.0f]
  - sets it as being owned by this object (meaning the destructor will be in
    charge of destroying it).
- Sets up the font-bitmap texture with the following settings:
  - load from the default file (`res/img/font_bitmap.png`)
  - sets it as being owned by this object (meaning the destructor will be in
    charge of destroying it).
- Creates a texture to contain the text to be drawn over the background:
  - text area size in characters:
    [columns: 40,
    lines: 24] _(can be changed later)_
  - size, in pixels, is calculated from the font size and the size in
    characters.
  - blend mode: ADD _(can be changed later)_
  - color mod: [red: 89, green: 221, blue:0] _(can be changed later)_
    \
    Note that this setting controls the color of **all** the text, both the
    text that has already been printed to the screen and the text that is
    going to be printed to the screen.
  - sets it as being owned by this object (meaning the destructor will be in
    charge of destroying it).
- Gives the following properties an initial value _(those can all be changed
    later)_:
  - delay between chars: 60 (milliseconds)
    \
    Whenever text is printed to the monitor, it gets displayed one character
    at the time with a slight delay between.
  - inverse: off
    \
    Set this on for text to be displayed with the background color and
    the foreground color switched.
    \
    Changing this doesn't affect text that has already been printed to the
    screen.
  - cursor position, counted from the top-left corner:
    [column: 0,
    line: 0]
  - scrolling of text: on
    \
    If set on, the content of the screen scrolls up one line when the cursor
    goes below the bottom line.
  - text wrapping: off
    \
    This can be set to `off` (text printed beyond the right border gets lost),
    `char` (text wraps to next line. This might happen in the middle of a
    word) or `word` (text wraps to the next line, if possible at the last
    whitespace before getting to the right border).

###### [Back to top](#remotemo-api-design)
### Cleanup
```C++
remotemo::Temo::~Temo()
```

The destructor, in charge of deleting/closing textures, the renderer, the
window and quitting SDL (unless the `remotemo::Temo` object was configured to
not handle some of those, e.g. if you want to create the window yourself so it
can display some menu before and after having ReMoTeMo taking control of the
window).

###### [Back to top](#remotemo-api-design)
