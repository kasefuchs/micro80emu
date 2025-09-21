#include <raylib.h>

#include "Video.h"

Video::Video(Core::ReadMemory rm, Core::ReadMemory rf)
    : readMemory(std::move(rm)), readFont(std::move(rf)) {
}

Video::~Video() {
    UnloadImage(buffer);
}

bool Video::HasColor(const Core::byte attribute) {
    return attribute & 0x7F;
}

Rectangle Video::GetTextureBounds() {
    return {0, 0, WIDTH, HEIGHT};
}

Color Video::GetTextColor(const Core::byte attribute) {
    if (!HasColor(attribute)) return COLOR_TEXT;

    const Core::byte intensityAdjustment = (attribute >> 3 & 1) * (0xFF - COLOR_INTENSITY);
    const Core::byte red = (attribute >> 2 & 1) * COLOR_INTENSITY + intensityAdjustment;
    const Core::byte green = (attribute >> 1 & 1) * COLOR_INTENSITY + intensityAdjustment;
    const Core::byte blue = (attribute >> 0 & 1) * COLOR_INTENSITY + intensityAdjustment;
    return {red, green, blue, 0xFF};
}

Color Video::GetBackgroundColor(const Core::byte attribute) {
    if (!HasColor(attribute)) return COLOR_BACKGROUND;

    const Core::byte red = (attribute >> 6 & 1) * COLOR_INTENSITY;
    const Core::byte green = (attribute >> 5 & 1) * COLOR_INTENSITY;
    const Core::byte blue = (attribute >> 4 & 1) * COLOR_INTENSITY;
    return {red, green, blue, 0xFF};
}

void Video::initialize() {
    buffer = GenImageColor(WIDTH, HEIGHT, COLOR_BACKGROUND);
}

void Video::drawCell(const int column, const int row, const Core::byte code, const Core::byte attribute) const {
    const bool invertNextBit = column != COLUMNS - 1 && attribute & 0x80;

    const Color textColor = GetTextColor(attribute);
    const Color backgroundColor = GetBackgroundColor(attribute);

    for (int dy = 0; dy < CELL_HEIGHT; dy++) {
        const Core::byte fontData = readFont(
            dy % 8 + code * 8 + (dy & 8 ? 0x800 : 0) + (attribute & 0x80 ? 0x1000 : 0));

        for (int dx = 0; dx < CELL_WIDTH; dx++) {
            const int x = column * CELL_WIDTH + dx;
            const int y = row * CELL_HEIGHT + dy;

            Color color;
            if (dy < CHAR_HEIGHT) {
                bool pixelOn = fontData & 0x80 >> dx;
                if (invertNextBit) pixelOn = !pixelOn;
                color = pixelOn ? backgroundColor : textColor;
            } else color = invertNextBit ? textColor : backgroundColor;

            static_cast<Color *>(buffer.data)[WIDTH * y + x] = color;
        }
    }
}

Image Video::getBuffer() const {
    return buffer;
}

Image Video::updateBuffer() const {
    Core::address charCodeAddress = CHAR_CODE_OFFSET;
    Core::address attributeAddress = ATTRIBUTE_OFFSET;

    for (int row = 0; row < ROWS; row++) {
        for (int column = 0; column < COLUMNS; column++) {
            const Core::byte code = readMemory(charCodeAddress++);
            const Core::byte attribute = readMemory(attributeAddress++);

            drawCell(column, row, code, attribute);
        }
    }

    return buffer;
}
