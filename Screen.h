#pragma once
#include <functional>
#include <raylib.h>

#include "Types.h"

class Screen {
public:
    Screen(Core::ReadMemory rm, Core::ReadMemory rf);

    ~Screen();

    static void SetWindowScale(float value);

    static void ShouldClose();

    void draw() const;

    void render() const;

private:
    static constexpr Core::address CHAR_CODE_OFFSET = 0xE800;
    static constexpr Core::address ATTRIBUTE_OFFSET = 0xE000;

    static constexpr int CHAR_WIDTH = 8;
    static constexpr int CHAR_HEIGHT = 10;

    static constexpr int COLUMNS = 64;
    static constexpr int ROWS = 32;

    static constexpr int WIDTH = COLUMNS * CHAR_WIDTH;
    static constexpr int HEIGHT = ROWS * CHAR_HEIGHT;

    static constexpr int WINDOW_FPS = 30;

    static constexpr int COLOR_INTENSITY = 0xA0;

    Image buffer{};
    RenderTexture2D target{};

    Core::ReadMemory readMemory;
    Core::ReadMemory readFont;

    static Color GetTextColor(Core::byte attribute);

    static Color GetBackgroundColor(Core::byte attribute);

    static Rectangle GetWindowBounds();

    void drawCharacter(int column, int row, Core::byte code, Core::byte attribute) const;
};
