#include "Keyboard.h"

#include <raylib.h>
#include <unordered_map>

const std::unordered_map<int, int> Keyboard::KEYMAP = {
    {KEY_ZERO, 0}, {KEY_ONE, 1}, {KEY_TWO, 2},
    {KEY_THREE, 3}, {KEY_FOUR, 4}, {KEY_FIVE, 5},
    {KEY_SIX, 6}, {KEY_SEVEN, 8}, {KEY_EIGHT, 9},
    {KEY_NINE, 10}, {KEY_SEMICOLON, 11}, {KEY_EQUAL, 12},
    {KEY_COMMA, 13}, {KEY_MINUS, 14}, {KEY_PERIOD, 16},
    {KEY_SLASH, 17}, {KEY_GRAVE, 18}, {KEY_A, 19},
    {KEY_B, 20}, {KEY_C, 21}, {KEY_D, 22},
    {KEY_E, 24}, {KEY_F, 25}, {KEY_G, 26},
    {KEY_H, 27}, {KEY_I, 28}, {KEY_J, 29},
    {KEY_K, 30}, {KEY_L, 32}, {KEY_M, 33},
    {KEY_N, 34}, {KEY_O, 35}, {KEY_P, 36},
    {KEY_Q, 37}, {KEY_R, 38}, {KEY_S, 40},
    {KEY_T, 41}, {KEY_U, 42}, {KEY_V, 43},
    {KEY_W, 44}, {KEY_X, 45}, {KEY_Y, 46},
    {KEY_Z, 48}, {KEY_LEFT_BRACKET, 49}, {KEY_BACKSLASH, 50},
    {KEY_RIGHT_BRACKET, 51}, {KEY_APOSTROPHE, 52}, {KEY_SPACE, 54},
    {KEY_RIGHT, 56}, {KEY_LEFT, 57}, {KEY_UP, 58},
    {KEY_DOWN, 59}, {KEY_ENTER, 60}, {KEY_F2, 61},
    {KEY_F3, 62}, {KEY_BACKSPACE, 64}, {KEY_F1, 65},
    {KEY_LEFT_SHIFT, 66}, {KEY_RIGHT_SHIFT, 66}
};

Keyboard::Keyboard() {
    matrix.fill(0xFF);
}

void Keyboard::processKey(const int key, const bool pressed) {
    const int row = key >> 3;
    const Core::byte mask = 1 << (key & 7);

    if (pressed) matrix[row] &= ~mask;
    else matrix[row] |= mask;
}

void Keyboard::update() {
    for (const std::pair<const int, int> &kv: KEYMAP) {
        if (IsKeyPressed(kv.first)) return processKey(kv.second, true);
        if (IsKeyReleased(kv.first)) return processKey(kv.second, false);
    }
}

Core::byte Keyboard::read(const int columnMask) const {
    Core::byte result = 0xFF;
    for (int row = 0; row < ROWS; ++row) {
        if (~columnMask & 1 << row) result &= matrix[row];
    }

    return result;
}
