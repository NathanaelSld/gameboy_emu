#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>


typedef enum Reg
{
    A,
    B,
    C,
    D,
    E,
    H,
    L
} reg_8bits_t;

typedef enum Flag
{
    ZERO,
    SUB,
    HALF_CARRY,
    CARRY
} flag_t;



typedef struct Registers
{
    uint16_t *AF;
    uint16_t *BC;
    uint16_t *DE;
    uint16_t *HL;
    uint16_t *SP;
    uint16_t *PC;
} registers_t;




typedef struct cpu
{
    registers_t *registers;
    uint8_t *memorybus;
    uint16_t PC;
    uint16_t SP;
    bool IME;
    bool IME_pending;
} cpu_t;


typedef struct OperationResult
{
    uint16_t result;
    bool carry;
    bool halfcarry;
} operation_result_t;


uint8_t get_8bit_register(cpu_t *cpu, reg_8bits_t reg_8bits);
void set_8bit_register(cpu_t *cpu, reg_8bits_t reg_8bits, uint8_t value);


int get_flag(cpu_t *cpu, flag_t flag);
void set_flag(cpu_t *cpu, flag_t flag, int value);

int execute_next_instruction(cpu_t *cpu);


cpu_t *new_cpu();
void free_cpu(cpu_t *cpu);


uint8_t read_memory(cpu_t *cpu, uint16_t address);
void write_memory(cpu_t *cpu, uint16_t address, uint8_t value);

int execute_instruction(cpu_t *cpu, uint8_t instruction_byte, bool prefixed);


// =================================================================================
//                          Instructions
// =================================================================================


//https://rgbds.gbdev.io/docs/v0.9.4/gbz80.7


// Load instructions
int LD_r8_r8(cpu_t *cpu, reg_8bits_t reg_dest, reg_8bits_t reg_src);
int LD_r8_n8(cpu_t *cpu, reg_8bits_t reg);
int LD_r16_n16(cpu_t *cpu, uint16_t *reg);
int LD_HL_r8(cpu_t *cpu, reg_8bits_t reg);
int LD_HL_n8(cpu_t *cpu);
int LD_r8_HL(cpu_t *cpu, reg_8bits_t reg);
int LD_r16_A(cpu_t *cpu, uint16_t *reg);
int LD_n16_A(cpu_t *cpu);
int LDH_n16_A(cpu_t *cpu);
int LDH_C_A(cpu_t *cpu);
int LD_A_r16(cpu_t *cpu, uint16_t *reg);
int LD_A_n16(cpu_t *cpu);
int LDH_A_n16(cpu_t *cpu);
int LDH_A_C(cpu_t *cpu);
int LD_HLI_A(cpu_t *cpu);
int LD_HLD_A(cpu_t *cpu);
int LD_A_HLI(cpu_t *cpu);
int LD_A_HLD(cpu_t *cpu);


// 8-bit arithmetic instructions


int ADC_r8(cpu_t *cpu, reg_8bits_t reg);
int ADC_HL(cpu_t *cpu);
int ADC_n8(cpu_t *cpu);
int ADD_r8(cpu_t *cpu, reg_8bits_t reg);
int ADD_HL(cpu_t *cpu);
int ADD_n8(cpu_t *cpu);
int CP_r8(cpu_t *cpu, reg_8bits_t reg);
int CP_HL(cpu_t *cpu);
int CP_n8(cpu_t *cpu);
int DEC_r8(cpu_t *cpu, reg_8bits_t reg);
int DEC_HL(cpu_t *cpu);
int INC_r8(cpu_t *cpu, reg_8bits_t reg);
int INC_HL(cpu_t *cpu);
int SBC_r8(cpu_t *cpu, reg_8bits_t reg);
int SBC_HL(cpu_t *cpu);
int SBC_n8(cpu_t *cpu);
int SUB_r8(cpu_t *cpu, reg_8bits_t reg);
int SUB_HL(cpu_t *cpu);
int SUB_n8(cpu_t *cpu);
// 16-bit arithmetic instructions

int ADD_HL_r16(cpu_t *cpu, uint16_t *reg);
int DEC_r16(cpu_t *cpu, uint16_t *reg);
int INC_r16(cpu_t *cpu, uint16_t *reg);

// Bitwise logic instructions

