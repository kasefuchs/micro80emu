#ifndef MICRO80EMU_I8080_H
#define MICRO80EMU_I8080_H
#include <array>
#include <cstdint>
#include <functional>


class I8080 {
public:
    using byte = std::uint8_t;
    using word = std::uint16_t;
    using address = std::uint16_t;

    using ReadMemory = std::function<byte(address)>;
    using WriteMemory = std::function<void(address, byte)>;
    using ReadIO = std::function<byte(address)>;
    using WriteIO = std::function<void(address, byte)>;

    enum class Opcode : byte {
        NOP, LXI_B, STAX_B, INX_B, INR_B, DCR_B, MVI_B, RLC,
        NOP_08, DAD_B, LDAX_B, DCX_B, INR_C, DCR_C, MVI_C, RRC,
        NOP_10, LXI_D, STAX_D, INX_D, INR_D, DCR_D, MVI_D, RAL,
        NOP_18, DAD_D, LDAX_D, DCX_D, INR_E, DCR_E, MVI_E, RAR,
        NOP_20, LXI_H, SHLD, INX_H, INR_H, DCR_H, MVI_H, DAA,
        NOP_28, DAD_H, LHLD, DCH_X, INR_L, DCR_L, MVI_L, CMA,
        NOP_30, LXI_SP, STA, INX_SP, INR_M, DCR_M, MVI_M, STC,
        NOP_38, DAD_SP, LDA, DCX, INR_A, DCR_A, MVI_A, CMC,
        MOV_B_B, MOV_B_C, MOV_B_D, MOV_B_E, MOV_B_H, MOV_B_L, MOV_B_M, MOV_B_A,
        MOV_C_B, MOV_C_C, MOV_C_D, MOV_C_E, MOV_C_H, MOV_C_L, MOV_C_M, MOV_C_A,
        MOV_D_B, MOV_D_C, MOV_D_D, MOV_D_E, MOV_D_H, MOV_D_L, MOV_D_M, MOV_D_A,
        MOV_E_B, MOV_E_C, MOV_E_D, MOV_E_E, MOV_E_H, MOV_E_L, MOV_E_M, MOV_E_A,
        MOV_H_B, MOV_H_C, MOV_H_D, MOV_H_E, MOV_H_H, MOV_H_L, MOV_H_M, MOV_H_A,
        MOV_L_B, MOV_L_C, MOV_L_D, MOV_L_E, MOV_L_H, MOV_L_L, MOV_L_M, MOV_L_A,
        MOV_M_B, MOV_M_C, MOV_M_D, MOV_M_E, MOV_M_H, MOV_M_L, HLT, MOV_M_A,
        MOV_A_B, MOV_A_C, MOV_A_D, MOV_A_E, MOV_A_H, MOV_A_L, MOV_A_M, MOV_A_A,
        ADD_B, ADD_C, ADD_D, ADD_E, ADD_H, ADD_L, ADD_M, ADD_A,
        ADC_B, ADC_C, ADC_D, ADC_E, ADC_H, ADC_L, ADC_M, ADC_A,
        SUB_B, SUB_C, SUB_D, SUB_E, SUB_H, SUB_L, SUB_M, SUB_A,
        SBB_B, SBB_C, SBB_D, SBB_E, SBB_H, SBB_L, SBB_M, SBB_A,
        ANA_B, ANA_C, ANA_D, ANA_E, ANA_H, ANA_L, ANA_M, ANA_A,
        XRA_B, XRA_C, XRA_D, XRA_E, XRA_H, XRA_L, XRA_M, XRA_A,
        ORA_B, ORA_C, ORA_D, ORA_E, ORA_H, ORA_L, ORA_M, ORA_A,
        CMP_B, CMP_C, CMP_D, CMP_E, CMP_H, CMP_L, CMP_M, CMP_A,
        RNZ, POP_B, JNZ, JMP, CNZ, PUSH_B, ADI, RST_0,
        RZ, RET, JZ, JMP_C8, CZ, CALL, ACI, RST_1,
        RNC, POP_D, JNC, OUT, CNC, PUSH_D, SUI, RST_2,
        RC, RET_D9, JC, IN, CC, CALL_DD, SBI, RST_3,
        RPO, POP_H, JPO, XTHL, CPO, PUSH_H, ANI, RST_4,
        RPE, PCHL, JPE, XCHG, CPE, CALL_ED, XRI, RST_5,
        RP, POP_PSW, JP, DI, CP, PUSH_PSW, ORI, RST_6,
        RM, SPHL, JM, EI, CM, CALL_FD, CPI, RST_7,
    };

    I8080(ReadMemory rm, WriteMemory wm, ReadIO rio = nullptr, WriteIO wio = nullptr);

    int step();

    void jump(address addr);

    void reset(address addr = 0x0);

    [[nodiscard]] int getCycles() const;

    [[nodiscard]] bool isHalted() const;

    [[nodiscard]] address getProgramCounter() const;

private:
    static const int OPCODE_CYCLES[256];

    static const std::array<byte, 256> PARITY_TABLE;

    enum class Register : int { B, C, D, E, H, L, M, A };

    enum class RegisterPair : int { BC, DE, HL, SP_PSW };

    byte regA{}, regB{}, regC{}, regD{}, regE{}, regH{}, regL{};
    byte *regs[8];
    address stackPointer{}, programCounter{};

    bool signFlag{}, parityFlag{}, auxCarryFlag{}, zeroFlag{}, carryFlag{};
    bool interruptEnable{}, halted{};

    int cycles{};

    ReadMemory readMemory;
    WriteMemory writeMemory;
    ReadIO readIO;
    WriteIO writeIO;

    [[nodiscard]] word getBC() const;

    [[nodiscard]] word getDE() const;

    [[nodiscard]] word getHL() const;

    void setBC(word v);

    void setDE(word v);

    void setHL(word v);

    byte popCommandByte();

    word popCommandWord();

    word readMemoryWord(address addr) const;

    static Register getDestFromOpcode(Opcode opcode);

    static Register getSrcFromOpcode(Opcode opcode);

    static RegisterPair getRegisterPairFromOpcode(Opcode opcode);

    byte readRegOrMemory(Register reg) const;

    void writeRegOrMemory(Register reg, byte value) const;

    void writeMemoryWord(address addr, word v) const;

    void pushStack(word v);

    word popStack();

    byte getByteFromFlags() const;

    void setFlagsFromByte(byte field);

    void addWithFlags(byte value, bool withCarry);

    void subtractWithFlags(byte value, bool withBorrow);

    int executeMove(Opcode opcode) const;

    int executeDecrement(Opcode opcode);

    int executeImmediateMove(Opcode opcode);

    int executeCompare(Opcode opcode);

    int executeLogical(Opcode opcode);

    int executeImmediateLogical(Opcode opcode);

    int executeSubtract(Opcode opcode);

    int executeAdd(Opcode opcode);

    int executeIncrement(Opcode opcode);

    int executeIncrementPair(Opcode opcode);

    int executePush(Opcode opcode);

    int executeConditionalCall(bool condition);

    int executeDoubleAdd(Opcode opcode);

    int executeConditionalJump(bool condition);

    int executePop(Opcode opcode);

    int executeConditionalReturn(bool condition);

    int executeReset(Opcode opcode);

    int executeOpcode(Opcode opcode);
};


#endif //MICRO80EMU_I8080_H
