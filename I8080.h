#ifndef MICRO80EMU_I8080_H
#define MICRO80EMU_I8080_H
#include <cstdint>
#include <functional>


class I8080 {
public:
    using ReadMem = std::function<uint8_t(uint16_t)>;
    using WriteMem = std::function<void(uint16_t, uint8_t)>;

    enum class Opcode : uint8_t {
        NOP = 0x00, LXI_B = 0x01, STAX_B = 0x02, INX_B = 0x03, INR_B = 0x04, DCR_B = 0x05, MVI_B = 0x06, RLC = 0x07,
        NOP08 = 0x08, DAD_B = 0x09, LDAX_B = 0x0A, DCX_B = 0x0B, INR_C = 0x0C, DCR_C = 0x0D, MVI_C = 0x0E, RRC = 0x0F,
        NOP10 = 0x10, LXI_D = 0x11, STAX_D = 0x12, INX_D = 0x13, INR_D = 0x14, DCR_D = 0x15, MVI_D = 0x16, RAL = 0x17,
        NOP18 = 0x18, DAD_D = 0x19, LDAX_D = 0x1A, DCX_D = 0x1B, INR_E = 0x1C, DCR_E = 0x1D, MVI_E = 0x1E, RAR = 0x1F,

        HLT = 0x76,
    };

    I8080(ReadMem rm, WriteMem wm);

    int step();

    void jump(std::uint16_t addr);

    void reset(std::uint16_t addr = 0);

    [[nodiscard]] bool isHalted() const { return halted; }

    [[nodiscard]] std::uint16_t getProgramCounter() const;

    void dumpRegisters() const;

private:
    static constexpr int OPCODE_CYCLES[256] = {
        4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, // 0x00..0x0F
        4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, // 0x10..0x1F
        4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, // 0x20..0x2F
        4, 10, 13, 5, 5, 5, 7, 4, 4, 10, 13, 5, 5, 5, 7, 4, // 0x30..0x3F
        5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 7, 5, 4, // 0x40..0x4F
        5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 7, 5, 4, 5, // 0x50..0x5F
        5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 7, 5, 4, 5, // 0x60..0x6F
        7, 7, 7, 7, 7, 7, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, // 0x70..0x7F
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4, 4, // 0x80..0x8F
        4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, // 0x90..0x9F
        4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, // 0xA0..0xAF
        4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, // 0xB0..0xBF
        5, 11, 10, 17, 11, 7, 11, 5, 10, 17, 11, 11, 17, 7, 11, 5, // 0xC0..0xCF
        5, 10, 10, 17, 11, 7, 11, 5, 10, 17, 11, 11, 17, 7, 11, 5, // 0xD0..0xDF
        5, 11, 10, 17, 11, 7, 11, 5, 10, 5, 11, 17, 11, 7, 11, 5, // 0xE0..0xEF
        5, 11, 10, 17, 11, 7, 11, 5, 10, 5, 11, 17, 11, 7, 11, 5 // 0xF0..0xFF
    };

    static constexpr std::array<uint8_t, 256> PARITY_TABLE = [] {
        std::array<uint8_t, 256> arr{};
        for (int i = 0; i < 256; i++) {
            int bits = 0;
            for (int j = 0; j < 8; j++) if (i & 1 << j) bits++;
            arr[i] = bits % 2 == 0;
        }
        return arr;
    }();

    std::uint8_t regA{}, regB{}, regC{}, regD{}, regE{}, regH{}, regL{};
    std::uint16_t stackPointer{}, programCounter{};

    bool signFlag{}, parityFlag{}, auxCarryFlag{}, zeroFlag{}, carryFlag{};
    bool interruptEnable{}, halted{};

    ReadMem readMemory;
    WriteMem writeMemory;

    [[nodiscard]] std::uint16_t getBC() const;

    [[nodiscard]] std::uint16_t getDE() const;

    [[nodiscard]] std::uint16_t getHL() const;

    void setBC(std::uint16_t v);

    void setDE(std::uint16_t v);

    void setHL(std::uint16_t v);

    std::uint8_t fetchByte();

    std::uint16_t fetchWord();

    void updateFlagsInr(std::uint8_t v);

    void updateFlagsDcr(std::uint8_t v);

    int executeOpcode(Opcode opcode);

    uint8_t packFlags() const;
};


#endif //MICRO80EMU_I8080_H
