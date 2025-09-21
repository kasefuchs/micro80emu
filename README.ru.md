<h3 align="center">Micro80Emu</h3>

---

<p align="center"> Эмулятор советского компьютера Микро-80.
    <br>
    <a href="README.md">English</a> | <b>Русский</b>
</p>

## Содержание

* [Начало работы](#getting_started)
* [Использование](#usage)
* [Используемые технологии](#built_using)
* [Благодарности](#acknowledgement)

## Начало работы <a name="getting_started"></a>

### Сборка из исходников

> Проект требует CMake для сборки. Проверено на Arch Linux; работа на других Linux-системах и Windows не гарантирована.

Сначала клонируем репозиторий:

```bash
git clone https://github.com/kasefuchs/micro80emu.git
cd micro80emu
```

Затем создаём папку для сборки и конфигурируем проект:

```bash
mkdir cmake-build/
cd cmake-build/
cmake ..
```

Наконец, компилируем исходники:

```bash
cmake --build .
```

В результате будет создан исполняемый файл `micro80emu` внутри папки `cmake-build/app`.

## Использование <a name="usage"></a>

> Для работы эмулятора необходимы **ROM-образ** (например, `monitor.rom`) и **образ ПЗУ знакогенератора** (например, `font.rom`).
> Без этих файлов эмулятор не будет работать корректно.

### Аргументы коммандной строки

| Опция         | Описание                         | По умолчанию  |
| ------------- | -------------------------------- | ------------- |
| `-h, --help`  | Показать справку                 | -             |
| `-m, --mem`   | Файл образа памяти               | -             |
| `-r, --rom`   | ROM-файл                         | `monitor.rom` |
| `-f, --font`  | Файл шрифтов                     | `font.rom`    |
| `-d, --dump`  | Сохранить дамп памяти при выходе | -             |
| `-e, --entry` | Адрес входа для сброса CPU (hex) | `0xF800`      |

## Используемые технологии <a name="built_using"></a>

- [C++11](https://isocpp.org/) — Основной язык
- [CMake](https://cmake.org/) — Система сборки
- [Raylib](https://www.raylib.com/) — Библиотека для графики
- [cxxopts](https://github.com/jarro2783/cxxopts) — Парсер аргументов командной строки

## Благодарности <a name="acknowledgement"></a>

- [@alemorf](https://github.com/alemorf) — за [цикл видео](https://youtube.com/playlist?list=PLVE4LTyHQzdhimVCuiFWpoHwgU9YShYs7) и [всю его работу](https://github.com/alemorf/retro_computers/tree/master/Micro_80) о Микро-80
