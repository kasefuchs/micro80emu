#include "Hardware.h"

#include <fstream>
#include <raylib.h>

#include "Video.h"

Hardware::Hardware(Core::ReadMemory rm, Core::WriteMemory wm, Core::ReadMemory rf)
    : readMemory(std::move(rm)), writeMemory(std::move(wm)), readFont(std::move(rf)),
      cpu(
          readMemory,
          writeMemory,
          [this](const Core::address addr) { return readIO(addr); },
          [this](const Core::address addr, const Core::byte value) { writeIO(addr, value); }
      ),
      video(readMemory, readFont) {
    initializeGraphics();
}

Rectangle Hardware::GetWindowBounds() {
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();

    return {0, 0, static_cast<float>(width), static_cast<float>(height)};
}

Core::byte Hardware::readIO(const Core::address addr) const {
    switch (static_cast<Port>(addr)) {
        case Port::KeyboardModifier:
            return keyboard.read(0xFF);
        case Port::KeyboardRow:
            return keyboard.read(keyboardColumn | 0x100);
        default: return 0x82;
    }
}

void Hardware::writeIO(const Core::address addr, const Core::byte value) {
    switch (static_cast<Port>(addr)) {
        case Port::KeyboardColumn:
            keyboardColumn = value;
            break;
        default: break;
    }
}

void Hardware::initializeGraphics() {
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    const Rectangle bounds = Video::GetTextureBounds();
    InitWindow(static_cast<int>(bounds.width), static_cast<int>(bounds.height), "Micro-80 Emulator");

    const int monitor = GetCurrentMonitor();
    const int refreshRate = GetMonitorRefreshRate(monitor);
    SetTargetFPS(refreshRate);

    video.initialize();
    videoOutput = LoadTextureFromImage(video.getBuffer());
}

void Hardware::run(const Core::address entryAddr) {
    cpu.reset(entryAddr);

    double leftoverCycles = 0.0;
    while (!shouldExit()) {
        BeginDrawing();
        {
            keyboard.update();

            const float delta = GetFrameTime();
            double cyclesToRun = delta * I8080::CLOCK_FREQUENCY + leftoverCycles;
            while (cyclesToRun >= I8080::MIN_INSTRUCTION_CYCLES) cyclesToRun -= static_cast<double>(cpu.step());
            leftoverCycles = cyclesToRun;

            UpdateTexture(videoOutput, video.updateBuffer().data);
            DrawTexturePro(videoOutput, Video::GetTextureBounds(), GetWindowBounds(), {0, 0}, 0.0f, WHITE);
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
