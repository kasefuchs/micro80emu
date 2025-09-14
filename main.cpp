#include <csignal>
#include <fstream>
#include <iostream>
#include <cxxopts.hpp>

#include "Hardware.h"

Core::byte rom[0x800];
Core::byte memory[0xF800];

constexpr Core::address ROM_START = 0xF800;
Hardware hardware(
    [](const Core::address addr) {
        return addr >= ROM_START ? rom[addr % sizeof(rom)] : memory[addr];
    },
    [](const Core::address addr, const Core::byte val) {
        if (addr < ROM_START) memory[addr] = val;
    }
);

void signalHandler(const int _) { hardware.stop(); }

cxxopts::Options options("micro80emu", "Micro-80 Emulator");

int main(const int argc, char *argv[]) {
    // Signals.
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // CLI options.
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

    // Load ROM image
    std::ifstream romFile(result["rom"].as<std::string>(), std::ios::binary);
    romFile.read(reinterpret_cast<char *>(rom), sizeof(rom));
    romFile.close();

    // Load memory image if exists
    if (result.count("mem")) {
        std::ifstream memFile(result["mem"].as<std::string>(), std::ios::binary);
        memFile.read(reinterpret_cast<char *>(memory), sizeof(memory));
        memFile.close();
    }

    // Parse entry address
    const Core::address entryAddr = std::stoul(result["entry"].as<std::string>(), nullptr, 16);

    hardware.run(entryAddr);

    if (result.count("dump")) {
        std::ofstream dumpFile(result["dump"].as<std::string>(), std::ios_base::binary);
        dumpFile.write(reinterpret_cast<const std::ostream::char_type *>(memory), sizeof(memory));
        dumpFile.close();
    }

    return 0;
}
