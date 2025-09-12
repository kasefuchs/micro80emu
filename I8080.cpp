#include "I8080.h"

#include <cstdio>
#include <iostream>

const std::array<I8080::byte, 256> I8080::PARITY_TABLE = {
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
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
    4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,
    4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    0, 10, 10, 10, 0, 11, 7, 11, 0, 10, 10, 10, 0, 17, 7, 11,
    0, 10, 10, 10, 0, 11, 7, 11, 0, 10, 10, 10, 0, 17, 7, 11,
    0, 10, 10, 18, 0, 11, 7, 11, 0, 5, 10, 5, 0, 17, 7, 11,
    0, 10, 10, 4, 0, 11, 7, 11, 0, 5, 10, 4, 0, 17, 7, 11,
};

I8080::I8080(ReadMemory rm, WriteMemory wm, ReadIO rio, WriteIO wio)
    : regs{&regB, &regC, &regD, &regE, &regH, &regL, nullptr, &regA},
      readMemory(std::move(rm)), writeMemory(std::move(wm)),
      readIO(std::move(rio)), writeIO(std::move(wio)) {
}

int I8080::step() {
    return cycles += halted ? executeOpcode(Opcode::NOP) : executeOpcode(static_cast<Opcode>(popCommandByte()));
}

void I8080::jump(const address addr) { programCounter = addr; }

void I8080::reset(const address addr) {
    interruptEnable = false;
    halted = false;
    programCounter = addr;
}

int I8080::getCycles() const { return cycles; }
bool I8080::isHalted() const { return halted; }
I8080::address I8080::getProgramCounter() const { return programCounter; }

I8080::word I8080::getBC() const { return regB << 8 | regC; }
I8080::word I8080::getDE() const { return regD << 8 | regE; }
I8080::word I8080::getHL() const { return regH << 8 | regL; }

void I8080::setBC(const word v) {
    regB = v >> 8;
    regC = v & 0xFF;
}

void I8080::setDE(const word v) {
    regD = v >> 8;
    regE = v & 0xFF;
}

void I8080::setHL(const word v) {
    regH = v >> 8;
    regL = v & 0xFF;
}

I8080::byte I8080::popCommandByte() {
    const byte v = readMemory(programCounter);
    programCounter = programCounter + 1 & 0xFFFF;
    return v;
}

I8080::word I8080::popCommandWord() {
    return popCommandByte() | popCommandByte() << 8;
}

I8080::word I8080::readMemoryWord(const address addr) const {
    return readMemory(addr) | readMemory(addr + 1 & 0xFFFF) << 8;
}

int I8080::getDestFromOpcode(Opcode opcode) {
    return static_cast<int>(opcode) >> 3 & 0x07;
}

int I8080::getSrcFromOpcode(Opcode opcode) {
    return static_cast<int>(opcode) & 0x07;
}

I8080::RegisterPair I8080::getRegisterPairFromOpcode(Opcode opcode) {
    return static_cast<RegisterPair>(static_cast<int>(opcode) >> 4 & 0x03);
}

I8080::byte I8080::readRegOrMemory(const int code) const {
    if (code == REG_INDEX_M) return readMemory(getHL());
    return *regs[code];
}

void I8080::writeRegOrMemory(const int code, const byte value) const {
    if (code == REG_INDEX_M) writeMemory(getHL(), value);
    else *regs[code] = value;
}

void I8080::writeMemoryWord(const address addr, const word v) const {
    writeMemory(addr, v & 0xFF);
    writeMemory(addr + 1 & 0xFFFF, v >> 8);
}

void I8080::pushStack(const word v) {
    stackPointer = stackPointer - 2 & 0xFFFF;
    writeMemoryWord(stackPointer, v);
}

I8080::word I8080::popStack() {
    const word v = readMemoryWord(stackPointer);
    stackPointer = stackPointer + 2 & 0xFFFF;
    return v;
}

void I8080::updateFlagsAfterIncrease(const byte v) {
    auxCarryFlag = (v & 0x0F) == 0;
    signFlag = v & 0x80;
    zeroFlag = v == 0;
    parityFlag = PARITY_TABLE[v];
}

void I8080::updateFlagsAfterDecrease(const byte v) {
    auxCarryFlag = (v & 0x0F) != 0x0F;
    signFlag = v & 0x80;
    zeroFlag = v == 0;
    parityFlag = PARITY_TABLE[v];
}

I8080::byte I8080::getByteFromFlags() const {
    byte field = 2;
    field |= signFlag ? 0x80 : 0x00;
    field |= zeroFlag ? 0x40 : 0x00;
    field |= auxCarryFlag ? 0x10 : 0x00;
    field |= parityFlag ? 0x04 : 0x00;
    field |= carryFlag ? 0x01 : 0x00;
    return field;
}

void I8080::setFlagsFromByte(const byte field) {
    signFlag = (field & 0x80) != 0;
    zeroFlag = (field & 0x40) != 0;
    auxCarryFlag = (field & 0x10) != 0;
    parityFlag = (field & 0x04) != 0;
    carryFlag = (field & 0x01) != 0;
}

