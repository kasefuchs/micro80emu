#pragma once
#include <functional>

#include "Types.h"

class Display {
public:
    explicit Display(Core::ReadMemory rm);

    ~Display();

    static void ShouldClose();

private:
    static constexpr int WIDTH = 512;
    static constexpr int HEIGHT = 320;
    static constexpr int TARGET_FPS = 60;

    Core::ReadMemory readMemory;
};
