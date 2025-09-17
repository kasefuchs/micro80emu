#pragma once
#include "Screen.h"
#include "I8080.h"
#include "Types.h"

class Hardware {
public:
    Hardware(Core::ReadMemory rm, Core::WriteMemory wm, Core::ReadMemory rf);

    void run(Core::address entryAddr);

    void stop();

    [[nodiscard]] bool shouldExit() const;

private:
    static Rectangle GetWindowBounds();

    static Core::byte ReadIO(Core::address addr);

    static void WriteIO(Core::address addr, Core::byte value);

    Core::ReadMemory readMemory;
    Core::WriteMemory writeMemory;
    Core::ReadMemory readFont;

    bool stopped{};

    I8080 cpu;
    Screen screen;

    void initializeGraphics();
};

