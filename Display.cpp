#include <raylib.h>

#include "Display.h"

Display::Display(Core::ReadMemory rm) : readMemory(std::move(rm)) {
    InitWindow(WIDTH, HEIGHT, "Micro-80 Emulator Display");
    SetTargetFPS(TARGET_FPS);
}

Display::~Display() {
    CloseWindow();
}

void Display::ShouldClose() {
    WindowShouldClose();
}
