#include "I8080.h"

#include <cstdio>

const int I8080::PARITY_TABLE[256] = {
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

I8080::I8080(Core::ReadMemory rm, Core::WriteMemory wm, Core::ReadMemory rio, Core::WriteMemory wio)
    : readMemory(std::move(rm)), writeMemory(std::move(wm)),
      readIO(std::move(rio)), writeIO(std::move(wio)),
      regs{&regB, &regC, &regD, &regE, &regH, &regL, nullptr, &regA} {
}

long int I8080::step() {
    return cycles += halted ? executeOpcode(Opcode::NOP) : executeOpcode(static_cast<Opcode>(popCommandByte()));
}

void I8080::reset(const Core::address addr) {
    halted = false;
    interruptEnable = false;
    programCounter = addr;
}

bool I8080::isHalted() const { return halted; }

long int I8080::getCycles() const { return cycles; }

Core::address I8080::getProgramCounter() const { return programCounter; }

Core::byte I8080::popCommandByte() {
    const Core::byte v = readMemory(programCounter);
    programCounter = programCounter + 1 & 0xFFFF;
    return v;
}

Core::word I8080::popCommandWord() {
    return popCommandByte() | popCommandByte() << 8;
}

Core::word I8080::readMemoryWord(const Core::address addr) const {
    return readMemory(addr) | readMemory(addr + 1 & 0xFFFF) << 8;
}

I8080::Register I8080::GetDestinationFromOpcode(Opcode opcode) {
    return static_cast<Register>(static_cast<int>(opcode) >> 3 & 0x07);
}

I8080::Register I8080::GetSourceFromOpcode(Opcode opcode) {
    return static_cast<Register>(static_cast<int>(opcode) & 0x07);
}

I8080::RegisterPair I8080::GetRegisterPairFromOpcode(Opcode opcode) {
    return static_cast<RegisterPair>(static_cast<int>(opcode) >> 4 & 0x03);
}

Core::byte I8080::readRegisterOrMemory(const Register reg) const {
    if (reg == Register::M) return readMemory(readRegisterPair(RegisterPair::HL));
    return *regs[static_cast<int>(reg)];
}

void I8080::writeRegisterOrMemory(const Register reg, const Core::byte value) const {
    if (reg == Register::M) writeMemory(readRegisterPair(RegisterPair::HL), value);
    else *regs[static_cast<int>(reg)] = value;
}

Core::word I8080::readRegisterPair(const RegisterPair pair) const {
    if (pair == RegisterPair::SP_PSW) return stackPointer;

    const int idx = static_cast<int>(pair) * 2;
    return *regs[idx] << 8 | *regs[idx + 1];
}

void I8080::writeRegisterPair(const RegisterPair pair, const Core::word value) {
    if (pair == RegisterPair::SP_PSW) {
        stackPointer = value;
        return;
    }

    const int idx = static_cast<int>(pair) * 2;
    *regs[idx] = static_cast<Core::byte>(value >> 8);
    *regs[idx + 1] = static_cast<Core::byte>(value & 0xFF);
}

void I8080::writeMemoryWord(const Core::address addr, const Core::word v) const {
    writeMemory(addr, v & 0xFF);
    writeMemory(addr + 1 & 0xFFFF, v >> 8);
}

void I8080::pushStack(const Core::word v) {
    stackPointer = stackPointer - 2 & 0xFFFF;
    writeMemoryWord(stackPointer, v);
}

Core::word I8080::popStack() {
    const Core::word v = readMemoryWord(stackPointer);
    stackPointer = stackPointer + 2 & 0xFFFF;
    return v;
}

Core::byte I8080::getByteFromFlags() const {
    Core::byte field = 2;
    field |= signFlag ? 0x80 : 0x00;
    field |= zeroFlag ? 0x40 : 0x00;
    field |= auxCarryFlag ? 0x10 : 0x00;
    field |= parityFlag ? 0x04 : 0x00;
    field |= carryFlag ? 0x01 : 0x00;
    return field;
}

void I8080::setFlagsFromByte(const Core::byte field) {
    signFlag = (field & 0x80) != 0;
    zeroFlag = (field & 0x40) != 0;
    auxCarryFlag = (field & 0x10) != 0;
    parityFlag = (field & 0x04) != 0;
    carryFlag = (field & 0x01) != 0;
}

void I8080::addWithFlags(const Core::byte value, const bool withCarry) {
    const int carry = withCarry && carryFlag ? 1 : 0;

    auxCarryFlag = (regA & 0x0F) + (value & 0x0F) + carry > 0x0F;

    const int sum = regA + value + carry;
    regA = sum & 0xFF;

    carryFlag = (sum & 0x100) != 0;
    signFlag = (regA & 0x80) != 0;
    zeroFlag = regA == 0;
    parityFlag = PARITY_TABLE[regA];
}

void I8080::subtractWithFlags(const Core::byte value, const bool withBorrow) {
    const int borrow = withBorrow && carryFlag ? 1 : 0;

    auxCarryFlag = (regA & 0x0F) >= (value & 0x0F) + borrow;

    const int diff = regA - value - borrow;
    regA = diff & 0xFF;

    carryFlag = (diff & 0x100) != 0;
    signFlag = (regA & 0x80) != 0;
    zeroFlag = regA == 0;
    parityFlag = PARITY_TABLE[regA];
}

int I8080::executeMove(const Opcode opcode) const {
    const Register src = GetSourceFromOpcode(opcode);
    const Register dest = GetDestinationFromOpcode(opcode);

    writeRegisterOrMemory(dest, readRegisterOrMemory(src));

    return src == Register::M || dest == Register::M ? 7 : 5;
}

int I8080::executeDecrement(const Opcode opcode) {
    const Register dest = GetDestinationFromOpcode(opcode);

    const int value = readRegisterOrMemory(dest) - 1 & 0xFF;
    writeRegisterOrMemory(dest, value);

    auxCarryFlag = (value & 0x0F) != 0x0F;
    signFlag = (value & 0x80) != 0;
    zeroFlag = value == 0;
    parityFlag = PARITY_TABLE[value];

    return dest == Register::M ? 10 : 5;
}

int I8080::executeDecrementPair(const Opcode opcode) {
    const RegisterPair pair = GetRegisterPairFromOpcode(opcode);
    const Core::word value = readRegisterPair(pair);

    writeRegisterPair(pair, value - 1 & 0xFFFF);

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeImmediateMove(const Opcode opcode) {
    const Register dest = GetDestinationFromOpcode(opcode);
    const Core::byte value = popCommandByte();

    writeRegisterOrMemory(dest, value);

    return dest == Register::M ? 10 : 7;
}

int I8080::executeImmediateLoadPair(const Opcode opcode) {
    const Core::word value = popCommandWord();
    const RegisterPair pair = GetRegisterPairFromOpcode(opcode);

    writeRegisterPair(pair, value);

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeCompare(const Opcode opcode) {
    const Register src = GetSourceFromOpcode(opcode);
    const Core::byte value = readRegisterOrMemory(src);

    const Core::byte temp = regA;
    subtractWithFlags(value, false);
    regA = temp;

    return src == Register::M ? 7 : 4;
}

int I8080::executeLogical(const Opcode opcode) {
    const Register src = GetSourceFromOpcode(opcode);
    const Core::byte value = readRegisterOrMemory(src);

    // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
    switch (static_cast<int>(opcode) & 0xF8) {
        case 0xA0: // ANA
            regA &= value;
            auxCarryFlag = ((regA | value) & 0x08) != 0;
            break;
        case 0xA8: // XRA
            regA ^= value;
            auxCarryFlag = false;
            break;
        case 0xB0: // ORA
            regA |= value;
            auxCarryFlag = false;
            break;
    }

    carryFlag = false;
    signFlag = (regA & 0x80) != 0;
    zeroFlag = regA == 0;
    parityFlag = PARITY_TABLE[regA];

    return src == Register::M ? 7 : 4;
}

int I8080::executeImmediateLogical(const Opcode opcode) {
    const Core::byte value = popCommandByte();

    // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
    switch (opcode) {
        case Opcode::ANI:
            regA &= value;
            auxCarryFlag = ((regA | value) & 0x08) != 0;
            break;
        case Opcode::XRI:
            regA ^= value;
            auxCarryFlag = false;
            break;
        case Opcode::ORI:
            regA |= value;
            auxCarryFlag = false;
            break;
    }

    carryFlag = false;
    signFlag = (regA & 0x80) != 0;
    zeroFlag = regA == 0;
    parityFlag = PARITY_TABLE[regA];

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeSubtract(const Opcode opcode) {
    const Register src = GetSourceFromOpcode(opcode);
    const Core::byte value = readRegisterOrMemory(src);

    const bool withBorrow = (static_cast<int>(opcode) & 0x08) != 0;

    subtractWithFlags(value, withBorrow);

    return src == Register::M ? 7 : 4;
}

int I8080::executeAdd(const Opcode opcode) {
    const Register src = GetSourceFromOpcode(opcode);
    const Core::byte value = readRegisterOrMemory(src);

    const bool withCarry = (static_cast<int>(opcode) & 0x08) != 0;

    addWithFlags(value, withCarry);

    return src == Register::M ? 7 : 4;
}

int I8080::executeIncrement(const Opcode opcode) {
    const Register dest = GetDestinationFromOpcode(opcode);
    const Core::byte value = readRegisterOrMemory(dest);
    const Core::byte result = value + 1 & 0xFF;

    writeRegisterOrMemory(dest, result);

    auxCarryFlag = (result & 0x0F) == 0;
    signFlag = (result & 0x80) != 0;
    zeroFlag = result == 0;
    parityFlag = PARITY_TABLE[result];

    return dest == Register::M ? 10 : 5;
}

int I8080::executeIncrementPair(const Opcode opcode) {
    const RegisterPair pair = GetRegisterPairFromOpcode(opcode);
    const Core::word value = readRegisterPair(pair);

    writeRegisterPair(pair, value + 1 & 0xFFFF);

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executePush(const Opcode opcode) {
    const RegisterPair pair = GetRegisterPairFromOpcode(opcode);

    Core::word value = 0;
    if (pair != RegisterPair::SP_PSW) value = readRegisterPair(pair);
    else value = regA << 8 | getByteFromFlags();

    pushStack(value);

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executePop(const Opcode opcode) {
    const Core::word value = popStack();
    const RegisterPair pair = GetRegisterPairFromOpcode(opcode);

    if (pair != RegisterPair::SP_PSW) writeRegisterPair(pair, value);
    else {
        regA = value >> 8;
        setFlagsFromByte(value & 0xFF);
    }

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeDoubleAdd(const Opcode opcode) {
    const RegisterPair pair = GetRegisterPairFromOpcode(opcode);
    const Core::word value = readRegisterPair(pair);

    const int result = readRegisterPair(RegisterPair::HL) + value;
    writeRegisterPair(RegisterPair::HL, result);
    carryFlag = result > 0xFFFF;

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeConditionalJump(const bool condition) {
    const Core::address addr = popCommandWord();
    if (condition) programCounter = addr;

    return 10;
}

int I8080::executeConditionalCall(const bool condition) {
    const Core::word value = popCommandWord();
    if (!condition) return 11;
    pushStack(programCounter);
    programCounter = value;
    return 17;
}

int I8080::executeConditionalReturn(const bool condition) {
    if (!condition) return 5;
    programCounter = popStack();
    return 11;
}

int I8080::executeReset(const Opcode opcode) {
    const int index = static_cast<int>(opcode) >> 3 & 0x07;
    const Core::address addr = index * 0x8;

    pushStack(programCounter);
    programCounter = addr;

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}

int I8080::executeOpcode(Opcode opcode) {
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
        case Opcode::CMA:
            regA ^= 0xFF;
            break;
        case Opcode::CMC:
            carryFlag = !carryFlag;
            break;
        case Opcode::RRC:
            regA = (regA >> 1 | regA << 7) & 0xFF;
            carryFlag = (regA & 0x80) != 0;
            break;
        case Opcode::RAR: {
            const Core::byte temp = regA;
            regA = (regA >> 1 | (carryFlag ? 0x80 : 0)) & 0xFF;
            carryFlag = (temp & 0x01) != 0;
            break;
        }
        case Opcode::RAL: {
            const Core::byte temp = regA;
            regA = (regA << 1 | (carryFlag ? 0x01 : 0)) & 0xFF;
            carryFlag = (temp & 0x80) != 0;
            break;
        }
        case Opcode::STC:
            carryFlag = true;
            break;
        case Opcode::STAX_B:
            writeMemory(readRegisterPair(RegisterPair::BC), regA);
            break;
        case Opcode::STAX_D:
            writeMemory(readRegisterPair(RegisterPair::DE), regA);
            break;
        case Opcode::LHLD:
            writeRegisterPair(RegisterPair::HL, readMemoryWord(popCommandWord()));
            break;
        case Opcode::SHLD:
            writeMemoryWord(popCommandWord(), readRegisterPair(RegisterPair::HL));
            break;
        case Opcode::LDA:
            regA = readMemory(popCommandWord());
            break;
        case Opcode::CPI: {
            const Core::byte value = popCommandByte();
            const Core::byte temp = regA;
            subtractWithFlags(value, false);
            regA = temp;
            break;
        }
        case Opcode::SUI: {
            const Core::byte value = popCommandByte();
            subtractWithFlags(value, false);
            break;
        }
        case Opcode::SBI: {
            const Core::byte value = popCommandByte();
            subtractWithFlags(value, true);
            break;
        }
        case Opcode::XCHG: {
            Core::byte temp = regL;
            regL = regE;
            regE = temp;
            temp = regH;
            regH = regD;
            regD = temp;
            break;
        }
        case Opcode::ADI: {
            const Core::byte value = popCommandByte();
            addWithFlags(value, false);
            break;
        }
        case Opcode::ACI: {
            const Core::byte value = popCommandByte();
            addWithFlags(value, true);
            break;
        }
        case Opcode::RLC:
            regA = (regA << 1 | regA >> 7) & 0xFF;
            carryFlag = (regA & 1) != 0;
            break;
        case Opcode::LDAX_B:
            regA = readMemory(readRegisterPair(RegisterPair::BC));
            break;
        case Opcode::LDAX_D:
            regA = readMemory(readRegisterPair(RegisterPair::DE));
            break;
        case Opcode::HLT:
            halted = true;
            break;
        case Opcode::XTHL: {
            const Core::word value = readMemoryWord(stackPointer);
            writeMemoryWord(stackPointer, readRegisterPair(RegisterPair::HL));
            writeRegisterPair(RegisterPair::HL, value);
            break;
        }
        case Opcode::SPHL:
            stackPointer = readRegisterPair(RegisterPair::HL);
            break;
        case Opcode::PCHL:
            programCounter = readRegisterPair(RegisterPair::HL);
            break;

        // Jumps.
        case Opcode::JMP:
        case Opcode::JMP_C8:
            return executeConditionalJump(true);
        case Opcode::JNZ:
            return executeConditionalJump(!zeroFlag);
        case Opcode::JZ:
            return executeConditionalJump(zeroFlag);
        case Opcode::JNC:
            return executeConditionalJump(!carryFlag);
        case Opcode::JC:
            return executeConditionalJump(carryFlag);
        case Opcode::JPO:
            return executeConditionalJump(!parityFlag);
        case Opcode::JPE:
            return executeConditionalJump(parityFlag);
        case Opcode::JM:
            return executeConditionalJump(signFlag);
        case Opcode::JP:
            return executeConditionalJump(!signFlag);

        // Calls.
        case Opcode::CALL:
        case Opcode::CALL_DD:
        case Opcode::CALL_ED:
        case Opcode::CALL_FD: {
            const Core::word temp = popCommandWord();
            pushStack(programCounter);
            programCounter = temp;
            break;
        }
        case Opcode::CNZ:
            return executeConditionalCall(!zeroFlag);
        case Opcode::CZ:
            return executeConditionalCall(zeroFlag);
        case Opcode::CNC:
            return executeConditionalCall(!carryFlag);
        case Opcode::CC:
            return executeConditionalCall(carryFlag);
        case Opcode::CPO:
            return executeConditionalCall(parityFlag);
        case Opcode::CPE:
            return executeConditionalCall(!parityFlag);
        case Opcode::CP:
            return executeConditionalCall(signFlag);
        case Opcode::CM:
            return executeConditionalCall(!signFlag);

        // Returns.
        case Opcode::RET:
        case Opcode::RET_D9:
            programCounter = popStack();
            break;
        case Opcode::RNZ:
            return executeConditionalReturn(!zeroFlag);
        case Opcode::RZ:
            return executeConditionalReturn(zeroFlag);
        case Opcode::RNC:
            return executeConditionalReturn(!carryFlag);
        case Opcode::RC:
            return executeConditionalReturn(carryFlag);
        case Opcode::RPO:
            return executeConditionalReturn(!parityFlag);
        case Opcode::RPE:
            return executeConditionalReturn(parityFlag);
        case Opcode::RP:
            return executeConditionalReturn(!signFlag);
        case Opcode::RM:
            return executeConditionalReturn(signFlag);

        // I/O.
        case Opcode::IN:
            regA = readIO(popCommandByte());
            break;
        case Opcode::OUT:
            writeIO(popCommandByte(), regA);
            break;

        // Interrupts.
        case Opcode::DI:
            interruptEnable = false;
            break;
        case Opcode::EI:
            interruptEnable = true;
            break;

        default:
            // ADD.
            if ((static_cast<int>(opcode) & 0xF0) == 0x80) return executeAdd(opcode);

            // SUB.
            if ((static_cast<int>(opcode) & 0xF0) == 0x90) return executeSubtract(opcode);

            // INR.
            if ((static_cast<int>(opcode) & 0xC7) == 0x04) return executeIncrement(opcode);

            // INX.
            if ((static_cast<int>(opcode) & 0xCF) == 0x03) return executeIncrementPair(opcode);

            // DAD.
            if ((static_cast<int>(opcode) & 0xCF) == 0x09) return executeDoubleAdd(opcode);

            // DCR.
            if ((static_cast<int>(opcode) & 0xC7) == 0x05) return executeDecrement(opcode);

            // DCX.
            if ((static_cast<int>(opcode) & 0xCF) == 0x0B) return executeDecrementPair(opcode);

            // CMP.
            if ((static_cast<int>(opcode) & 0xF8) == 0xB8) return executeCompare(opcode);

            // PUSH.
            if ((static_cast<int>(opcode) & 0xCF) == 0xC5) return executePush(opcode);

            // POP.
            if ((static_cast<int>(opcode) & 0xCF) == 0xC1) return executePop(opcode);

            // MVI.
            if ((static_cast<int>(opcode) & 0xC7) == 0x06) return executeImmediateMove(opcode);

            // RST.
            if ((static_cast<int>(opcode) & 0xC7) == 0xC7) return executeReset(opcode);

            // MOV.
            if ((static_cast<int>(opcode) & 0xC0) == 0x40 && opcode != Opcode::HLT) {
                return executeMove(opcode);
            }

            // LXI.
            if ((static_cast<int>(opcode) & 0x0F) == 0x01) return executeImmediateLoadPair(opcode);

            // ANI / XRI / ORI.
            if ((static_cast<int>(opcode) & 0xE7) == 0xE6 && opcode != Opcode::CPI) {
                return executeImmediateLogical(opcode);
            }

            // ANA / XRA / ORA.
            if (opcode >= Opcode::ANA_B && opcode <= Opcode::ORA_A) return executeLogical(opcode);

            // Halt.
            halted = true;
            std::printf("unknown opcode: %02x\n", opcode);
            break;
    }

    return OPCODE_CYCLES[static_cast<int>(opcode)];
}
