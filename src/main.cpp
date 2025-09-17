#include <csignal>
#include <fstream>
#include <iostream>
#include <cxxopts.hpp>

#include "Hardware.h"
#include "Screen.h"

Core::byte rom[0x800];
Core::byte font[0x2000];
Core::byte memory[0xF800];

constexpr Core::address ROM_OFFSET = 0xF800;

static Hardware *hardwarePtr = nullptr;

void signalHandler(int) {
    if (hardwarePtr) hardwarePtr->stop();
}

void loadBinaryFile(const std::string &path, void *buffer, const std::streamsize size) {
    std::ifstream file(path, std::ios::binary);
    file.read(static_cast<char *>(buffer), size);
    file.close();
}

cxxopts::Options options("micro80emu", "Micro-80 Emulator");

int main(const int argc, char *argv[]) {
    // Signals.
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // CLI options.
    options.add_options()
            ("h,help", "display this help")
            ("r,rom", "ROM image file", cxxopts::value<std::string>()->default_value("monitor.rom"))
            ("m,mem", "memory image file", cxxopts::value<std::string>())
            ("f,font", "font image file", cxxopts::value<std::string>()->default_value("font.rom"))
            ("d,dump", "write memory dump to file before exit", cxxopts::value<std::string>())
            ("e,entry", "entry address for CPU reset (hex)", cxxopts::value<std::string>()->default_value("0xF800"))
            ("c,clock", "CPU clock rate", cxxopts::value<float>()->default_value("1"))
            ("s,scale", "screen scaling factor", cxxopts::value<float>()->default_value("2"));

    const auto result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    // Load ROM image
    loadBinaryFile(result["rom"].as<std::string>(), rom, sizeof(rom));

    // Load font image
    loadBinaryFile(result["font"].as<std::string>(), font, sizeof(font));

    // Load memory image if exists
    if (result.count("mem")) loadBinaryFile(result["mem"].as<std::string>(), memory, sizeof(memory));

    // Initialize hardware.
    static Hardware hardware(
        [](const Core::address addr) {
            return addr >= ROM_OFFSET ? rom[addr % sizeof(rom)] : memory[addr];
        },
        [](const Core::address addr, const Core::byte val) {
            if (addr < ROM_OFFSET) memory[addr] = val;
        },
        [](const Core::address addr) {
            return font[addr];
        }
    );

    hardwarePtr = &hardware;

    // Set window scale.
    Screen::SetWindowScale(result["scale"].as<float>());

    // Set CPU clock rate.
    hardware.setClockRate(result["clock"].as<float>());

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
