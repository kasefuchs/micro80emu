#include "I8080.h"

#include <cstdio>

I8080::I8080(ReadMem rm, WriteMem wm)
    : readMemory(std::move(rm)), writeMemory(std::move(wm)) {
}

int I8080::step() {
    if (halted) return executeOpcode(Opcode::NOP);
    return executeOpcode(static_cast<Opcode>(fetchByte()));
}

void I8080::jump(std::uint16_t addr) { programCounter = addr; }

void I8080::reset(std::uint16_t addr) {
    interruptEnable = false;
    halted = false;
    programCounter = addr;
}

std::uint16_t I8080::getProgramCounter() const { return programCounter; }

std::uint16_t I8080::getBC() const { return regB << 8 | regC; }
std::uint16_t I8080::getDE() const { return regD << 8 | regE; }
std::uint16_t I8080::getHL() const { return regH << 8 | regL; }

void I8080::setBC(std::uint16_t v) {
    regB = v >> 8;
    regC = v & 0xFF;
}

void I8080::setDE(std::uint16_t v) {
    regD = v >> 8;
    regE = v & 0xFF;
}

void I8080::setHL(std::uint16_t v) {
    regH = v >> 8;
    regL = v & 0xFF;
}

std::uint8_t I8080::fetchByte() {
    auto v = readMemory(programCounter);
    programCounter = programCounter + 1 & 0xFFFF;
    return v;
}

std::uint16_t I8080::fetchWord() {
    return fetchByte() | fetchByte() << 8;
}

void I8080::updateFlagsInr(std::uint8_t v) {
    auxCarryFlag = (v & 0x0F) == 0;
    signFlag = v & 0x80;
    zeroFlag = (v == 0);
    parityFlag = PARITY_TABLE[v];
}

void I8080::updateFlagsDcr(std::uint8_t v) {
    auxCarryFlag = (v & 0x0F) != 0x0F;
    signFlag = v & 0x80;
    zeroFlag = (v == 0);
    parityFlag = PARITY_TABLE[v];
}

int I8080::executeOpcode(Opcode opcode) {
    std::uint16_t temp;

    switch (opcode) {
        case Opcode::NOP:
        case Opcode::NOP08:
        case Opcode::NOP10:
        case Opcode::NOP18:
            break;
        case Opcode::LXI_B:
            setBC(fetchWord());
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
        case Opcode::MVI_B:
            fetchByte();
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
        default:
            std::printf("unknown opcode: %02x\n", opcode);
            halted = true;
            break;
    }

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

uint8_t I8080::packFlags() const {
    uint8_t f = 0;
    f |= signFlag ? 0x80 : 0x00;
    f |= zeroFlag ? 0x40 : 0x00;
    f |= auxCarryFlag ? 0x10 : 0x00;
    f |= parityFlag ? 0x04 : 0x00;
    f |= carryFlag ? 0x01 : 0x00;
    return f;
}

void I8080::dumpRegisters() const {
    printf("A=%02X B=%02X C=%02X D=%02X E=%02X H=%02X L=%02X SP=%04X PC=%04X F=%08X\n",
           regA, regB, regC, regD, regE, regH, regL, stackPointer, programCounter, packFlags());
}
