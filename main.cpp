#include <csignal>
#include <fstream>
#include <iostream>
#include <cxxopts.hpp>

#include "I8080.h"

constexpr I8080::address ROM_START = 0xF800;

I8080::byte rom[0x800];
I8080::byte memory[0xF800];

volatile bool interrupted = false;

void signalHandler(const int signal) {
    if (signal == SIGINT) interrupted = true;
}

I8080 cpu(
    [](const I8080::address addr) {
        return addr >= ROM_START ? rom[addr % sizeof(rom)] : memory[addr];
    },
    [](const I8080::address addr, const I8080::byte val) {
        if (addr < ROM_START) memory[addr] = val;
    },
    [](const I8080::address addr) {
        return 0xFF;
    },
    [](const I8080::address addr, const I8080::byte val) {
    }
);

cxxopts::Options options("micro80emu", "Micro-80 Emulator");

int main(const int argc, char *argv[]) {
    options.add_options()
            ("h,help", "display this help")
            ("r,rom", "ROM image file", cxxopts::value<std::string>()->default_value("rom.bin"))
            ("m,mem", "memory image file", cxxopts::value<std::string>())
            ("d,dump", "write memory dump to file before exit", cxxopts::value<std::string>())
            ("e,entry", "entry address for CPU reset (hex)", cxxopts::value<std::string>()->default_value("0xF800"));

    const auto result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    std::ifstream romFile(result["rom"].as<std::string>(), std::ios::binary);
    romFile.read(reinterpret_cast<char *>(rom), sizeof(rom));
    romFile.close();

    if (result.count("mem")) {
        std::ifstream memFile(result["mem"].as<std::string>(), std::ios::binary);
        memFile.read(reinterpret_cast<char *>(memory), sizeof(memory));
        memFile.close();
    }

    const I8080::address entryAddr = std::stoul(result["entry"].as<std::string>(), nullptr, 16);

    std::signal(SIGINT, signalHandler);
    for (cpu.reset(entryAddr); !cpu.isHalted() && !interrupted; cpu.step()) {
    }

    if (result.count("dump")) {
        std::ofstream dumpFile(result["dump"].as<std::string>(), std::ios_base::binary);
        dumpFile.write(reinterpret_cast<const std::ostream::char_type *>(memory), sizeof(memory));
        dumpFile.close();
    }

    return 0;
}
