#include <csignal>
#include <iostream>

#include "I8080.h"

I8080::byte memory[0x10000];
volatile bool interrupted = false;

void signalHandler(const int signal) {
    if (signal == SIGINT) interrupted = true;
}

I8080 cpu(
    [](const I8080::address addr) { return memory[addr]; },
    [](const I8080::address addr, const I8080::byte val) { memory[addr] = val; },
    [](const I8080::address addr) { return 0xFF; },
    [](const I8080::address addr, const I8080::byte val) {
    }
);

int main() {
    std::signal(SIGINT, signalHandler);

    std::cin.read(reinterpret_cast<char *>(memory), sizeof(memory));

    for (cpu.reset(0xF800); !cpu.isHalted() && !interrupted; cpu.step()) {
    }

    printf("exiting after %d cycles!\n", cpu.getCycles());

    return 0;
}
