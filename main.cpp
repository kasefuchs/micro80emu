#include <cstdint>
#include <iostream>

#include "I8080.h"

uint8_t memory[0x10000];

int main() {
    std::cin.read(reinterpret_cast<char *>(memory), sizeof(memory));

    I8080 cpu(
        [&](uint16_t addr) { return memory[addr]; },
        [&](uint16_t addr, uint8_t val) { memory[addr] = val; }
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

