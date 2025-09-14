#pragma once
#include <cstdint>

namespace Core {
    using byte = std::uint8_t;
    using word = std::uint16_t;
    using address = std::uint16_t;

    using ReadMemory = std::function<byte(address)>;
    using WriteMemory = std::function<void(address, byte)>;
}
