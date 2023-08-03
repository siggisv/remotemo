# Compiling remoTemo

To get the source code you either need to have `git` installed (any version
that works with GitHub) and use it to clone this repository (`--depth 1` can
be skipped if you do want the whole commit history instead of just the latest
commit in the main branch):

```sh
git clone --depth 1 https://github.com/siggisv/remotemo.git
```

or [download as
ZIP](https://github.com/siggisv/remotemo/archive/refs/heads/main.zip)
(submodules needed for the tests probably not included).

`CMake`, version 3.16 or newer, is needed both for the setup of the project
and for the setup of documentation generation.

(Actually you can skip CMake, but then you would need to set up the project
all by yourself. I.e. which files to compile, what flags to use, which
libraries to link to, etc.)

## For building just the library itself (and the samples)

### Dependencies

- A C++ compiler that supports `C++17`
- `SDL` - has been tested with version 2.0.10. Older versions of SDL2 might
  work. Newer versions (at least up to 3.0) should work.
- `SDL_image` - has been tested with 2.0.5. Older versions for SDL2 might
  work. Newer versions (at least up to 3.0) should work.

### Setup and build

Use `CMake` to generate the project:

```sh
cd remotemo
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

(To setup the project for an IDE of your choice, add the appropriate flags.
E.g. `-G "Visual Studio 15 2017" -A Win64`. See the [CMake
docs](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#id13)
)

**TODO**
- Change `CMakeLists.txt` to take into account that `SDL` and `SDL_image`
  might be installed in a non-default place.
- Add instructions here on how to set the path to those, if needed.

If you want to also build the samples, specify `-DREMOTEMO_BUILD_SAMPLES=ON`
(or change that setting using `ccmake` or the CMake GUI).

Build from command line:

```sh
cmake --build build --config Release
```

or build using your IDE of choice.

Finally make sure your own project can find the header files (in
`include/remotemo`) and the binary file (in the `build` directory:
`remotemo.a` or `remotemo.lib`, depending on operating system).

**Note**
Unless you want to use your own resources for the background image and
font-bitmap, you would want to copy the `res` folder to your own project.

## For building the tests

### Dependencies

On top of what is needed for the building of the library itself, you also need
`Catch2` and `trompeloeil`. Both are provided as git submodules. To fetch
them, either:

- pass `--recurse-submodules` to the `git clone` command when cloning this
  repository.
- or run the following commands within the directory where you cloned this
  repository:
  ```sh
  git submodule init
  git submodule update
  ```

### Setup and build

Generate the project in the same way as for the library itself (and optionally
the samples also), except you also need to specify `-DREMOTEMO_BUILD_TESTS=ON`
(or change that setting using `ccmake` or the CMake GUI).

Optionally, you can also specify `-DREMOTEMO_VERBOSE_TESTS=ON` if you want the
tests to be more verbose.

Then build in the same way as you would build the library itself.

Finally, to run the tests:
```sh
cd build/tests
ctest -C Release -VV
```

or you could run each one of the generated test directly. E.g.:
```sh
cd build/tests
test_io
# Only run those tests within that file that have the [print] tag:
test_io [print]
# See what more flags can be passed to the test:
test_io --help
```

## For generating local `html` documentation of the public API

### Dependencies

- `doxygen` - has been tested with version 1.8.17. Older versions might work.
  Newer versions should work.

### Setup and generate

Generate the project in the same way as for the library itself (and optionally
the samples or/and tests also), except you also need to specify
`-DREMOTEMO_BUILD_DOCS=ON` (or change that setting using `ccmake` or the CMake
GUI).

Then generate the documentation (into `build/docs/html`) by running:
```sh
cmake --build build --target docs
```
