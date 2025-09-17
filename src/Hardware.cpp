#include "Hardware.h"

#include <fstream>
#include <raylib.h>

#include "Screen.h"

Hardware::Hardware(Core::ReadMemory rm, Core::WriteMemory wm, Core::ReadMemory rf)
    : readMemory(std::move(rm)), writeMemory(std::move(wm)), readFont(std::move(rf)),
      cpu(
          readMemory,
          writeMemory,
          &Hardware::ReadIO,
          &Hardware::WriteIO
      ),
      screen(readMemory, readFont) {
    initializeGraphics();
}

Rectangle Hardware::GetWindowBounds() {
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();

    return {0, 0, static_cast<float>(width), static_cast<float>(height)};
}

Core::byte Hardware::ReadIO(const Core::address addr) {
    // TODO: Implement IO read.
    return 0xFF;
}

void Hardware::WriteIO(const Core::address addr, const Core::byte value) {
    // TODO: Implement IO write.
}

void Hardware::initializeGraphics() {
    const int monitor = GetCurrentMonitor();
    const int refreshRate = GetMonitorRefreshRate(monitor);
    SetTargetFPS(refreshRate);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    const Rectangle bounds = Screen::GetTextureBounds();
    InitWindow(static_cast<int>(bounds.width), static_cast<int>(bounds.height), "Micro-80 Emulator");

    screen.initialize();
}

void Hardware::run(const Core::address entryAddr) {
    cpu.reset(entryAddr);

    double leftoverCycles = 0.0;
    while (!shouldExit()) {
        const float delta = GetFrameTime();
        double cyclesToRun = delta * I8080::FREQUENCY + leftoverCycles;
        while (cyclesToRun > 0) cyclesToRun -= static_cast<double>(cpu.step());
        leftoverCycles = cyclesToRun;

        screen.updateTexture();
        BeginDrawing();
        {
            const Rectangle textureBounds = Screen::GetTextureBounds();
            const Rectangle windowBounds = GetWindowBounds();
            DrawTexturePro(screen.getTexture(), textureBounds, windowBounds, {0, 0}, 0.0f, WHITE);
        }
        EndDrawing();
    }
}

void Hardware::stop() {
    stopped = true;
}

bool Hardware::shouldExit() const {
    return stopped || cpu.isHalted() || WindowShouldClose();
}