int AND_r8(cpu_t *cpu, reg_8bits_t reg);
int AND_HL(cpu_t *cpu);
int AND_n8(cpu_t *cpu);
int CPL(cpu_t *cpu);
int OR_r8(cpu_t *cpu, reg_8bits_t reg);
int OR_HL(cpu_t *cpu);
int OR_n8(cpu_t *cpu);
int XOR_r8(cpu_t *cpu, reg_8bits_t reg);
int XOR_HL(cpu_t *cpu);
int XOR_n8(cpu_t *cpu);


// Bit flag instructions

int BIT_r8(cpu_t *cpu, reg_8bits_t reg, uint8_t bit);
int BIT_HL(cpu_t *cpu, uint8_t bit);
int RES_r8(cpu_t *cpu, reg_8bits_t reg, uint8_t bit);
int RES_HL(cpu_t *cpu, uint8_t bit);
int SET_r8(cpu_t *cpu, reg_8bits_t reg, uint8_t bit);
int SET_HL(cpu_t *cpu, uint8_t bit);


// Bit shift instructions

int RL_r8(cpu_t *cpu, reg_8bits_t reg);
int RL_HL(cpu_t *cpu);
int RLA(cpu_t *cpu);
int RLC_r8(cpu_t *cpu, reg_8bits_t reg);
int RLC_HL(cpu_t *cpu);
int RLCA(cpu_t *cpu);
int RR_r8(cpu_t *cpu, reg_8bits_t reg);
int RR_HL(cpu_t *cpu);
int RRA(cpu_t *cpu);
int RRC_r8(cpu_t *cpu, reg_8bits_t reg);
int RRC_HL(cpu_t *cpu);
int RRCA(cpu_t *cpu);
int SLA_r8(cpu_t *cpu, reg_8bits_t reg);
int SLA_HL(cpu_t *cpu);
int SRA_r8(cpu_t *cpu, reg_8bits_t reg);
int SRA_HL(cpu_t *cpu);
int SRL_r8(cpu_t *cpu, reg_8bits_t reg);
int SRL_HL(cpu_t *cpu);
int SWAP_r8(cpu_t *cpu, reg_8bits_t reg);
int SWAP_HL(cpu_t *cpu);



// Jumps and subroutine instructions

int CALL_n16(cpu_t *cpu);
int CALL_cc_n16(cpu_t *cpu);
int JP_HL(cpu_t *cpu);
int JP_n16(cpu_t *cpu);
int JP_cc_n16(cpu_t *cpu, bool cc);
int JR_e(cpu_t *cpu);
int JR_cc_e(cpu_t *cpu, bool cc);
int RET_cc(cpu_t *cpu, bool cc);
int RET(cpu_t *cpu);
int RETI(cpu_t *cpu);
int RST_n(cpu_t *cpu, uint8_t n);

// Carry flag instructions
int CCF(cpu_t *cpu);
int SCF(cpu_t *cpu);

// Stack manipulation instructions
int ADD_HL_Sp(cpu_t *cpu);
int ADD_SP_e8(cpu_t *cpu, int8_t offset);
int DEC_SP(cpu_t *cpu);
int INC_SP(cpu_t *cpu);
int LD_n16_SP(cpu_t *cpu);
int LD_SP_HL(cpu_t *cpu);
int LD_HL_SP_e8(cpu_t *cpu, int8_t offset);
int POP_r16(cpu_t *cpu, uint16_t *reg);
int PUSH_r16(cpu_t *cpu, uint16_t *reg);
int PUSH_AF(cpu_t *cpu);

// Interrupt-related instructions
int DI(cpu_t *cpu);
int EI(cpu_t *cpu);
int HALT(cpu_t *cpu);


// Miscellaneous instructions
int DAA(cpu_t *cpu);
int NOP(cpu_t *cpu);
int STOP(cpu_t *cpu);




bool compute_carry_add(uint8_t a, uint8_t b);
bool compute_halfcarry_add(uint8_t a, uint8_t b);
bool compute_carry_sub(uint8_t a, uint8_t b);
bool compute_halfcarry_sub(uint8_t a, uint8_t b);

//=================Utils=======================
uint16_t unsigned_16(uint8_t msb, uint8_t lsb);
operation_result_t add(uint8_t a, uint8_t b);

operation_result_t sub(uint8_t a, uint8_t b);
int bit(int i, uint8_t n);
uint8_t lsb(uint16_t u);
uint8_t msb(uint16_t u);


#endif