int I8080::executeMove(const Opcode opcode) const {
    const int src = getSrcFromOpcode(opcode);
    const int dest = getDestFromOpcode(opcode);

    writeRegOrMemory(dest, readRegOrMemory(src));

    return dest == REG_INDEX_M ? 7 : 5;
}

int I8080::executeDecrement(const Opcode opcode) {
    const int dest = getDestFromOpcode(opcode);

    const byte oldValue = readRegOrMemory(dest);
    const byte newValue = oldValue - 1;

    auxCarryFlag = (oldValue & 0x0F) == 0x00;
    signFlag = (newValue & 0x80) != 0;
    zeroFlag = newValue == 0;
    parityFlag = PARITY_TABLE[newValue];

    writeRegOrMemory(dest, newValue);

    return dest == REG_INDEX_M ? 10 : 5;
}

int I8080::executeMoveImmediate(const Opcode opcode) {
    const int dest = getDestFromOpcode(opcode);
    const byte value = popCommandByte();

    writeRegOrMemory(dest, value);

    return dest == REG_INDEX_M ? 10 : 7;
}

int I8080::executeCompare(const Opcode opcode) {
    const byte temp = regA;
    const int src = getSrcFromOpcode(opcode);
    const byte value = readRegOrMemory(src);

    auxCarryFlag = (regA & 0x0F) >= ((value & 0x0F));
    const int diff = regA - value;
    regA = diff & 0xFF;
    carryFlag = (diff & 0x100) != 0;
    signFlag = (regA & 0x80) != 0;
    zeroFlag = (regA == 0);
    parityFlag = PARITY_TABLE[regA];

    regA = temp;

    return src == REG_INDEX_M ? 7 : 4;
}

int I8080::executeOr(const Opcode opcode) {
    const int src = getSrcFromOpcode(opcode);
    const byte value = readRegOrMemory(src);

    regA |= value;

    carryFlag = false;
    auxCarryFlag = false;
    signFlag = (regA & 0x80) != 0;
    zeroFlag = regA == 0;
    parityFlag = PARITY_TABLE[regA];

    return src == REG_INDEX_M ? 7 : 4;
}

int I8080::executeSubtractWithBorrow(const Opcode opcode) {
    const int src = getSrcFromOpcode(opcode);
    const byte value = readRegOrMemory(src);

    auxCarryFlag = (regA & 0x0F) >= (value & 0x0F) - (carryFlag ? 1 : 0);
    const int diff = regA - value - (carryFlag ? 1 : 0);
    regA = diff & 0xFF;
    carryFlag = (diff & 0x100) != 0;
    signFlag = (regA & 0x80) != 0;
    zeroFlag = regA == 0;
    parityFlag = PARITY_TABLE[regA];

    return src == REG_INDEX_M ? 7 : 4;
}

