#include <cstdint>
#include <fstream>
#include <iostream>

#include "I8080.h"

I8080::byte memory[0x10000];

int main() {
    std::cin.read(reinterpret_cast<char *>(memory), sizeof(memory));

    I8080 cpu(
        [&](const I8080::address addr) { return memory[addr]; },
        [&](const I8080::address addr, const I8080::byte val) { memory[addr] = val; },
        [](const I8080::address addr) { return 0xFF; },
        [](const I8080::address addr, const I8080::byte val) {
        }
    );

    for (cpu.reset(0xF800); !cpu.isHalted(); cpu.step()) {
    }

    return 0;
}
