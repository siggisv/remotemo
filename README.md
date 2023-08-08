# remoTemo
Retro Monochrome Text Monitor - a library for building applications with a
really old-school looking user interface.

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)][mit]
[![GitHub Super-Linter](https://github.com/siggisv/remotemo/actions/workflows/super-linter.yml/badge.svg?branch=main)](https://github.com/siggisv/remotemo/actions/workflows/super-linter.yml)
[![Build](https://github.com/siggisv/remotemo/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/siggisv/remotemo/actions/workflows/cmake.yml)

<sup>Media files:</sup>
[![Media license: CC BY-SA 4.0][cc-by-sa-shield]][cc-by-sa]

By really old-school, we mean late 70s - beginning of the 80s. A monochrome,
fixed size, text-only monitor with keyboard-only user input (and restricted to
ASCII only, not even extended ASCII).

By default the text gets displayed at a slow rate to recreate the feeling of
being on a dumb terminal, connected over a slow connection to a mainframe:

[remotemo_simple_sample.webm](https://github.com/siggisv/remotemo/assets/5704941/bf79a37e-5e7b-4056-abe0-69efb158f183)

## WARNING
This is still work-in-progress.

Although those parts that are needed to run the following sample have been
implemented and are working, anything might get changed at any time.

## Licenses

The library itself is distributed under [the MIT license][mit] while any media
(e.g. the content of `res/img/`) is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

## Building from source

In short, install these dependencies:

- `git` - has been tested with 2.25 but any version that works with GitHub is
  fine.
- A C++ compiler that supports `C++17`
- `CMake` version 3.16 or newer
- `SDL` - has been tested with version 2.0.10. Older versions of SDL2 might
  work. Newer versions (at least up to 3.0) should work.
- `SDL_image` - has been tested with 2.0.5. Older versions for SDL2 might
  work. Newer versions (at least up to 3.0) should work.

Then use `git` to clone this repository (`--depth 1` can be skipped if you do
want the whole commit history instead of just the latest commit in the main
branch):

```sh
git clone --depth 1 https://github.com/siggisv/remotemo.git
cd remotemo
git submodule update --init cmake/sdl2
```

Now use `CMake` to generate the project and build:

```sh
cmake -S . -B release -DCMAKE_BUILD_TYPE=Release
cmake --build release --config Release
```

Finally make sure your own project can find the header files (in
`include/remotemo`) and the binary file (in the `release` directory:
`remotemo.a` or `remotemo.lib`, depending on operating system).

**Note:**
Unless you want to use your own resources for the background image and
font-bitmap, you would want to copy the `res` folder to your own project.

[Further instructions](docs/compiling.md)

## Quick start

The easiest way to start using remoTemo is to simply call
`remotemo::create()`, check its return status and then you can start
calling the text-input/output methods of the object created.

```cpp
#include <remotemo/remotemo.hpp>

int main(int argc, char* argv[])
{
    auto text_monitor = remotemo::create();
    if (!text_monitor) {
        return -1;

    }

    text_monitor->pause(1500);
    text_monitor->set_cursor(10, 2);
    text_monitor->print("Hello world!!!");

    text_monitor->pause(2500);

    text_monitor->set_inverse(true);
    text_monitor->print_at(5, 15, "Press any key to quit");
    text_monitor->set_inverse(false);
    text_monitor->print("\n\n\n");

    while (text_monitor->get_key() == remotemo::Key::K_n) {
        text_monitor->print("Actually no! Any key except that key :P\n");
    }
}
```

For further instruction check out [the API design](docs/API_design.md).

[mit]: https://opensource.org/licenses/MIT
[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg
