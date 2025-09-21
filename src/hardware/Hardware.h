#pragma once
#include "Types.h"
#include "I8080.h"
#include "../video/Video.h"
#include "Keyboard.h"

class Hardware {
public:
    Hardware(Core::ReadMemory rm, Core::WriteMemory wm, Core::ReadMemory rf);

    void run(Core::address entryAddr);

    void stop();

    [[nodiscard]] bool shouldExit() const;

private:
    enum class Port {
        KeyboardModifier = 0x05,
        KeyboardRow = 0x06,
        KeyboardColumn = 0x07,
    };

    static Rectangle GetWindowBounds();

    Core::byte readIO(Core::address addr) const;

    void writeIO(Core::address addr, Core::byte value);

    Core::ReadMemory readMemory;
    Core::WriteMemory writeMemory;
    Core::ReadMemory readFont;

    bool stopped{};
    Texture2D videoOutput{};
    Core::byte keyboardColumn = 0xFF;

    I8080 cpu;
    Video video;
    Keyboard keyboard;

    void initializeGraphics();
};

