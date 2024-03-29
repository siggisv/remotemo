# Compiling remoTemo

To get the source code you either need to have `git` installed (any version
that works with GitHub) and use it to clone this repository (`--depth 1` can
be skipped if you do want the whole commit history instead of just the latest
commit in the main branch):

```sh
git clone --depth 1 https://github.com/siggisv/remotemo.git
cd remotemo # Note all further instructions assume you are in this directory
```

or [download as
ZIP](https://github.com/siggisv/remotemo/archive/refs/heads/main.zip)
(submodules probably not included).

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
- The submodule `gitlab.com/aminosbh/sdl2-cmake-modules` in the `cmake/sdl2`
  directory is needed for most versions of SDL2 to correcly find it.
  To only get that submodule:
  ```sh
  git submodule update --init cmake/sdl2
  ```
> **Note:**
> If using `Conan` to install `SDL2` (and `SDL2_image`) on Windows, running
> the following from within this project's directory should do it:
>
> ```sh
> conan install . --output-folder=build --build=missing
> ```
>
> Optionally add the following flags to that command:
>
> ```sh
> # IF also building the tests, they NEED the shared library version:
> --options=sdl*:shared=True
> # If running the tests from Powershell, add this flag also:
> -c tools.env.virtualenv:powershell=True
> # IF building a Debug version, instead of Release, then the dependencies
> # must be the same:
> -s build_type=Debug
> ```

### Setup and build

Use `CMake` to generate the project:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

To setup the project for an IDE of your choice, add the appropriate flags.
E.g. `-G "Visual Studio 15 2017" -A x64`. See the [CMake
docs](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#id13)
to find how to specify your IDE and what target platform names can be
specified with the `-A` flag for that IDE.

> **Note:**
> If you've installed `SDL2` in a non-standard directory, you might have to
> follow the instructions of [the CMake modules we use to find
> SDL2](https://gitlab.com/aminosbh/sdl2-cmake-modules#special-customization-variables)
> on how to specify a different path to search.
>
> If you did follow the instructions on how to install with `Conan`, then
> you need instead to add `-DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake`
> to the `cmake` command.

If you want to also build the samples, specify `-DREMOTEMO_BUILD_SAMPLES=ON`
(or change that setting using `ccmake` or the CMake GUI).

Build from command line:

```sh
cmake --build build --config Release
```

(or build using your IDE of choice).

Finally make sure your own project:
- can find the header files (in `include/remotemo`) and the binary file (in
  the `build` directory: `remotemo.a` or `remotemo.lib`, depending on
  operating system).
- can find the header files for `SDL2` and that the source file containing
  your `main()` function includes `SDL.h` (either directly or indirectly
  through including `remotemo/remotemo.hpp`).
- links correctly to `SDL2_main`. If using `CMake`, link your target to
  `SDL2::SDL2main` or `SDL2::Main`, depending on which one can be found. If
  your version of SDL2 does not provide either one of them as a target, you
  could e.g. use the same [CMake modules to find
  SDL2](https://github.com/aminosbh/sdl2-cmake-modules/) as this project uses.

**Note:**
- Unless you want to use your own resources for the background image and
  font-bitmap, you would want to copy the `res` folder to your own project. Do
  note the [license](../res/img/README.md) for the media files in that
  directory.
- When building your own project, the resource folder needs to be copied to
  the same directory as the resulting executable.

## For building the tests

### Dependencies of the tests

On top of what is needed for the building of the library itself, you also need
`Catch2` and `trompeloeil`. Both are provided as git submodules. To fetch
them, either:

- pass `--recurse-submodules` to the `git clone` command when cloning this
  repository.
- or run the following commands within the directory where you cloned this
  repository:
  ```sh
  git submodule update --init
  ```
If using `Conan`, do not forget to add the correct flags to get the needed
shared library version of `SDL2` and `SDL2_image` ([see note above on using
Conan](#dependencies)).

### Setup, build and run

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

# or you could run each one of the generated tests directly:

cd build/tests
# NOTE, some build systems, like Visual Studio, will build the executables in
# a subdirectory named 'Release' (or 'Debug'). In that case you need:
cd build/tests/Release
# instead of the above command.

test_init
test_io

# To only run those tests within that file that have the [print] tag:
test_io [print]
# See what more flags can be passed to the test:
test_io --help
```
> **NOTE**
> If SDL2 and SDL2_image were installed with conan, run the following
> first so that the shared libraries can be found:
>
> ```sh
> # If using CMD (the old DOS command prompt):
> build/conanrun.bat
> # or if using Powershell:
> ./build/conanrun.ps1
> ```


## For generating local `html` documentation of the public API

`doxygen` needs to be installed. Has been tested with version 1.8.17. Older
versions might work. Newer versions should work.

Generate the project in the same way as for the library itself (and optionally
the samples or/and tests also), except you also need to specify
`-DREMOTEMO_BUILD_DOCS=ON` (or change that setting using `ccmake` or the CMake
GUI).

Then generate the documentation (into `build/docs/html`) by running:
```sh
cmake --build build --target docs
```
