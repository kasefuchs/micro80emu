#ifndef MICRO80EMU_I8080_H
#define MICRO80EMU_I8080_H
#include <cstdint>
#include <functional>


class I8080 {
public:
    using ReadMem = std::function<std::uint8_t(std::uint16_t)>;
    using WriteMem = std::function<void(std::uint16_t, std::uint8_t)>;
    using ReadIO = std::function<std::uint8_t(std::uint16_t)>;
    using WriteIO = std::function<void(std::uint16_t, std::uint8_t)>;

    enum class Opcode : std::uint8_t {
        NOP = 0x00, LXI_B = 0x01, STAX_B = 0x02, INX_B = 0x03,
        INR_B = 0x04, DCR_B = 0x05, MVI_B = 0x06, RLC = 0x07,
        NOP08 = 0x08, DAD_B = 0x09, LDAX_B = 0x0A, DCX_B = 0x0B,
        INR_C = 0x0C, DCR_C = 0x0D, MVI_C = 0x0E, RRC = 0x0F,
        NOP10 = 0x10, LXI_D = 0x11, STAX_D = 0x12, INX_D = 0x13,
        INR_D = 0x14, DCR_D = 0x15, MVI_D = 0x16, RAL = 0x17,
        NOP18 = 0x18, DAD_D = 0x19, LDAX_D = 0x1A, DCX_D = 0x1B,
        INR_E = 0x1C, DCR_E = 0x1D, MVI_E = 0x1E, RAR = 0x1F,

        MOV_B_B = 0x40, MOV_B_C = 0x41, MOV_B_D = 0x42, MOV_B_E = 0x43,
        MOV_B_H = 0x44, MOV_B_L = 0x45, MOV_B_M = 0x46, MOV_B_A = 0x47,
        MOV_C_B = 0x48, MOV_C_C = 0x49, MOV_C_D = 0x4A, MOV_C_E = 0x4B,
        MOV_C_H = 0x4C, MOV_C_L = 0x4D, MOV_C_M = 0x4E, MOV_C_A = 0x4F,
        MOV_D_B = 0x50, MOV_D_C = 0x51, MOV_D_D = 0x52, MOV_D_E = 0x53,
        MOV_D_H = 0x54, MOV_D_L = 0x55, MOV_D_M = 0x56, MOV_D_A = 0x57,
        MOV_E_B = 0x58, MOV_E_C = 0x59, MOV_E_D = 0x5A, MOV_E_E = 0x5B,
        MOV_E_H = 0x5C, MOV_E_L = 0x5D, MOV_E_M = 0x5E, MOV_E_A = 0x5F,
        MOV_H_B = 0x60, MOV_H_C = 0x61, MOV_H_D = 0x62, MOV_H_E = 0x63,
        MOV_H_H = 0x64, MOV_H_L = 0x65, MOV_H_M = 0x66, MOV_H_A = 0x67,
        MOV_L_B = 0x68, MOV_L_C = 0x69, MOV_L_D = 0x6A, MOV_L_E = 0x6B,
        MOV_L_H = 0x6C, MOV_L_L = 0x6D, MOV_L_M = 0x6E, MOV_L_A = 0x6F,
        MOV_M_B = 0x70, MOV_M_C = 0x71, MOV_M_D = 0x72, MOV_M_E = 0x73,
        MOV_M_H = 0x74, MOV_M_L = 0x75, HLT = 0x76, MOV_M_A = 0x77,
        MOV_A_B = 0x78, MOV_A_C = 0x79, MOV_A_D = 0x7A, MOV_A_E = 0x7B,
        MOV_A_H = 0x7C, MOV_A_L = 0x7D, MOV_A_M = 0x7E, MOV_A_A = 0x7F,

        OUT = 0xD3, IN = 0xDB,
        DI = 0xF3, EI = 0xFB
    };

    I8080(ReadMem rm, WriteMem wm, ReadIO rio = nullptr, WriteIO wio = nullptr);

    int step();

    void jump(std::uint16_t addr);

    void reset(std::uint16_t addr = 0);

    [[nodiscard]] bool isHalted() const;

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

    static constexpr std::array<std::uint8_t, 256> PARITY_TABLE = [] {
        std::array<std::uint8_t, 256> arr{};
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
    ReadIO readIO;
    WriteIO writeIO;

    [[nodiscard]] std::uint16_t getBC() const;

    [[nodiscard]] std::uint16_t getDE() const;

    [[nodiscard]] std::uint16_t getHL() const;

    void setBC(std::uint16_t v);

    void setDE(std::uint16_t v);

    void setHL(std::uint16_t v);

    std::uint8_t fetchByte();

    std::uint16_t fetchWord();

    [[nodiscard]] std::uint8_t packFlags() const;

    void updateFlagsInr(std::uint8_t v);

    void updateFlagsDcr(std::uint8_t v);

    int executeMov(Opcode opcode);

    int executeOpcode(Opcode opcode);
};


#endif //MICRO80EMU_I8080_H
