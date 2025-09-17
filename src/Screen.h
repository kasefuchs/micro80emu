#pragma once
#include <functional>
#include <raylib.h>

#include "Types.h"

class Screen {
public:
    Screen(Core::ReadMemory rm, Core::ReadMemory rf);

    ~Screen();

    static Rectangle GetTextureBounds();

    void initialize();

    void updateTexture() const;

    Texture2D getTexture() const;

private:
    static constexpr Core::address CHAR_CODE_OFFSET = 0xE800;
    static constexpr Core::address ATTRIBUTE_OFFSET = 0xE000;

    static constexpr int COLUMNS = 64;
    static constexpr int ROWS = 32;

    static constexpr int CELL_WIDTH = 8;
    static constexpr int CELL_HEIGHT = 10;
    static constexpr int CHAR_HEIGHT = 8;

    static constexpr int WIDTH = COLUMNS * CELL_WIDTH;
    static constexpr int HEIGHT = ROWS * CELL_HEIGHT;

    static constexpr auto COLOR_TEXT = WHITE;
    static constexpr auto COLOR_BACKGROUND = BLACK;
    static constexpr Core::byte COLOR_INTENSITY = 0xA0;

    static bool HasColor(Core::byte attribute);

    static Color GetTextColor(Core::byte attribute);

    static Color GetBackgroundColor(Core::byte attribute);

    Core::ReadMemory readMemory;
    Core::ReadMemory readFont;

    Image buffer{};
    Texture2D target{};

    void drawCell(int column, int row, Core::byte code, Core::byte attribute) const;
};
