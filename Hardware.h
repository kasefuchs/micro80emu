#pragma once
#include "Display.h"
#include "I8080.h"
#include "Types.h"

class Hardware {
public:
    Hardware(Core::ReadMemory rm, Core::WriteMemory wm);

    void run(Core::address entryAddr);

    void stop();

    [[nodiscard]] bool isStopped() const;

private:
    Core::ReadMemory readMemory;
    Core::WriteMemory writeMemory;

    static Core::byte readIO(Core::address addr);

    static void writeIO(Core::address addr, Core::byte value);

    bool stopped{};

    Display display;
    I8080 cpu;
};

