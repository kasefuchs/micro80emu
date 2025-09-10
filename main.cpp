#include <cstdint>
#include <fstream>
#include <iostream>

#include "I8080.h"

std::uint8_t memory[0x10000];

int main() {
    std::cin.read(reinterpret_cast<char *>(memory), sizeof(memory));

    I8080 cpu(
        [&](const std::uint16_t addr) { return memory[addr]; },
        [&](const std::uint16_t addr, const std::uint8_t val) { memory[addr] = val; },
        [](const std::uint16_t addr) { return 0x82; },
        [](const std::uint16_t addr, const std::uint8_t val) { printf("out: %04X %02X\n", addr, val); }
    );

    cpu.reset();

    int cycles = 0;
    do {
        cycles += cpu.step();
        printf("cycles: %d\n", cycles);
        cpu.dumpRegisters();
    } while (!cpu.isHalted());

    return 0;
}
