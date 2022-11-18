# ReMoTeMo API design - 1st draft

## Warning! Work in progress.

**Both** the design and the implementation are work-in-progress.

## Creation, initialization and cleanup

```C++
remotemo::Temo::Temo()
```
The default constructor.

Creates a window showing a text monitor and then returns an object
representing that window and that text monitor.

Creates and initializes everything with the default settings. Unless noted,
those can not be changed after the object has been created:
- Initializes SDL and sets it as being handled by this object (meaning that
  the destructor will be in charge of calling `SDL_Quit();`).
- Creates a `SDL_window` with the following settings.
  Note that although most of those settings can be changed later, usually it
  should be better to just let the library handle them (which would happen
  whenever the user drags or resizes the window):
  - width: 1280 _(can be changed later)_
  - height: 720 _(can be changed later)_
  - position: undefined _(can be changed later)_
  - resizable: true _(can be changed later)_
  - fullscreen: false _(can be changed later)_
  - sets it as being owned by this object (meaning the destructor will be in
    charge of destroying it).
- Also sets the following properties tied to the window:
  - sets `F11` as being the key used to switch between fullscreen and windowed
    mode _(can be changed later)_
  - sets `Ctrl-w` as being the key combination used to close the window _(can
    be changed later)_
  - sets `Ctrl-q` as being the key combination used to quit the application
    _(can be changed later)_
  - sets closing window to be the same as quitting to `true` _(can be changed
    later)_
    \
    Not recommended to change that setting to `false` unless you either really
    want the application to continue running without a window or you have
    taken care of opening another window yourself.
  - sets function-to-call-before-closing-window to `nullptr` _(can and
    **should** be changed later)_
    \
    If set, that function can be used to e.g. offer the user the option to
    cancel that action or to save before continuing.
- Creates a `SDL_Renderer`
  - sets it as being owned by this object (meaning the destructor will be in
    charge of destroying it).
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
    Setting this on makes text to be displayed with the background color and
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




```C++
remotemo::Config::Config()
```

