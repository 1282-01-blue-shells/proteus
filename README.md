# proteus

A collection of C++ applications and libraries targeting the FEH Proteus for completing the 2023 course challenge.

## Build Prerequisites

Python 3 and the *arm-none-eabi* GCC toolchain must be installed. Doxygen is required for building documentation but is otherwise optional.

This repository contains Git submodules that must be cloned before building. The recommended method of cloning Git submodules is to pass `--recurse-submodules` to `git clone` when cloning this repository for the first time:

```sh
git clone --recurse-submodules https://github.com/1282-01-blue-shells/proteus
```

Alternatively, submodules may be installed *ex post facto* like so:

```sh
git submodule init
git submodule update
```

## Building

To build all applications, run GNU Make without any arguments. If the repository is opened in VS Code, the <kbd>Ctrl+Shift+B</kbd> shortcut will invoke Make for you in parallel mode. On Windows, `mingw32-make` is used instead of `make`.

Once built, a particular application may be installed to an SD card with `python3 deploy.py <app-name>` where `<app-name>` is the name of the application. See [*Makefile*](./Makefile) and [*deploy.py*](./deploy.py) for additional usage information.

## Project Structure

- *Apps*: contains a subdirectory for each Proteus application.
- *Libs*: contains headers and implementation files for libraries, which are common code across applications.
- *Vendor*: contains external dependencies vendored as Git submodules. Currently, this directory only contains the Proteus firmware repository.
- *Build*: contains all build products, including object files and linked executables. This directory is generated when `make` is invoked and is removed after a `make clean`.

## Credits

- AK Miedler <address>miedler.6@buckeyemail.osu.edu</address>
- Reina Quinn <address>quinn.548@buckeyemail.osu.edu</address>
- Sariah Echols <address>echols.65@buckeyemail.osu.edu</address>
- Will Blankemeyer <address>blankemeyer.33@buckeyemail.osu.edu</address>
