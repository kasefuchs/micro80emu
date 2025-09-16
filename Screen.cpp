#include <raylib.h>

#include "Screen.h"

Screen::Screen(Core::ReadMemory rm, Core::ReadMemory rf)
    : readMemory(std::move(rm)), readFont(std::move(rf)) {
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(WINDOW_FPS);
    InitWindow(WIDTH, HEIGHT, "Micro-80 Emulator Screen");

    buffer = GenImageColor(WIDTH, HEIGHT, BLACK);
    target = LoadTextureFromImage(buffer);
}

Screen::~Screen() {
    UnloadTexture(target);

    CloseWindow();
}

Rectangle Screen::GetWindowBounds() {
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();

    return {0, 0, static_cast<float>(width), static_cast<float>(height)};
}

void Screen::SetWindowScale(const float value) {
    SetWindowSize(static_cast<int>(WIDTH * value), static_cast<int>(HEIGHT * value));
}

void Screen::ShouldClose() {
    WindowShouldClose();
}

bool Screen::HasColor(const Core::byte attribute) {
    return attribute & 0x7F;
}

Color Screen::GetTextColor(const Core::byte attribute) {
    if (!HasColor(attribute)) return WHITE;

    const int intensityAdjustment = (attribute >> 3 & 1) * (0xFF - COLOR_INTENSITY);
    const Core::byte red = (attribute >> 2 & 1) * COLOR_INTENSITY + intensityAdjustment;
    const Core::byte green = (attribute >> 1 & 1) * COLOR_INTENSITY + intensityAdjustment;
    const Core::byte blue = (attribute >> 0 & 1) * COLOR_INTENSITY + intensityAdjustment;
    return {red, green, blue, 0xFF};
}

Color Screen::GetBackgroundColor(const Core::byte attribute) {
    if (!HasColor(attribute)) return BLACK;

    const Core::byte red = (attribute >> 6 & 1) * COLOR_INTENSITY;
    const Core::byte green = (attribute >> 5 & 1) * COLOR_INTENSITY;
    const Core::byte blue = (attribute >> 4 & 1) * COLOR_INTENSITY;
    return {red, green, blue, 0xFF};
}

void Screen::draw() const {
    const Rectangle bounds = GetWindowBounds();

    DrawTexturePro(target, {0, 0, WIDTH, HEIGHT}, bounds, {0, 0}, 0.0f, RAYWHITE);
}

void Screen::drawCharacter(
    const int column, const int row,
    const Core::byte code, const Core::byte attribute
) const {
    const bool invertNextBit = column != COLUMNS - 1 && attribute & 0x80;

    const Color textColor = GetTextColor(attribute);
    const Color backgroundColor = GetBackgroundColor(attribute);

    for (int dy = 0; dy < CHAR_HEIGHT; dy++) {
        const Core::byte fontData = readFont(
            dy % 8 + code * 8 + (dy & 8 ? 0x800 : 0) + (attribute & 0x80 ? 0x1000 : 0));

        for (int dx = 0; dx < CHAR_WIDTH; dx++) {
            const int x = column * CHAR_WIDTH + dx;
            const int y = row * CHAR_HEIGHT + dy;

            Color color;
            if (dy < 8) {
                bool pixelOn = fontData & 0x80 >> dx;
                if (invertNextBit) pixelOn = !pixelOn;
                color = pixelOn ? backgroundColor : textColor;
            } else color = invertNextBit ? textColor : backgroundColor;

            static_cast<Color *>(buffer.data)[y * WIDTH + x] = color;
        }
    }
}

void Screen::render() const {
    Core::address charCodeAddress = CHAR_CODE_OFFSET;
    Core::address attributeAddress = ATTRIBUTE_OFFSET;

    for (int row = 0; row < ROWS; row++) {
        for (int column = 0; column < COLUMNS; column++) {
            const Core::byte code = readMemory(charCodeAddress++);
            const Core::byte attribute = readMemory(attributeAddress++);

            drawCharacter(column, row, code, attribute);
        }
    }

    UpdateTexture(target, buffer.data);
}
