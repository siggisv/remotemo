# remoTemo
Retro Monochrome Text Monitor - a library for building applications with a
really old-school looking user interface.

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)
[![GitHub Super-Linter](https://github.com/siggisv/remotemo/actions/workflows/super-linter.yml/badge.svg?branch=main)](https://github.com/siggisv/remotemo/actions/workflows/super-linter.yml)
[![Build](https://github.com/siggisv/remotemo/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/siggisv/remotemo/actions/workflows/cmake.yml)

By really old-school, we mean late 70s - beginning of the 80s. A monochrome, fixed size,
text-only monitor with keyboard-only user input (and restricted to ASCII only, not even extended ASCII).

Optionally the text gets displayed at a slow rate to recreate the feeling of being connected over a slow connection.

[remotemo_simple_sample.webm](https://github.com/siggisv/remotemo/assets/5704941/bf79a37e-5e7b-4056-abe0-69efb158f183)

## WARNING
This is still work-in-progress.

Although those parts that are needed to run the following sample have been
implemented and are working, anything might get changed at any time.

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