int I8080::executeIncrementPair(const Opcode opcode) {
    switch (getRegisterPairFromOpcode(opcode)) {
        case RegisterPair::BC:
            setBC(getBC() + 1 & 0xFFFF);
            break;
        case RegisterPair::DE:
            setDE(getDE() + 1 & 0xFFFF);
            break;
        case RegisterPair::HL:
            setHL(getHL() + 1 & 0xFFFF);
            break;
        case RegisterPair::SP_PSW:
            stackPointer = stackPointer + 1 & 0xFFFF;
            break;
    }

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executePush(const Opcode opcode) {
    word value = 0;

    switch (getRegisterPairFromOpcode(opcode)) {
        case RegisterPair::BC:
            value = getBC();
            break;
        case RegisterPair::DE:
            value = getDE();
            break;
        case RegisterPair::HL:
            value = getHL();
            break;
        case RegisterPair::SP_PSW:
            value = regA << 8 | getByteFromFlags();
            break;
    }

    pushStack(value);

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executePop(const Opcode opcode) {
    const word value = popStack();

    switch (getRegisterPairFromOpcode(opcode)) {
        case RegisterPair::BC:
            setBC(value);
            break;
        case RegisterPair::DE:
            setDE(value);
            break;
        case RegisterPair::HL:
            setHL(value);
            break;
        case RegisterPair::SP_PSW:
            regA = value >> 8;
            setFlagsFromByte(value & 0xFF);
            break;
    }

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeDoubleAdd(const Opcode opcode) {
    word value = 0;

    switch (getRegisterPairFromOpcode(opcode)) {
        case RegisterPair::BC:
            value = getBC();
            break;
        case RegisterPair::DE:
            value = getDE();
            break;
        case RegisterPair::HL:
            value = getHL();
            break;
        case RegisterPair::SP_PSW:
            value = stackPointer;
            break;
    }

    const word result = getHL() + value;
    setHL(result);
    carryFlag = result > 0xFFFF;

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeConditionalJump(const Opcode opcode, const bool condition) {
    const address addr = popCommandWord();
    if (condition) programCounter = addr;

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeOpcode(Opcode opcode) {
    printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", cycles, opcode, programCounter, stackPointer, regB,
           regC, regD,
           regE, regH, regL, regA, signFlag, parityFlag, auxCarryFlag, zeroFlag, carryFlag);

    if ((static_cast<int>(opcode) & 0xCF) == 0x03) return executeIncrementPair(opcode);
    if ((static_cast<int>(opcode) & 0xCF) == 0x09) return executeDoubleAdd(opcode);
    if ((static_cast<int>(opcode) & 0xC7) == 0x05) return executeDecrement(opcode);
    if ((static_cast<int>(opcode) & 0xF8) == 0xB8) return executeCompare(opcode);
    if ((static_cast<int>(opcode) & 0xC7) == 0x06) return executeMoveImmediate(opcode);
    if ((static_cast<int>(opcode) & 0xC0) == 0x40 && opcode != Opcode::HLT) return executeMove(opcode);
    if ((static_cast<int>(opcode) & 0xF8) == 0xB0) return executeOr(opcode);
    if ((static_cast<int>(opcode) & 0xCF) == 0xC5) return executePush(opcode);
    if ((static_cast<int>(opcode) & 0xCF) == 0xC1) return executePop(opcode);
    if ((static_cast<int>(opcode) & 0xF8) == 0x98) return executeSubtractWithBorrow(opcode);

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
        case Opcode::STA:
            writeMemory(popCommandWord(), regA);
            break;
        case Opcode::LXI_B:
            setBC(popCommandWord());
            break;
        case Opcode::LXI_D:
            setDE(popCommandWord());
            break;
        case Opcode::LXI_H:
            setHL(popCommandWord());
            break;
        case Opcode::LXI_SP:
            stackPointer = popCommandWord();
            break;
        case Opcode::STAX_B:
            writeMemory(getBC(), regA);
            break;
        case Opcode::STAX_D:
            writeMemory(getDE(), regA);
            break;
        case Opcode::LHLD:
            setHL(readMemoryWord(popCommandWord()));
            break;
        case Opcode::LDA:
            regA = readMemory(popCommandWord());
            break;
        case Opcode::CPI: {
            const byte value = popCommandByte();
            const byte result = regA - value;

            carryFlag = regA < value;
            auxCarryFlag = ((regA ^ value ^ result) & 0x10) != 0;
            signFlag = (result & 0x80) != 0;
            zeroFlag = result == 0;
            parityFlag = PARITY_TABLE[result];

            return 7;
        }
        case Opcode::INR_B:
            updateFlagsAfterIncrease(getBC() + 1 & 0xFF);
            break;
        case Opcode::DCR_B:
            updateFlagsAfterDecrease(getBC() + 1 & 0xFF);
            break;
        case Opcode::RLC:
            regA = (regA << 1 | regA >> 7) & 0xFF;
            carryFlag = (regA & 1) != 0;
            break;
        case Opcode::DAD_B: {
            const word temp = getHL() + getBC();
            setHL(temp & 0xFFFF);
            carryFlag = temp > 0xFFFF;
            break;
        }
        case Opcode::LDAX_B:
            regA = readMemory(getBC());
            break;
        case Opcode::HLT:
            halted = true;
            break;
        case Opcode::JMP:
        case Opcode::JMP_C8:
            return executeConditionalJump(opcode, true);
        case Opcode::JNZ:
            return executeConditionalJump(opcode, !zeroFlag);
        case Opcode::JZ:
            return executeConditionalJump(opcode, zeroFlag);
        case Opcode::JNC:
            return executeConditionalJump(opcode, !carryFlag);
        case Opcode::JC:
            return executeConditionalJump(opcode, carryFlag);
        case Opcode::JPO:
            return executeConditionalJump(opcode, !parityFlag);
        case Opcode::JPE:
            return executeConditionalJump(opcode, parityFlag);
        case Opcode::JP:
            return executeConditionalJump(opcode, !signFlag);
        case Opcode::JM:
            return executeConditionalJump(opcode, signFlag);
        case Opcode::CALL:
        case Opcode::CALL_DD:
        case Opcode::CALL_ED:
        case Opcode::CALL_FD: {
            const word temp = popCommandWord();
            pushStack(programCounter);
            programCounter = temp;
            break;
        }
        case Opcode::RZ:
            if (!zeroFlag) return 5;
            programCounter = popStack();
            return 11;
        case Opcode::RNZ:
            if (zeroFlag) return 5;
            programCounter = popStack();
            return 11;
        case Opcode::RET:
        case Opcode::RET_D9:
            programCounter = popStack();
            break;
        case Opcode::ANI:
            regA &= popCommandByte();
            auxCarryFlag = false;
            signFlag = (regA & 0x80) != 0;
            zeroFlag = regA == 0;
            parityFlag = PARITY_TABLE[regA];
            carryFlag = false;
            break;
        case Opcode::RM:
            if (!signFlag) return 5;
            programCounter = popStack();
            return 11;
        case Opcode::SPHL:
            stackPointer = getHL();
            break;
        case Opcode::OUT:
            writeIO(popCommandByte(), regA);
            break;
        case Opcode::IN:
            regA = readIO(popCommandByte());
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
