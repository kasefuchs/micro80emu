#pragma once
#include <array>
#include <unordered_map>

#include "Types.h"

class Keyboard {
public:
    Keyboard();

    void update();

    Core::byte read(int columnMask) const;

private:
    static const std::unordered_map<int, int> KEYMAP;
    static constexpr int ROWS = 9;

    enum class KeyState {
        None,
        Pressed,
        Released,
    };

    std::array<Core::byte, ROWS> matrix{};

    void processKey(int key, bool pressed);
};
