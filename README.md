<h3 align="center">Micro80Emu</h3>

---

<p align="center"> An emulator of the Soviet Micro-80 computer.
    <br>
    <b>English</b> | <a href="README.ru.md">Русский</a>
</p>

## Table of Contents
+ [Getting Started](#getting_started)
+ [Usage](#usage)
+ [Built Using](#built_using)
+ [Acknowledgements](#acknowledgement)

## Getting Started <a name = "getting_started"></a>

### Building from source

> The project requires CMake to build. It has been tested on Arch Linux; functionality on other Linux distributions and Windows is not guaranteed.

First, clone the repository:

```bash
git clone https://github.com/kasefuchs/micro80emu.git
cd micro80emu
```

Next, create a build directory and configure the project:

```bash
mkdir cmake-build/
cd cmake-build/
cmake ..
```

Finally, compile the sources:

```bash
cmake --build .
```

This will generate the `micro80emu` executable inside the `cmake-build/app` directory.

## Usage <a name = "usage"></a>

> To run the emulator you need a ROM image (e.g. monitor.rom) and a font image (e.g. font.rom).
> Without these files the emulator will not start correctly.

### CLI Options

| Option        | Description                       | Default       |
| ------------- | --------------------------------- | ------------- |
| `-h, --help`  | Display help                      | -             |
| `-m, --mem`   | Memory image file                 | -             |
| `-r, --rom`   | ROM image file                    | `monitor.rom` |
| `-f, --font`  | Font image file                   | `font.rom`    |
| `-d, --dump`  | Write memory dump to file on exit | -             |
| `-e, --entry` | Entry address for CPU reset (hex) | `0xF800`      |

## Built Using <a name = "built_using"></a>

- [C++11](https://isocpp.org/) - Core language
- [CMake](https://cmake.org/) - Build system
- [Raylib](https://www.raylib.com/) - Graphics library
- [cxxopts](https://github.com/jarro2783/cxxopts) - Command line options parser

## Acknowledgements <a name = "acknowledgement"></a>

- [@alemorf](https://github.com/alemorf) — for the [video series](https://youtube.com/playlist?list=PLVE4LTyHQzdhimVCuiFWpoHwgU9YShYs7) and [all his work](https://github.com/alemorf/retro_computers/tree/master/Micro_80) about Micro-80
