#pragma once
#include "Screen.h"
#include "I8080.h"
#include "Types.h"

class Hardware {
public:
    Hardware(Core::ReadMemory rm, Core::WriteMemory wm, Core::ReadMemory rf);

    void run(Core::address entryAddr);

    void stop();

    void setClockRate(float value);

    [[nodiscard]] bool isStopped() const;

private:
    Core::ReadMemory readMemory;
    Core::WriteMemory writeMemory;
    Core::ReadMemory readFont;

    float clockRate = 1.0f;

    static Core::byte readIO(Core::address addr);

    static void writeIO(Core::address addr, Core::byte value);

    bool stopped{};

    Screen screen;
    I8080 cpu;
};

