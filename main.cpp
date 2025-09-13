#include <csignal>
#include <iostream>

#include "I8080.h"

I8080::byte memory[0x10000];
I8080::address loadAddr = 0x100;
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

int main(const int argc, char *argv[]) {
    std::signal(SIGINT, signalHandler);

    std::cin.read(reinterpret_cast<char *>(memory), sizeof(memory));

    if (argc > 1) {
        char *end = nullptr;
        const unsigned long val = std::strtoul(argv[1], &end, 0);
        if (end != argv[1] && val <= sizeof(memory)) loadAddr = static_cast<I8080::address>(val);
        else {
            printf("Invalid load address: %s\n", argv[1]);
            return 1;
        }
    }

    for (cpu.reset(loadAddr); !cpu.isHalted() && !interrupted; cpu.step()) {
    }

    printf("exiting after %d cycles!\n", cpu.getCycles());

    return 0;
}
