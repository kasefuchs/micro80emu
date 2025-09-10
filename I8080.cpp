#include "I8080.h"

#include <cstdio>
#include <iostream>

const std::array<std::uint8_t, 256> I8080::PARITY_TABLE = {
    {
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
    }
};

const int I8080::OPCODE_CYCLES[256] = {
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

I8080::I8080(ReadMem rm, WriteMem wm, ReadIO rio, WriteIO wio)
    : movRegs{&regB, &regC, &regD, &regE, &regH, &regL, nullptr, &regA},
      readMemory(std::move(rm)), writeMemory(std::move(wm)),
      readIO(std::move(rio)), writeIO(std::move(wio)) {
}

int I8080::step() {
    if (halted) return executeOpcode(Opcode::NOP);
    return executeOpcode(static_cast<Opcode>(fetchByte()));
}

void I8080::jump(const std::uint16_t addr) { programCounter = addr; }

void I8080::reset(const std::uint16_t addr) {
    interruptEnable = false;
    halted = false;
    programCounter = addr;
}

bool I8080::isHalted() const { return halted; }
std::uint16_t I8080::getProgramCounter() const { return programCounter; }

std::uint16_t I8080::getBC() const { return regB << 8 | regC; }
std::uint16_t I8080::getDE() const { return regD << 8 | regE; }
std::uint16_t I8080::getHL() const { return regH << 8 | regL; }

void I8080::setBC(const std::uint16_t v) {
    regB = v >> 8;
    regC = v & 0xFF;
}

void I8080::setDE(const std::uint16_t v) {
    regD = v >> 8;
    regE = v & 0xFF;
}

void I8080::setHL(const std::uint16_t v) {
    regH = v >> 8;
    regL = v & 0xFF;
}

std::uint8_t I8080::fetchByte() {
    const auto v = readMemory(programCounter);
    programCounter = programCounter + 1 & 0xFFFF;
    return v;
}

std::uint16_t I8080::fetchWord() {
    return fetchByte() | fetchByte() << 8;
}

void I8080::updateFlagsInr(const std::uint8_t v) {
    auxCarryFlag = (v & 0x0F) == 0;
    signFlag = v & 0x80;
    zeroFlag = v == 0;
    parityFlag = PARITY_TABLE[v];
}

void I8080::updateFlagsDcr(const std::uint8_t v) {
    auxCarryFlag = (v & 0x0F) != 0x0F;
    signFlag = v & 0x80;
    zeroFlag = v == 0;
    parityFlag = PARITY_TABLE[v];
}

int I8080::executeMov(Opcode opcode) const {
    const int dest = static_cast<int>(opcode) >> 3 & 0x07;
    const int src = static_cast<int>(opcode) & 0x07;

    auto readReg = [&](const int code) -> uint8_t {
        if (code == 6) return readMemory(getHL());
        return *movRegs[code];
    };

    auto writeReg = [&](const int code, const uint8_t value) {
        if (code == 6) {
            writeMemory(getHL(), value);
            return 7;
        }

        *movRegs[code] = value;
        return 5;
    };

    return writeReg(dest, readReg(src));
}

int I8080::executeMvi(Opcode opcode) {
    const int dest = static_cast<int>(opcode) >> 3 & 0x07;
    const uint8_t value = fetchByte();

    if (dest == 6) {
        writeMemory(getHL(), value);
        return 10;
    }

    *movRegs[dest] = value;
    return 7;
}

int I8080::executeOpcode(Opcode opcode) {
    std::uint16_t temp;

    if ((static_cast<int>(opcode) & 0xC7) == 0x06) return executeMvi(opcode);
    if ((static_cast<int>(opcode) & 0xC0) == 0x40 && opcode != Opcode::HLT) return executeMov(opcode);

    switch (opcode) {
        case Opcode::NOP:
        case Opcode::NOP_08:
        case Opcode::NOP_10:
        case Opcode::NOP_18:
        case Opcode::NOP_20:
        case Opcode::NOP_28:
        case Opcode::NOP_30:
        case Opcode::NOP_38:
            break;
        case Opcode::LXI_B:
            setBC(fetchWord());
            break;
        case Opcode::LXI_D:
            setDE(fetchWord());
            break;
        case Opcode::LXI_H:
            setHL(fetchWord());
            break;
        case Opcode::LXI_SP:
            stackPointer = fetchWord();
            break;
        case Opcode::STAX_B:
            writeMemory(getBC(), regA);
            break;
        case Opcode::INX_B:
            setBC(getBC() + 1 & 0xFFFF);
            break;
        case Opcode::INR_B:
            updateFlagsInr(getBC() + 1 & 0xFF);
            break;
        case Opcode::DCR_B:
            updateFlagsDcr(getBC() + 1 & 0xFF);
            break;
        case Opcode::RLC:
            regA = (regA << 1 | regA >> 7) & 0xFF;
            carryFlag = (regA & 1) != 0;
            break;
        case Opcode::DAD_B:
            temp = getHL() + getBC();
            setHL(temp & 0xFFFF);
            carryFlag = temp > 0xFFFF;
            break;
        case Opcode::LDAX_B:
            regA = readMemory(getBC());
            break;
        case Opcode::HLT:
            halted = true;
            break;
        case Opcode::JMP:
        case Opcode::JMP_C8:
            programCounter = fetchWord();
            break;
        case Opcode::OUT:
            writeIO(fetchByte(), regA);
            break;
        case Opcode::IN:
            regA = readIO(fetchByte());
            break;
        case Opcode::DI:
            interruptEnable = false;
            break;
        case Opcode::EI:
            interruptEnable = true;
            break;
        default:
            halted = true;
            std::printf("unknown opcode: %02x\n", opcode);
            break;
    }

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

std::uint8_t I8080::packFlags() const {
    std::uint8_t f = 0;
    f |= signFlag ? 0x80 : 0x00;
    f |= zeroFlag ? 0x40 : 0x00;
    f |= auxCarryFlag ? 0x10 : 0x00;
    f |= parityFlag ? 0x04 : 0x00;
    f |= carryFlag ? 0x01 : 0x00;
    return f;
}

void I8080::dumpRegisters() const {
    printf("A=%02X B=%02X C=%02X D=%02X E=%02X H=%02X L=%02X SP=%04X PC=%04X F=%02X\n",
           regA, regB, regC, regD, regE, regH, regL, stackPointer, programCounter, packFlags());
}
