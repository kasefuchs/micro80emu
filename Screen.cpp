#include <raylib.h>

#include "Screen.h"

Screen::Screen(Core::ReadMemory rm, Core::ReadMemory rf)
    : readMemory(std::move(rm)), readFont(std::move(rf)) {
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(WINDOW_FPS);
    InitWindow(WIDTH, HEIGHT, "Micro-80 Emulator Screen");

    buffer = GenImageColor(WIDTH, HEIGHT, BLACK);
    target = LoadRenderTexture(WIDTH, HEIGHT);
}

Screen::~Screen() {
    UnloadRenderTexture(target);

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

Color Screen::GetTextColor(const Core::byte attribute) {
    const int intensityAdjustment = (attribute >> 3 & 1) * (0xFF - COLOR_INTENSITY);
    const Core::byte red = (attribute >> 2 & 1) * COLOR_INTENSITY + intensityAdjustment;
    const Core::byte green = (attribute >> 1 & 1) * COLOR_INTENSITY + intensityAdjustment;
    const Core::byte blue = (attribute >> 0 & 1) * COLOR_INTENSITY + intensityAdjustment;
    return {red, green, blue, 0xFF};
}

Color Screen::GetBackgroundColor(const Core::byte attribute) {
    const Core::byte red = (attribute >> 6 & 1) * COLOR_INTENSITY;
    const Core::byte green = (attribute >> 5 & 1) * COLOR_INTENSITY;
    const Core::byte blue = (attribute >> 4 & 1) * COLOR_INTENSITY;
    return {red, green, blue, 0xFF};
}

void Screen::draw() const {
    const Rectangle bounds = GetWindowBounds();

    DrawTexturePro(target.texture, {0, 0, WIDTH, HEIGHT}, bounds, {0, 0}, 0.0f, RAYWHITE);
}

void Screen::drawCharacter(
    const int column, const int row,
    const Core::byte code, const Core::byte attribute
) const {
    for (int py = 0; py < CHAR_HEIGHT; py++) {
        const int fontData = readFont(py % 8 + code * 8 + (py & 8 ? 0x800 : 0) + (attribute & 0x80 ? 0x1000 : 0));

        for (int px = 0; px < CHAR_WIDTH; px++) {
            const int x = column * CHAR_WIDTH + px;
            const int y = row * CHAR_HEIGHT + py;

            const Color color = fontData & 0x80 >> px ? GetBackgroundColor(attribute) : GetTextColor(attribute);

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

    UpdateTexture(target.texture, buffer.data);
}
