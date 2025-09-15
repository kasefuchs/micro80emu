#include "Hardware.h"

#include <fstream>
#include <raylib.h>

Hardware::Hardware(Core::ReadMemory rm, Core::WriteMemory wm, Core::ReadMemory rf)
    : readMemory(std::move(rm)), writeMemory(std::move(wm)), readFont(std::move(rf)),
      screen(readMemory, readFont),
      cpu(
          readMemory,
          writeMemory,
          [](const Core::address addr) { return readIO(addr); },
          [](const Core::address addr, const Core::byte val) { writeIO(addr, val); }
      ) {
}

void Hardware::run(const Core::address entryAddr) {
    cpu.reset(entryAddr);

    double leftoverCycles = 0.0;
    while (!isStopped()) {
        screen.render();

        BeginDrawing();
        {
            const float delta = GetFrameTime();
            double cyclesToRun = delta * I8080::FREQUENCY + leftoverCycles;
            while (cyclesToRun > 0) cyclesToRun -= cpu.step();
            leftoverCycles = cyclesToRun;

            screen.draw();
        }
        EndDrawing();
    }
}

void Hardware::stop() {
    stopped = true;
}

bool Hardware::isStopped() const {
    return stopped || cpu.isHalted() || WindowShouldClose();
}

Core::byte Hardware::readIO(const Core::address addr) {
    return 0xFF;
}

void Hardware::writeIO(const Core::address addr, const Core::byte value) {
}
