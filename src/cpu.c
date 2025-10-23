#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"




uint8_t get_8bit_register(cpu_t *cpu, reg_8bits_t reg_8bits) 
{
    registers_t *registers = cpu->registers;
    switch (reg_8bits)
    {
    case A:
        return (*registers->AF>>8) & 0x00FF;
    case B:
        return (*registers->BC>>8) & 0x00FF;
    case C:
        return *registers->BC & 0x00FF;
    case D:
        return (*registers->DE>>8) & 0x00FF;
    case E:
        return *registers->DE & 0x00FF;
    case H:
        return (*registers->HL>>8) & 0x00FF;
    case L:
        return *registers->HL & 0x00FF;
    default:
        fprintf(stderr, "Invalid 8-bit register specified.\n");
        exit(1);
    }
    return 0;
 
}

void set_8bit_register(cpu_t *cpu, reg_8bits_t reg_8bits, uint8_t value) 
{
    registers_t *registers = cpu->registers;
    switch (reg_8bits)
    {
    case A:
        *registers->AF = (*registers->AF & 0x00FF) | ((value & 0x00FF)<<8);
        break;
    case B:
        *registers->BC = (*registers->BC & 0x00FF) | ((value & 0x00FF)<<8);
        break;
    case C:
        *registers->BC = (*registers->BC & 0xFF00) | (value & 0x00FF);
        break;
    case D:
        *registers->DE = (*registers->DE & 0x00FF) | ((value & 0x00FF)<<8);
        break;
    case E:
        *registers->DE = (*registers->DE & 0xFF00) | (value & 0x00FF);
        break;
    case H:
        *registers->HL = (*registers->HL & 0x00FF) | ((value & 0x00FF)<<8);
        break;
    case L:
        *registers->HL = (*registers->HL & 0xFF00) | (value & 0x00FF);;
        break;
    default:
        fprintf(stderr, "Invalid 8-bit register specified.\n");
        exit(1);
    }
}


int get_flag(cpu_t *cpu, flag_t flag)
{
    registers_t *reg = cpu->registers; 
    switch (flag)
    {
    case ZERO:
        return (*reg->AF & 0x80) >> 7;
    case SUB:
        return (*reg->AF & 0x40) >> 6;
    case HALF_CARRY:
        return (*reg->AF & 0x20) >> 5;
    case CARRY:
        return (*reg->AF & 0x10) >> 4;
    default:
        fprintf(stderr, "Invalid flag specified.\n");
        exit(1);
    }
}

void set_flag(cpu_t *cpu, flag_t flag, int value)
{
    registers_t *reg = cpu->registers;
    switch (flag)
    {
    case ZERO:
        if (value)
            *reg->AF |= 0x80; // Set the ZERO flag
        else
            *reg->AF &= ~0x80;
        break;
    case SUB:
        if (value)
            *reg->AF |= 0x40; // Set the SUB flag
        else
            *reg->AF &= ~0x40;
        break;
    case HALF_CARRY:
        if (value)
            *reg->AF |= 0x20; // Set the HALF_CARRY flag
        else
            *reg->AF &= ~0x20;
        break;
    case CARRY:
        if (value)
            *reg->AF |= 0x10;
        else
            *reg->AF &= ~0x10;
        break;

    default:
        fprintf(stderr, "Invalid flag specified.\n");
        exit(1);
    }
}





//TODO: COMPLETE
cpu_t *new_cpu() 
{
    registers_t *registers = malloc(sizeof(registers_t));
    if (registers == NULL)
    {
        return NULL;
    }

    uint8_t *memorybus = malloc(0xFFFF * sizeof(uint8_t));
    if (memorybus == NULL)
    {
        free(registers);
        return NULL;
    }

    cpu_t *new_cpu = malloc(sizeof(cpu_t));
    if (new_cpu == NULL) 
    {
        free(registers);
        free(memorybus);
        return NULL;
    }
    new_cpu->registers = registers;
    new_cpu->memorybus = memorybus;
    new_cpu->PC = *registers->PC;
    new_cpu->SP = *registers->SP;
    new_cpu->IME = false;
    return new_cpu;
}

//TODO complete
void free_cpu(cpu_t *cpu) 
{   
    free(cpu->registers);
    cpu->registers = NULL;
    
    free(cpu->memorybus);
    cpu->memorybus = NULL;
    
    free(cpu);
    cpu = NULL;

    return;
}


uint8_t read_memory(cpu_t *cpu, uint16_t address) 
{
    if (cpu == NULL)
    {
        printf("Trying to read memory on null cpu pointer ");
        exit(1);
    }

    return cpu->memorybus[address];
}
/**
 * Writes a value to the specified memory address.
 *
 * @param cpu Pointer to the CPU structure.
 * @param address The memory address to write to.
 * @param value The value to write to memory.
*/
void write_memory(cpu_t *cpu, uint16_t address, uint8_t value)
{
    if (cpu == NULL)
    {
        printf("Trying to write memory on null cpu pointer ");
        exit(1);
    }

    cpu->memorybus[address] = value;
    return;
}


int execute_next_instruction(cpu_t *cpu)
{
    if (cpu == NULL)
    {
        printf("Trying to execute instructions on null cpu pointer ");
        exit(1);
    }
    
    uint8_t instruction_byte = read_memory(cpu, cpu->PC); cpu->PC++;
    bool prefixed = instruction_byte == 0xCB;
    if (prefixed) 
    {
        instruction_byte = read_memory(cpu, cpu->PC); cpu->PC++;
    }
    // Output : number of cycles took by instruction
    return execute_instruction(cpu, instruction_byte, prefixed); 
}



int execute_instruction(cpu_t *cpu, uint8_t instruction_byte, bool prefixed)
{
    if (cpu == NULL)
    {
        printf("Trying to execute instructions on null cpu pointer ");
        exit(1);
    }

    if (!prefixed)
    {
        switch (instruction_byte)
        {
        case 0x00: // NOP
            return NOP(cpu);
        case 0x01: // LD BC,nn
            return LD_r16_n16(cpu, cpu->registers->BC);
        case 0x02: // LD (BC),A
            return LD_r16_A(cpu, cpu->registers->BC);
        case 0x03: // INC BC
            return INC_r16(cpu, cpu->registers->BC);
        case 0x04: // INC B
            return INC_r8(cpu, B);
        case 0x05: // DEC B
            return DEC_r8(cpu, B);
        case 0x06: // LD B,n
            return LD_r8_n8(cpu, B);
        case 0x07: // RLCA
            return RLCA(cpu);
        case 0x08: // LD (nn),SP
            return LD_n16_SP(cpu);
        case 0x09: // ADD HL,BC
            return ADD_HL_r16(cpu, cpu->registers->BC);
        case 0x0A: // LD A,(BC)
            return LD_A_r16(cpu, cpu->registers->BC);
        case 0x0B: // DEC BC
            return DEC_r16(cpu, cpu->registers->BC);
        case 0x0C: // INC C
            return INC_r8(cpu, C);
        case 0x0D: // DEC C
            return DEC_r8(cpu, C);
        case 0x0E: // LD C,n
            return LD_r8_n8(cpu, C);
        case 0x0F: // RRCA
            return RRCA(cpu);
        //case 0x10: // STOP
        case 0x11: // LD DE,nn
            return LD_r16_n16(cpu, cpu->registers->DE);
        case 0x12: // LD (DE),A
            return LD_r16_A(cpu, cpu->registers->DE);
        case 0x13: // INC DE
            return INC_r16(cpu, cpu->registers->DE);
        case 0x14: // INC D
            return INC_r8(cpu, D);
        case 0x15: // DEC D
            return DEC_r8(cpu, D);
        case 0x16: // LD D,n
            return LD_r8_n8(cpu, D);
        case 0x17: // RLA
            return RLCA(cpu);
        case 0x18: // JR e
            return JR_e(cpu);
        case 0x19: // ADD HL,DE
            return ADD_HL_r16(cpu, cpu->registers->DE);
        case 0x1A: // LD A,(DE)
            return LD_A_r16(cpu, cpu->registers->DE);
        case 0x1B: // DEC DE
            return DEC_r16(cpu, cpu->registers->DE);
        case 0x1C: // INC E
            return INC_r8(cpu, E);
        case 0x1D: // DEC E
            return DEC_r8(cpu, E);
        case 0x1E: // LD E,n
            return LD_r8_n8(cpu, E);
        case 0x1F: // RRA
            return RRA(cpu);
        case 0x20: // JR NZ,e
            return JR_cc_e(cpu, !get_flag(cpu, ZERO));
        case 0x21: // LD HL,nn
            return LD_r16_n16(cpu, cpu->registers->HL);
        case 0x22: // LD (HL+),A
            return LD_HLI_A(cpu);
        case 0x23: // INC HL
            return INC_r16(cpu, cpu->registers->HL);
        case 0x24: // INC H
            return INC_r8(cpu, H);
        case 0x25: // DEC H
            return DEC_r8(cpu, H);
        case 0x26: // LD H,n
            return LD_r8_n8(cpu, H);
        //case 0x27: // DAA
        case 0x28: // JR Z,n
            return JR_cc_e(cpu, get_flag(cpu, ZERO));
        case 0x29: // ADD HL,HL
            return ADD_HL_r16(cpu, cpu->registers->HL);
        case 0x2A: // LD A,(HL+)
            return LD_A_HLI(cpu);
        case 0x2B: // DEC HL
            return DEC_r16(cpu, cpu->registers->HL);
        case 0x2C: // INC L
            return INC_r8(cpu, L);
        case 0x2D: // DEC L
            return DEC_r8(cpu, L);
        case 0x2E: // LD L,n
            return LD_r8_n8(cpu, L);
        case 0x2F: // CPL
            return CPL(cpu);
        case 0x30: // JR NC,n
            return JR_cc_e(cpu, !get_flag(cpu, CARRY));
        case 0x31: // LD SP,nn
            return LD_r16_n16(cpu, cpu->registers->SP);
        case 0x32: // LD (HL-),A
            return LD_HLD_A(cpu);
        case 0x33: // INC SP
            return INC_r16(cpu, cpu->registers->SP);    
        case 0x34: // INC (HL)
            return INC_HL(cpu);    
        case 0x35: // DEC (HL)
            return DEC_HL(cpu);
        case 0x36: // LD (HL),n
            return LD_HL_n8(cpu);
        case 0x37: // SCF
            return SCF(cpu);
        case 0x38: // JR C,n
            return JR_cc_e(cpu, get_flag(cpu, CARRY));
        case 0x39: // ADD HL,SP   
            return ADD_HL_r16(cpu, cpu->registers->SP);
        case 0x3A: // LD A,(HL-)
            return LD_A_HLD(cpu);
        case 0x3B: // DEC SP
            return DEC_r16(cpu, cpu->registers->SP);
        case 0x3C: // INC A
            return INC_r8(cpu, A);
        case 0x3D: // DEC A
            return DEC_r8(cpu, A);
        case 0x3E: // LD A,n
            return LD_r8_n8(cpu, A);
        case 0x3F: // CCF
            return CCF(cpu);
        case 0x40: // LD B,B
            return LD_r8_r8(cpu, B, B);
        case 0x41: // LD B,C
            return LD_r8_r8(cpu, B, C);
        case 0x42: // LD B,D
            return LD_r8_r8(cpu, B, D);
        case 0x43: // LD B,E
            return LD_r8_r8(cpu, B, E);
        case 0x44: // LD B,H
            return LD_r8_r8(cpu, B, H);
        case 0x45: // LD B,L
            return LD_r8_r8(cpu, B, L);
        case 0x46: // LD B,(HL)
            return LD_r8_HL(cpu, B);
        case 0x47: // LD B,A
            return LD_r8_r8(cpu, B, A);
        case 0x48: // LD C,B
            return LD_r8_r8(cpu, C, B);
        case 0x49: // LD C,C
            return LD_r8_r8(cpu, C, C);
        case 0x4A: // LD C,D
            return LD_r8_r8(cpu, C, D);
        case 0x4B: // LD C,E
            return LD_r8_r8(cpu, C, E);
        case 0x4C: // LD C,H
            return LD_r8_r8(cpu, C, H);
        case 0x4D: // LD C,L
            return LD_r8_r8(cpu, C, L);
        case 0x4E: // LD C,(HL)
            return LD_r8_HL(cpu, C);
        case 0x4F: // LD C,A
            return LD_r8_r8(cpu, C, A);
        case 0x50: // LD D,B
            return LD_r8_r8(cpu, D, B);
        case 0x51: // LD D,C
            return LD_r8_r8(cpu, D, C);
        case 0x52: // LD D,D
            return LD_r8_r8(cpu, D, D);
        case 0x53: // LD D,E
            return LD_r8_r8(cpu, D, E);
        case 0x54: // LD D,H
            return LD_r8_r8(cpu, D, H);
        case 0x55: // LD D,L
            return LD_r8_r8(cpu, D, L);
        case 0x56: // LD D,(HL)
            return LD_r8_HL(cpu, D);
        case 0x57: // LD D,A
            return LD_r8_r8(cpu, D, A);
        case 0x58: // LD E,B
            return LD_r8_r8(cpu, E, B);
        case 0x59: // LD E,C
            return LD_r8_r8(cpu, E, C);
        case 0x5A: // LD E,D
            return LD_r8_r8(cpu, E, D);
        case 0x5B: // LD E,E
            return LD_r8_r8(cpu, E, E);
        case 0x5C: // LD E,H
            return LD_r8_r8(cpu, E, H);
        case 0x5D: // LD E,L
            return LD_r8_r8(cpu, E, L);
        case 0x5E: // LD E,(HL)
            return LD_r8_HL(cpu, E);
        case 0x5F: // LD E,A
            return LD_r8_r8(cpu, E, A);
        case 0x60: // LD H,B
            return LD_r8_r8(cpu, H, B);
        case 0x61: // LD H,C
            return LD_r8_r8(cpu, H, C);
        case 0x62: // LD H,D
            return LD_r8_r8(cpu, H, D);
        case 0x63: // LD H,E
            return LD_r8_r8(cpu, H, E);
        case 0x64: // LD H,H
            return LD_r8_r8(cpu, H, H);
        case 0x65: // LD H,L
            return LD_r8_r8(cpu, H, L);
        case 0x66: // LD H,(HL)
            return LD_r8_HL(cpu, H);
        case 0x67: // LD H,A
            return LD_r8_r8(cpu, H, A);
        case 0x68: // LD L,B
            return LD_r8_r8(cpu, L, B);
        case 0x69: // LD L,C
            return LD_r8_r8(cpu, L, C);
        case 0x6A: // LD L,D
            return LD_r8_r8(cpu, L, D);
        case 0x6B: // LD L,E
            return LD_r8_r8(cpu, L, E);
        case 0x6C: // LD L,H
            return LD_r8_r8(cpu, L, H);
        case 0x6D: // LD L,L
            return LD_r8_r8(cpu, L, L);
        case 0x6E: // LD L,(HL)
            return LD_r8_HL(cpu, L);
        case 0x6F: // LD L,A
            return LD_r8_r8(cpu, L, A);
        case 0x70: // LD (HL),B
            return LD_HL_r8(cpu, B);
        case 0x71: // LD (HL),C
            return LD_HL_r8(cpu, C);
        case 0x72: // LD (HL),D
            return LD_HL_r8(cpu, D);
        case 0x73: // LD (HL),E
            return LD_HL_r8(cpu, E);
        case 0x74: // LD (HL),H
            return LD_HL_r8(cpu, H);
        case 0x75: // LD (HL),L
            return LD_HL_r8(cpu, L);
        //case 0x76: // HALT
        case 0x77: // LD (HL),A
            return LD_HL_r8(cpu, A);
        case 0x78: // LD A,B
            return LD_r8_r8(cpu, A, B);
        case 0x79: // LD A,C
            return LD_r8_r8(cpu, A, C);
        case 0x7A: // LD A,D
            return LD_r8_r8(cpu, A, D);
        case 0x7B: // LD A,E
            return LD_r8_r8(cpu, A, E);
        case 0x7C: // LD A,H
            return LD_r8_r8(cpu, A, H);
        case 0x7D: // LD A,L
            return LD_r8_r8(cpu, A, L);
        case 0x7E: // LD A,(HL)
            return LD_r8_HL(cpu, A);
        case 0x7F: // LD A,A
            return LD_r8_r8(cpu, A, A);
        case 0x80: // ADD A,B
            return ADD_r8(cpu, B);
        case 0x81: // ADD C
            return ADD_r8(cpu, C);
        case 0x82: // ADD D
            return ADD_r8(cpu, D);
        case 0x83: // ADD E
            return ADD_r8(cpu, E);
        case 0x84: // ADD H
            return ADD_r8(cpu, H);
        case 0x85: // ADD L
            return ADD_r8(cpu, L);
        case 0x86: // ADD (HL)
            return ADD_HL(cpu);
        case 0x87: // ADD A
            return ADD_r8(cpu, A);
        case 0x88: // ADC B
            return ADC_r8(cpu, B);
        case 0x89: // ADC C
            return ADC_r8(cpu, C);
        case 0x8A: // ADC D
            return ADC_r8(cpu, D);
        case 0x8B: // ADC E
            return ADC_r8(cpu, E);
        case 0x8C: // ADC H
            return ADC_r8(cpu, H);
        case 0x8D: // ADC L
            return ADC_r8(cpu, L);
        case 0x8E: // ADC (HL)
            return ADC_HL(cpu);
        case 0x8F: // ADC A
            return ADC_r8(cpu, A);
        case 0x90: // SUB B
            return SUB_r8(cpu, B);
        case 0x91: // SUB C
            return SUB_r8(cpu, C);
        case 0x92: // SUB D
            return SUB_r8(cpu, D);
        case 0x93: // SUB E
            return SUB_r8(cpu, E);
        case 0x94: // SUB H
            return SUB_r8(cpu, H);
        case 0x95: // SUB L
            return SUB_r8(cpu, L);
        case 0x96: // SUB (HL)
            return SUB_HL(cpu);
        case 0x97: // SUB A
            return SUB_r8(cpu, A);
        case 0x98: // SBC A,B
            return SBC_r8(cpu, B);
        case 0x99: // SBC A,C
            return SBC_r8(cpu, C);
        case 0x9A: // SBC A,D
            return SBC_r8(cpu, D);
        case 0x9B: // SBC A,E
            return SBC_r8(cpu, E);
        case 0x9C: // SBC A,H
            return SBC_r8(cpu, H);
        case 0x9D: // SBC A,L
            return SBC_r8(cpu, L);
        case 0x9E: // SBC A,(HL)
            return SBC_HL(cpu);
        case 0x9F: // SBC A,A
            return SBC_r8(cpu, A);
        case 0xA0: // AND A,B
            return AND_r8(cpu, B);
        case 0xA1: // AND A,C
            return AND_r8(cpu, C);
        case 0xA2: // AND A,D
            return AND_r8(cpu, D);
        case 0xA3: // AND A,E
            return AND_r8(cpu, E);
        case 0xA4: // AND A,H
            return AND_r8(cpu, H);
        case 0xA5: // AND A,L
            return AND_r8(cpu, L);
        case 0xA6: // AND A,(HL)
            return AND_HL(cpu);
        case 0xA7: // AND A,A
            return AND_r8(cpu, A);
        case 0xA8: // XOR A,B
            return XOR_r8(cpu, B);
        case 0xA9: // XOR A,C
            return XOR_r8(cpu, C);
        case 0xAA: // XOR A,D
            return XOR_r8(cpu, D);
        case 0xAB: // XOR A,E
            return XOR_r8(cpu, E);
        case 0xAC: // XOR A,H
            return XOR_r8(cpu, H);
        case 0xAD: // XOR A,L
            return XOR_r8(cpu, L);
        case 0xAE: // XOR A,(HL)
            return XOR_HL(cpu);
        case 0xAF: // XOR A,A 
            return XOR_r8(cpu, A);
        case 0xB0: // OR A,B
            return OR_r8(cpu, B);
        case 0xB1: // OR A,C
            return OR_r8(cpu, C);
        case 0xB2: // OR A,D
            return OR_r8(cpu, D);
        case 0xB3: // OR A,E
            return OR_r8(cpu, E);
        case 0xB4: // OR A,H
            return OR_r8(cpu, H);
        case 0xB5: // OR A,L  
            return OR_r8(cpu, L);
        case 0xB6: // OR A,(HL)
            return OR_HL(cpu);
        case 0xB7: // OR A,A
            return OR_r8(cpu, A);
        case 0xB8: // CP A,B
            return CP_r8(cpu, B);
        case 0xB9: // CP A,C
            return CP_r8(cpu, C);
        case 0xBA: // CP A,D
            return CP_r8(cpu, D);
        case 0xBB: // CP A,E
            return CP_r8(cpu, E);
        case 0xBC: // CP A,H
            return CP_r8(cpu, H);
        case 0xBD: // CP A,L
            return CP_r8(cpu, L);
        case 0xBE: // CP A,(HL)
            return CP_HL(cpu);
        case 0xBF: // CP A,A
            return CP_r8(cpu, A);
        case 0xC0: // RET NZ
            return RET_cc(cpu, !get_flag(cpu, ZERO));
        case 0xC1: // POP BC
            return POP_r16(cpu, cpu->registers->BC);
        case 0xC2: // JP NZ,nn
            return JP_cc_n16(cpu, !get_flag(cpu, ZERO));
        case 0xC3: // JP nn
            return JP_n16(cpu);
        case 0xC4: // CALL NZ,nn
            return CALL_cc_n16(cpu, !get_flag(cpu, ZERO));
        case 0xC5: // PUSH BC
            return PUSH_r16(cpu, cpu->registers->BC);
        case 0xC6: // ADD A,n
            return ADD_n8(cpu);
        case 0xC7: // RST 00H
            return RST_n(cpu, 0x00);
        case 0xC8: // RET Z
            return RET_cc(cpu, get_flag(cpu, ZERO));
        case 0xC9: // RET
            return RET(cpu);
        case 0xCA: // JP Z,nn
            return JP_cc_n16(cpu, get_flag(cpu, ZERO));
        case 0xCB: // PREFIX
            fprintf(stderr, "Unexpected PREFIX instruction without prefixed flag.\n");
            exit(1);
        case 0xCC: // CALL Z,nn
            return CALL_cc_n16(cpu, get_flag(cpu, ZERO));
        case 0xCD: // CALL nn
            return CALL_n16(cpu);
        case 0xCE: // ADC A,n
            return ADC_n8(cpu);
        case 0xCF: // RST 08H
            return RST_n(cpu, 0x08);
        case 0xD0: // RET NC
            return RET_cc(cpu, !get_flag(cpu, CARRY));
        case 0xD1: // POP DE
            return POP_r16(cpu, cpu->registers->DE);
        case 0xD2: // JP NC,nn
            return JP_cc_n16(cpu, !get_flag(cpu, CARRY));
        case 0xD3: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xD3.\n");
            exit(1);
        case 0xD4: // CALL NC,nn
            return CALL_cc_n16(cpu, !get_flag(cpu, CARRY));
        case 0xD5: // PUSH DE
            return PUSH_r16(cpu, cpu->registers->DE);
        case 0xD6: // SUB n
            return SUB_n8(cpu);
        case 0xD7: // RST 10H
            return RST_n(cpu, 0x10);
        case 0xD8: // RET C
            return RET_cc(cpu, get_flag(cpu, CARRY));
        case 0xD9: // RETI
            return RETI(cpu);
        case 0xDA: // JP C,nn
            return JP_cc_n16(cpu, get_flag(cpu, CARRY));
        case 0xDB: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xDB.\n");
            exit(1);
        case 0xDC: // CALL C,nn
            return CALL_cc_n16(cpu, get_flag(cpu, CARRY));
        case 0xDD: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xDD.\n");
            exit(1);
        case 0xDE: // SBC A,n
            return SBC_n8(cpu);
        case 0xDF: // RST 18H
            return RST_n(cpu, 0x18);
        case 0xE0: // LDH (n),A
            return LDH_n8_A(cpu);
        case 0xE1: // POP HL
            return POP_r16(cpu, cpu->registers->HL);
        case 0xE2: // LD (C),A
            return LDH_C_A(cpu);
        case 0xE3: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xE3.\n");
            exit(1);
        case 0xE4: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xE4.\n");
            exit(1);
        case 0xE5: // PUSH HL
            return PUSH_r16(cpu, cpu->registers->HL);
        case 0xE6: // AND A,n
            return AND_n8(cpu);
        case 0xE7: // RST 20H
            return RST_n(cpu, 0x20);
        case 0xE8: // ADD SP,n
            return ADD_SP_e8(cpu);
        case 0xE9: // JP (HL)
            return JP_HL(cpu);
        case 0xEA: // LD (nn),A
            return LD_n16_A(cpu);
        case 0xEB: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xEB.\n");
            exit(1);
        case 0xEC: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xEC.\n");
            exit(1);
        case 0xED: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xED.\n");
            exit(1);    
        case 0xEE: // XOR A,n
            return XOR_n8(cpu);
        case 0xEF: // RST 28H
            return RST_n(cpu, 0x28);
        case 0xF0: // LDH A,(n)
            return LDH_A_n16(cpu);   
        case 0xF1: // POP AF
            return POP_r16(cpu, cpu->registers->AF);
        case 0xF2: // LD A,(C)
            return LDH_A_C(cpu);
        case 0xF3: // DI
            return DI(cpu);
        case 0xF4: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xF4.\n");
            exit(1);
        case 0xF5: // PUSH AF
            return PUSH_r16(cpu, cpu->registers->AF);
        case 0xF6: // OR A,n
            return OR_n8(cpu);
        case 0xF7: // RST 30H
            return RST_n(cpu, 0x30);
        case 0xF8: // LD HL,SP+n
            return LD_HL_SP_e8(cpu);
        case 0xF9: // LD SP,HL
            return LD_SP_HL(cpu);
        case 0xFA: // LD A,(nn)
            return LD_A_n16(cpu);
        case 0xFB: // EI
            return EI(cpu);
        case 0xFC: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xFC.\n");
            exit(1);
        case 0xFD: // Blank
            fprintf(stderr, "Attempted to execute invalid instruction 0xFD.\n");
            exit(1);
        case 0xFE: // CP A,n
            return CP_n8(cpu);
        case 0xFF: // RST 38H
            return RST_n(cpu, 0x38);
        default:
            break;
        }
    } 
    else 
    {
        switch (instruction_byte)
        {
        case 0x00: // RLC B
            return RLC_r8(cpu, B);
        case 0x01: // RLC C
            return RLC_r8(cpu, C);
        case 0x02: // RLC D
            return RLC_r8(cpu, D);
        case 0x03: // RLC E
            return RLC_r8(cpu, E);
        case 0x04: // RLC H
            return RLC_r8(cpu, H);
        case 0x05: // RLC L
            return RLC_r8(cpu, L);
        case 0x06: // RLC (HL)
            return RLC_HL(cpu);
        case 0x07: // RLC A
            return RLC_r8(cpu, A);
        case 0x08: // RRC B
            return RRC_r8(cpu, B);
        case 0x09: // RRC C
            return RRC_r8(cpu, C);
        case 0x0A: // RRC D
            return RRC_r8(cpu, D);
        case 0x0B: // RRC E
            return RRC_r8(cpu, E);
        case 0x0C: // RRC H 
            return RRC_r8(cpu, H);
        case 0x0D: // RRC L
            return RRC_r8(cpu, L);
        case 0x0E: // RRC (HL)
            return RRC_HL(cpu);
        case 0x0F: // RRC A
            return RRC_r8(cpu, A);
        case 0x10: // RL B
            return RL_r8(cpu, B);
        case 0x11: // RL C
            return RL_r8(cpu, C);
        case 0x12: // RL D
            return RL_r8(cpu, D);
        case 0x13: // RL E
            return RL_r8(cpu, E);
        case 0x14: // RL H
            return RL_r8(cpu, H);
        case 0x15: // RL L
            return RL_r8(cpu, L);
        case 0x16: // RL (HL)
            return RL_HL(cpu);
        case 0x17: // RL A
            return RL_r8(cpu, A);
        case 0x18: // RR B
            return RR_r8(cpu, B);
        case 0x19: // RR C
            return RR_r8(cpu, C);  
        case 0x1A: // RR D
            return RR_r8(cpu, D);
        case 0x1B: // RR E
            return RR_r8(cpu, E);
        case 0x1C: // RR H
            return RR_r8(cpu, H);
        case 0x1D: // RR L
            return RR_r8(cpu, L);
        case 0x1E: // RR (HL)
            return RR_HL(cpu);
        case 0x1F: // RR A 
            return RR_r8(cpu, A);
        case 0x20: // SLA B
            return SLA_r8(cpu, B);
        case 0x21: // SLA C
            return SLA_r8(cpu, C);
        case 0x22: // SLA D
            return SLA_r8(cpu, D);
        case 0x23: // SLA E
            return SLA_r8(cpu, E);
        case 0x24: // SLA H
            return SLA_r8(cpu, H);
        case 0x25: // SLA L
            return SLA_r8(cpu, L);
        case 0x26: // SLA (HL)
            return SLA_HL(cpu);
        case 0x27: // SLA A
            return SLA_r8(cpu, A);
        case 0x28: // SRA B
            return SRA_r8(cpu, B);
        case 0x29: // SRA C
            return SRA_r8(cpu, C);
        case 0x2A: // SRA D
            return SRA_r8(cpu, D);
        case 0x2B: // SRA E
            return SRA_r8(cpu, E);
        case 0x2C: // SRA H
            return SRA_r8(cpu, H);  
        case 0x2D: // SRA L
            return SRA_r8(cpu, L);
        case 0x2E: // SRA (HL)
            return SRA_HL(cpu);
        case 0x2F: // SRA A
            return SRA_r8(cpu, A);
        case 0x30: // SWAP B
            return SWAP_r8(cpu, B);
        case 0x31: // SWAP C
            return SWAP_r8(cpu, C);
        case 0x32: // SWAP D
            return SWAP_r8(cpu, D);
        case 0x33: // SWAP E
            return SWAP_r8(cpu, E);
        case 0x34: // SWAP H
            return SWAP_r8(cpu, H);
        case 0x35: // SWAP L
            return SWAP_r8(cpu, L);
        case 0x36: // SWAP (HL)
            return SWAP_HL(cpu);
        case 0x37: // SWAP A
            return SWAP_r8(cpu, A);
        case 0x38: // SRL B
            return SRL_r8(cpu, B);
        case 0x39: // SRL C
            return SRL_r8(cpu, C);
        case 0x3A: // SRL D
            return SRL_r8(cpu, D);
        case 0x3B: // SRL E
            return SRL_r8(cpu, E);
        case 0x3C: // SRL H
            return SRL_r8(cpu, H);
        case 0x3D: // SRL L
            return SRL_r8(cpu, L);
        case 0x3E: // SRL (HL)
            return SRL_HL(cpu);
        case 0x3F: // SRL A
            return SRL_r8(cpu, A);
        case 0x40: // BIT 0,B
            return BIT_r8(cpu, 0, B);
        case 0x41: // BIT 0,C
            return BIT_r8(cpu, 0, C);
        case 0x42: // BIT 0,D
            return BIT_r8(cpu, 0, D);
        case 0x43: // BIT 0,E
            return BIT_r8(cpu, 0, E);
        case 0x44: // BIT 0,H
            return BIT_r8(cpu, 0, H);
        case 0x45: // BIT 0,L
            return BIT_r8(cpu, 0, L);
        case 0x46: // BIT 0,(HL)
            return BIT_HL(cpu, 0);
        case 0x47: // BIT 0,A
            return BIT_r8(cpu, 0, A);
        case 0x48: // BIT 1,B
            return BIT_r8(cpu, 1, B);
        case 0x49: // BIT 1,C
            return BIT_r8(cpu, 1, C);
        case 0x4A: // BIT 1,D
            return BIT_r8(cpu, 1, D);
        case 0x4B: // BIT 1,E
            return BIT_r8(cpu, 1, E);
        case 0x4C: // BIT 1,H
            return BIT_r8(cpu, 1, H);
        case 0x4D: // BIT 1,L
            return BIT_r8(cpu, 1, L);
        case 0x4E: // BIT 1,(HL)
            return BIT_HL(cpu, 1);
        case 0x4F: // BIT 1,A
            return BIT_r8(cpu, 1, A);
        case 0x50: // BIT 2,B
            return BIT_r8(cpu, 2, B);
        case 0x51: // BIT 2,C
            return BIT_r8(cpu, 2, C);
        case 0x52: // BIT 2,D
            return BIT_r8(cpu, 2, D);
        case 0x53: // BIT 2,E
            return BIT_r8(cpu, 2, E);
        case 0x54: // BIT 2,H
            return BIT_r8(cpu, 2, H);
        case 0x55: // BIT 2,L
            return BIT_r8(cpu, 2, L);
        case 0x56: // BIT 2,(HL)
            return BIT_HL(cpu, 2);
        case 0x57: // BIT 2,A
            return BIT_r8(cpu, 2, A);
        case 0x58: // BIT 3,B
            return BIT_r8(cpu, 3, B);
        case 0x59: // BIT 3,C
            return BIT_r8(cpu, 3, C);
        case 0x5A: // BIT 3,D
            return BIT_r8(cpu, 3, D);
        case 0x5B: // BIT 3,E
            return BIT_r8(cpu, 3, E);
        case 0x5C: // BIT 3,H
            return BIT_r8(cpu, 3, H);
        case 0x5D: // BIT 3,L
            return BIT_r8(cpu, 3, L);
        case 0x5E: // BIT 3,(HL)
            return BIT_HL(cpu, 3);
        case 0x5F: // BIT 3,A
            return BIT_r8(cpu, 3, A);
        case 0x60: // BIT 4,B
            return BIT_r8(cpu, 4, B);
        case 0x61: // BIT 4,C
            return BIT_r8(cpu, 4, C);
        case 0x62: // BIT 4,D
            return BIT_r8(cpu, 4, D);
        case 0x63: // BIT 4,E
            return BIT_r8(cpu, 4, E);
        case 0x64: // BIT 4,H
            return BIT_r8(cpu, 4, H);
        case 0x65: // BIT 4,L
            return BIT_r8(cpu, 4, L);
        case 0x66: // BIT 4,(HL)
            return BIT_HL(cpu, 4);
        case 0x67: // BIT 4,A
            return BIT_r8(cpu, 4, A);
        case 0x68: // BIT 5,B
            return BIT_r8(cpu, 5, B);
        case 0x69: // BIT 5,C
            return BIT_r8(cpu, 5, C);
        case 0x6A: // BIT 5,D
            return BIT_r8(cpu, 5, D);
        case 0x6B: // BIT 5,E
            return BIT_r8(cpu, 5, E);
        case 0x6C: // BIT 5,H
            return BIT_r8(cpu, 5, H);
        case 0x6D: // BIT 5,L
            return BIT_r8(cpu, 5, L);
        case 0x6E: // BIT 5,(HL)
            return BIT_HL(cpu, 5);
        case 0x6F: // BIT 5,A
            return BIT_r8(cpu, 5, A);
        case 0x70: // BIT 6,B
            return BIT_r8(cpu, 6, B);
        case 0x71: // BIT 6,C
            return BIT_r8(cpu, 6, C);
        case 0x72: // BIT 6,D
            return BIT_r8(cpu, 6, D);
        case 0x73: // BIT 6,E
            return BIT_r8(cpu, 6, E);
        case 0x74: // BIT 6,H
            return BIT_r8(cpu, 6, H);
        case 0x75: // BIT 6,L
            return BIT_r8(cpu, 6, L);
        case 0x76: // BIT 6,(HL)
            return BIT_HL(cpu, 6);
        case 0x77: // BIT 6,A
            return BIT_r8(cpu, 6, A);
        case 0x78: // BIT 7,B
            return BIT_r8(cpu, 7, B);
        case 0x79: // BIT 7,C
            return BIT_r8(cpu, 7, C);
        case 0x7A: // BIT 7,D
            return BIT_r8(cpu, 7, D);
        case 0x7B: // BIT 7,E
            return BIT_r8(cpu, 7, E);
        case 0x7C: // BIT 7,H
            return BIT_r8(cpu, 7, H);
        case 0x7D: // BIT 7,L
            return BIT_r8(cpu, 7, L);
        case 0x7E: // BIT 7,(HL)
            return BIT_HL(cpu, 7);
        case 0x7F: // BIT 7,A
            return BIT_r8(cpu, 7, A);
        case 0x80: // RES 0,B
            return RES_r8(cpu, 0, B);
        case 0x81: // RES 0,C
            return RES_r8(cpu, 0, C);
        case 0x82: // RES 0,D
            return RES_r8(cpu, 0, D);
        case 0x83: // RES 0,E
            return RES_r8(cpu, 0, E);
        case 0x84: // RES 0,H
            return RES_r8(cpu, 0, H);
        case 0x85: // RES 0,L
            return RES_r8(cpu, 0, L);
        case 0x86: // RES 0,(HL)
            return RES_HL(cpu, 0);
        case 0x87: // RES 0,A
            return RES_r8(cpu, 0, A);
        case 0x88: // RES 1,B
            return RES_r8(cpu, 1, B);
        case 0x89: // RES 1,C
            return RES_r8(cpu, 1, C);
        case 0x8A: // RES 1,D
            return RES_r8(cpu, 1, D);
        case 0x8B: // RES 1,E
            return RES_r8(cpu, 1, E);
        case 0x8C: // RES 1,H
            return RES_r8(cpu, 1, H);
        case 0x8D: // RES 1,L
            return RES_r8(cpu, 1, L);
        case 0x8E: // RES 1,(HL)
            return RES_HL(cpu, 1);
        case 0x8F: // RES 1,A
            return RES_r8(cpu, 1, A);
        case 0x90: // RES 2,B
            return RES_r8(cpu, 2, B);
        case 0x91: // RES 2,C
            return RES_r8(cpu, 2, C);
        case 0x92: // RES 2,D
            return RES_r8(cpu, 2, D);
        case 0x93: // RES 2,E
            return RES_r8(cpu, 2, E);
        case 0x94: // RES 2,H
            return RES_r8(cpu, 2, H);
        case 0x95: // RES 2,L
            return RES_r8(cpu, 2, L);
        case 0x96: // RES 2,(HL)
            return RES_HL(cpu, 2);
        case 0x97: // RES 2,A
            return RES_r8(cpu, 2, A);
        case 0x98: // RES 3,B
            return RES_r8(cpu, 3, B);
        case 0x99: // RES 3,C
            return RES_r8(cpu, 3, C);
        case 0x9A: // RES 3,D
            return RES_r8(cpu, 3, D);
        case 0x9B: // RES 3,E
            return RES_r8(cpu, 3, E);
        case 0x9C: // RES 3,H
            return RES_r8(cpu, 3, H);
        case 0x9D: // RES 3,L
            return RES_r8(cpu, 3, L);
        case 0x9E: // RES 3,(HL)
            return RES_HL(cpu, 3);
        case 0x9F: // RES 3,A
            return RES_r8(cpu, 3, A);
        case 0xA0: // RES 4,B
            return RES_r8(cpu, 4, B);
        case 0xA1: // RES 4,C
            return RES_r8(cpu, 4, C);
        case 0xA2: // RES 4,D
            return RES_r8(cpu, 4, D);
        case 0xA3: // RES 4,E
            return RES_r8(cpu, 4, E);
        case 0xA4: // RES 4,H
            return RES_r8(cpu, 4, H);
        case 0xA5: // RES 4,L
            return RES_r8(cpu, 4, L);
        case 0xA6: // RES 4,(HL)
            return RES_HL(cpu, 4);
        case 0xA7: // RES 4,A
            return RES_r8(cpu, 4, A);
        case 0xA8: // RES 5,B
            return RES_r8(cpu, 5, B);
        case 0xA9: // RES 5,C
            return RES_r8(cpu, 5, C);
        case 0xAA: // RES 5,D
            return RES_r8(cpu, 5, D);
        case 0xAB: // RES 5,E
            return RES_r8(cpu, 5, E);
        case 0xAC: // RES 5,H
            return RES_r8(cpu, 5, H);
        case 0xAD: // RES 5,L
            return RES_r8(cpu, 5, L);
        case 0xAE: // RES 5,(HL)
            return RES_HL(cpu, 5);
        case 0xAF: // RES 5,A
            return RES_r8(cpu, 5, A);
        case 0xB0: // RES 6,B
            return RES_r8(cpu, 6, B);
        case 0xB1: // RES 6,C
            return RES_r8(cpu, 6, C);
        case 0xB2: // RES 6,D
            return RES_r8(cpu, 6, D);
        case 0xB3: // RES 6,E
            return RES_r8(cpu, 6, E);
        case 0xB4: // RES 6,H
            return RES_r8(cpu, 6, H);
        case 0xB5: // RES 6,L
            return RES_r8(cpu, 6, L);
        case 0xB6: // RES 6,(HL)
            return RES_HL(cpu, 6);
        case 0xB7: // RES 6,A
            return RES_r8(cpu, 6, A);
        case 0xB8: // RES 7,B
            return RES_r8(cpu, 7, B);
        case 0xB9: // RES 7,C
            return RES_r8(cpu, 7, C);
        case 0xBA: // RES 7,D
            return RES_r8(cpu, 7, D);
        case 0xBB: // RES 7,E
            return RES_r8(cpu, 7, E);
        case 0xBC: // RES 7,H
            return RES_r8(cpu, 7, H);
        case 0xBD: // RES 7,L
            return RES_r8(cpu, 7, L);
        case 0xBE: // RES 7,(HL)
            return RES_HL(cpu, 7);
        case 0xBF: // RES 7,A
            return RES_r8(cpu, 7, A);
        
        case 0xC0: // SET 0,B
            return SET_r8(cpu, 0, B);
        case 0xC1: // SET 0,C
            return SET_r8(cpu, 0, C);
        case 0xC2: // SET 0,D
            return SET_r8(cpu, 0, D);
        case 0xC3: // SET 0,E
            return SET_r8(cpu, 0, E);
        case 0xC4: // SET 0,H
            return SET_r8(cpu, 0, H);
        case 0xC5: // SET 0,L
            return SET_r8(cpu, 0, L);
        case 0xC6: // SET 0,(HL)
            return SET_HL(cpu, 0);
        case 0xC7: // SET 0,A
            return SET_r8(cpu, 0, A);
        case 0xC8: // SET 1,B
            return SET_r8(cpu, 1, B);
        case 0xC9: // SET 1,C
            return SET_r8(cpu, 1, C);
        case 0xCA: // SET 1,D
            return SET_r8(cpu, 1, D);
        case 0xCB: // SET 1,E
            return SET_r8(cpu, 1, E);
        case 0xCC: // SET 1,H
            return SET_r8(cpu, 1, H);
        case 0xCD: // SET 1,L
            return SET_r8(cpu, 1, L);
        case 0xCE: // SET 1,(HL)
            return SET_HL(cpu, 1);
        case 0xCF: // SET 1,A
            return SET_r8(cpu, 1, A);
        case 0xD0: // SET 2,B
            return SET_r8(cpu, 2, B);
        case 0xD1: // SET 2,C
            return SET_r8(cpu, 2, C);
        case 0xD2: // SET 2,D
            return SET_r8(cpu, 2, D);
        case 0xD3: // SET 2,E
            return SET_r8(cpu, 2, E);
        case 0xD4: // SET 2,H
            return SET_r8(cpu, 2, H);
        case 0xD5: // SET 2,L
            return SET_r8(cpu, 2, L);
        case 0xD6: // SET 2,(HL)
            return SET_HL(cpu, 2);
        case 0xD7: // SET 2,A
            return SET_r8(cpu, 2, A);
        case 0xD8: // SET 3,B
            return SET_r8(cpu, 3, B);
        case 0xD9: // SET 3,C
            return SET_r8(cpu, 3, C);
        case 0xDA: // SET 3,D
            return SET_r8(cpu, 3, D);
        case 0xDB: // SET 3,E
            return SET_r8(cpu, 3, E);
        case 0xDC: // SET 3,H
            return SET_r8(cpu, 3, H);
        case 0xDD: // SET 3,L
            return SET_r8(cpu, 3, L);
        case 0xDE: // SET 3,(HL)
            return SET_HL(cpu, 3);
        case 0xDF: // SET 3,A
            return SET_r8(cpu, 3, A);
        case 0xE0: // SET 4,B
            return SET_r8(cpu, 4, B);
        case 0xE1: // SET 4,C
            return SET_r8(cpu, 4, C);
        case 0xE2: // SET 4,D
            return SET_r8(cpu, 4, D);
        case 0xE3: // SET 4,E
            return SET_r8(cpu, 4, E);
        case 0xE4: // SET 4,H
            return SET_r8(cpu, 4, H);
        case 0xE5: // SET 4,L
            return SET_r8(cpu, 4, L);
        case 0xE6: // SET 4,(HL)
            return SET_HL(cpu, 4);
        case 0xE7: // SET 4,A
            return SET_r8(cpu, 4, A);
        case 0xE8: // SET 5,B
            return SET_r8(cpu, 5, B);
        case 0xE9: // SET 5,C
            return SET_r8(cpu, 5, C);
        case 0xEA: // SET 5,D
            return SET_r8(cpu, 5, D);
        case 0xEB: // SET 5,E
            return SET_r8(cpu, 5, E);
        case 0xEC: // SET 5,H
            return SET_r8(cpu, 5, H);
        case 0xED: // SET 5,L
            return SET_r8(cpu, 5, L);
        case 0xEE: // SET 5,(HL)
            return SET_HL(cpu, 5);
        case 0xEF: // SET 5,A
            return SET_r8(cpu, 5, A);
        case 0xF0: // SET 6,B
            return SET_r8(cpu, 6, B);
        case 0xF1: // SET 6,C
            return SET_r8(cpu, 6, C);
        case 0xF2: // SET 6,D
            return SET_r8(cpu, 6, D);
        case 0xF3: // SET 6,E
            return SET_r8(cpu, 6, E);
        case 0xF4: // SET 6,H
            return SET_r8(cpu, 6, H);
        case 0xF5: // SET 6,L
            return SET_r8(cpu, 6, L);
        case 0xF6: // SET 6,(HL)
            return SET_HL(cpu, 6);
        case 0xF7: // SET 6,A
            return SET_r8(cpu, 6, A);
        case 0xF8: // SET 7,B
            return SET_r8(cpu, 7, B);
        case 0xF9: // SET 7,C
            return SET_r8(cpu, 7, C);
        case 0xFA: // SET 7,D
            return SET_r8(cpu, 7, D);
        case 0xFB: // SET 7,E
            return SET_r8(cpu, 7, E);
        case 0xFC: // SET 7,H
            return SET_r8(cpu, 7, H);
        case 0xFD: // SET 7,L
            return SET_r8(cpu, 7, L);
        case 0xFE: // SET 7,(HL)
            return SET_HL(cpu, 7);
        case 0xFF: // SET 7,A
            return SET_r8(cpu, 7, A);
        default:
            break;
        }

    }
    exit(1);
}

// =================================================================================
//                          Instructions
// =================================================================================


//https://rgbds.gbdev.io/docs/v0.9.4/gbz80.7


// Load instructions
int LD_r8_r8(cpu_t *cpu, reg_8bits_t reg_dest, reg_8bits_t reg_src)
{
    set_8bit_register(cpu, reg_dest, get_8bit_register(cpu, reg_src));
    return 1;
}
int LD_r8_n8(cpu_t *cpu, reg_8bits_t reg_dest) 
{
    uint8_t Z = read_memory(cpu, cpu->PC);
    cpu->PC ++;
    set_8bit_register(cpu, reg_dest, Z);
    return 2;
}

int LD_r16_n16(cpu_t *cpu, uint16_t *reg)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    *reg = unsigned_16(W, Z);
    return 3;
}
int LD_HL_r8(cpu_t *cpu, reg_8bits_t reg)
{
    write_memory(cpu, *cpu->registers->HL, get_8bit_register(cpu, reg));
    return 2;
}
int LD_HL_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    write_memory(cpu, *cpu->registers->HL, Z);
    return 3; 
}
int LD_r8_HL(cpu_t *cpu, reg_8bits_t reg_dest) 
{
    set_8bit_register(cpu, reg_dest, read_memory(cpu, *cpu->registers->HL));
    return 1;
}
int LD_r16_A(cpu_t *cpu, uint16_t *reg)
{
    write_memory(cpu, *reg, get_8bit_register(cpu, A));
    return 2;
}
int LD_n16_A(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    write_memory(cpu, unsigned_16(W,Z), get_8bit_register(cpu, A));
    return 4;    
}
int LDH_n8_A(cpu_t *cpu) 
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    write_memory(cpu, unsigned_16(0xFF,Z), get_8bit_register(cpu, A));
    return 3;
}
int LDH_C_A(cpu_t *cpu)
{
    write_memory(cpu, unsigned_16(0xFF, get_8bit_register(cpu, C)), get_8bit_register(cpu, A));
    cpu->PC++;
    return 2;
}
int LD_A_r16(cpu_t *cpu, uint16_t *reg)
{
    uint8_t Z = read_memory(cpu, *reg);
    set_8bit_register(cpu, A, Z);
   
    return 2; 
}
int LD_A_n16(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    Z = read_memory(cpu, unsigned_16(W,Z));
    set_8bit_register(cpu, A, Z);
    return 4;
}

int LDH_A_C(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, unsigned_16(0xFF, get_8bit_register(cpu, C)));
    set_8bit_register(cpu, A , Z);
    return 2;
}

int LDH_A_n16(cpu_t *cpu)
{
    uint8_t n = read_memory(cpu, cpu->PC); cpu->PC++;
    set_8bit_register(cpu, A, read_memory(cpu, unsigned_16(0xFF, n)));
    return 3;
}

int LD_HLI_A(cpu_t *cpu)
{
    write_memory(cpu, *cpu->registers->HL, get_8bit_register(cpu, A)); cpu->registers->HL++;
    return 2;
}
int LD_HLD_A(cpu_t *cpu)
{
    write_memory(cpu, *cpu->registers->HL, get_8bit_register(cpu, A)); cpu->registers->HL--;
    return 2;
}
int LD_A_HLI(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL); cpu->registers->HL++;
    set_8bit_register(cpu, A, Z);
    return 2;
}
int LD_A_HLD(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL); cpu->registers->HL--;
    set_8bit_register(cpu, A, Z);
    return 2;
}

// 8-bit arithmetic instructions


int ADC_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t a = get_8bit_register(cpu,A);
    uint8_t r = get_8bit_register(cpu,reg);
    uint8_t carry = get_flag(cpu, CARRY) ? 1 : 0;
    operation_result_t result = add(a, r + carry);
    set_8bit_register(cpu, A, result.result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else 
    {
        set_flag(cpu, HALF_CARRY, 0);   
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else 
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 0);
    return 1;
}
int ADC_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    uint8_t A = get_8bit_register(cpu, A);
    uint8_t carry = get_flag(cpu, CARRY) ? 1: 0;
    operation_result_t result = add(A, Z + carry);
    set_8bit_register(cpu, A, result .result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else 
    {
        set_flag(cpu, HALF_CARRY, 0);   
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else 
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 0);
    return 2;
}
int ADC_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = add(get_8bit_register(cpu, A), Z + (uint8_t)get_flag(cpu, CARRY));
    set_8bit_register(cpu, A, result.result);
    result.result == 0 ? set_flag(cpu, ZERO, 1) : set_flag(cpu, ZERO, 0);
    result.carry ? set_flag(cpu, CARRY, 1) : set_flag(cpu, CARRY, 0);
    result.halfcarry ? set_flag(cpu, HALF_CARRY, 1) : set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    return 2;
}
int ADD_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t a = get_8bit_register(cpu,A);
    uint8_t r = get_8bit_register(cpu,r);
    operation_result_t result = add(a, r);
    set_8bit_register(cpu, A, result.result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else 
    {
        set_flag(cpu, HALF_CARRY, 0);   
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else 
    {
        set_flag(cpu, CARRY, 0);   
    }
    return 1;
}
int ADD_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    uint8_t A = get_8bit_register(cpu, A);
    operation_result_t result = add(A, Z);
    set_8bit_register(cpu, A, result .result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);  
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else 
    {
        set_flag(cpu, HALF_CARRY, 0);   
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 0);
    return 2;
}
int ADD_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = add(get_8bit_register(cpu, A), Z);
    set_8bit_register(cpu, A, result.result);
    result.result == 0 ? set_flag(cpu, ZERO, 1) : set_flag(cpu, ZERO, 0);
    result.carry ? set_flag(cpu, CARRY, 1) : set_flag(cpu, CARRY, 0);
    result.halfcarry ? set_flag(cpu, HALF_CARRY, 1) : set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    return 2;

}
int CP_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t a = get_8bit_register(cpu,A);
    uint8_t r = get_8bit_register(cpu,reg);
    operation_result_t result = sub(a, r);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 1);
    return 1;
}
int CP_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    uint8_t A = get_8bit_register(cpu, A);
    operation_result_t result = sub(A, Z);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 1);
    return 2;
}
int CP_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = sub(get_8bit_register(cpu, A), Z);
    result.result == 0 ? set_flag(cpu, ZERO, 1) : set_flag(cpu, ZERO, 0);
    result.carry ? set_flag(cpu, CARRY, 1) : set_flag(cpu, CARRY, 0);
    result.halfcarry ? set_flag(cpu, HALF_CARRY, 1) : set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 1);
    return 2;
}
int DEC_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t r8 = get_8bit_register(cpu, reg);
    operation_result_t result = sub(r8, 1);
    if (result.halfcarry) 
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, SUB, 1);
    set_8bit_register(cpu, reg, result.result);
    return 1;
}
int DEC_HL(cpu_t *cpu)
{
    return DEC_r16(cpu, cpu->registers->HL);   
}
int INC_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t r8 = get_8bit_register(cpu, reg);
    operation_result_t result = add(r8, 1);
    if (result.halfcarry) 
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, SUB, 0);

    set_8bit_register(cpu, reg, result.result);
    return 1;
}
int INC_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    operation_result_t result = add(Z, 1);
    if (result.halfcarry) 
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);    
    }
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);    
    }
    set_flag(cpu, SUB, 0);
    write_memory(cpu, *cpu->registers->HL, result.result);
    return 3;

}
int SBC_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t a = get_8bit_register(cpu,A);
    uint8_t r = get_8bit_register(cpu,reg);
    uint8_t carry = get_flag(cpu, CARRY);
    operation_result_t result = sub(a, r + carry); // Peut etre un pb si r = 0xFF et carry =1 ?? TODO
    set_8bit_register(cpu, A, result.result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 1);
    return 1;
}
int SBC_HL(cpu_t *cpu)
{
    uint8_t a = get_8bit_register(cpu,A);
    uint8_t r = read_memory(cpu, *cpu->registers->HL);
    uint8_t carry = get_flag(cpu, CARRY);
    operation_result_t result = sub(a, r + carry); // Peut etre un pb si r = 0xFF et carry =1 ?? TODO
    set_8bit_register(cpu, A, result.result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 1);
    return 4;
}
int SBC_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t r = sub(A, Z + get_flag(cpu, CARRY));
    set_8bit_register(cpu, A, r.result);
    if (r.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (r.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (r.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 1);
    return 2; 
}
int SUB_r8(cpu_t *cpu, reg_8bits_t reg)
{
    operation_result_t result = sub(get_8bit_register(cpu, A), get_8bit_register(cpu, reg));
    set_8bit_register(cpu, A, result.result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 1);
    return 1;
}
int SUB_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    operation_result_t result = sub(get_8bit_register(cpu, A), Z);
    set_8bit_register(cpu, A, result.result);
    if (result.result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);   
    }
    if (result.halfcarry)
    {
        set_flag(cpu, HALF_CARRY, 1);
    } else
    {
        set_flag(cpu, HALF_CARRY, 0);   
    }
    if (result.carry)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);   
    }
    set_flag(cpu, SUB, 1);
    return 2;
}
int SUB_n8(cpu_t *cpu)
{
    uint8_t n = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = sub(get_8bit_register(cpu, A), n);
    set_8bit_register(cpu, A, (uint8_t)result.result);
    result.result == 0 ? set_flag(cpu, ZERO, 1) : set_flag(cpu, ZERO, 0);
    result.halfcarry ? set_flag(cpu, HALF_CARRY, 1) : set_flag(cpu, HALF_CARRY, 0);
    result.carry ? set_flag(cpu, CARRY, 1) : set_flag(cpu, CARRY, 0);
    set_flag(cpu, SUB, 1);
    return 2;
}
// 16-bit arithmetic instructions

int ADD_HL_r16(cpu_t *cpu, uint16_t *reg)
{
    operation_result_t result = add_16(*cpu->registers->HL, *reg);
    *cpu->registers->HL = result.result;
    set_flag(cpu, SUB, 0);
    result.halfcarry ? set_flag(cpu, HALF_CARRY, 1) : set_flag(cpu, HALF_CARRY, 0); 
    result.carry ? set_flag(cpu, CARRY, 1) : set_flag(cpu, CARRY, 0);
    return 2;
}

int DEC_r16(cpu_t *cpu, uint16_t *reg)
{
    *reg = *reg -1;
    return 2;
}



// Bitwise logic instructions

int AND_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t result = get_8bit_register(cpu, A) & get_8bit_register(cpu, reg);
    set_8bit_register(cpu, A, result);
    if (result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, HALF_CARRY, 1);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    return 1;
}

int AND_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    uint8_t result = get_8bit_register(cpu, A) & Z;
    set_8bit_register(cpu, A, result);
    if (result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, HALF_CARRY, 1);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    return 2;
}
int AND_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t result = get_8bit_register(cpu, A) & Z;
    result == 0 ? set_flag(cpu, ZERO, 1) : set_flag(cpu, ZERO, 0);
    set_flag(cpu, CARRY, 0);
    set_flag(cpu, HALF_CARRY, 1);
    set_flag(cpu, SUB, 0);
    return 2;
}

int CPL(cpu_t *cpu)
{
    set_8bit_register(cpu, A, ~get_8bit_register(cpu, A));
    set_flag(cpu, SUB, 1);
    set_flag(cpu, HALF_CARRY, 1);
    return 1;
}

int OR_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t result = get_8bit_register(cpu, A) | get_8bit_register(cpu, reg);
    set_8bit_register(cpu, A, result);
    if (result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, HALF_CARRY, 1);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    return 1;
}
int OR_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    uint8_t result = get_8bit_register(cpu, A) | Z;
    set_8bit_register(cpu, A, result);
    if (result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, HALF_CARRY, 1);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    return 2;

}
int OR_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t result = get_8bit_register(cpu, A) | Z;
    result == 0 ? set_flag(cpu, ZERO, 1) : set_flag(cpu, ZERO, 0);
    set_flag(cpu, CARRY, 0);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    return 2;
}
int XOR_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t result = get_8bit_register(cpu, A) ^ get_8bit_register(cpu, reg);
    set_8bit_register(cpu, A, result);
    if (result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, HALF_CARRY, 1);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    return 1;
}
int XOR_HL(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL);
    uint8_t result = get_8bit_register(cpu, A) ^ Z;
    set_8bit_register(cpu, A, result);
    if (result == 0)
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, HALF_CARRY, 1);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    return 2;

}
int XOR_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t result = get_8bit_register(cpu, A) ^ Z;
    result == 0 ? set_flag(cpu, ZERO, 1) : set_flag(cpu, ZERO, 0);
    set_flag(cpu, CARRY, 0);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    return 2;
}


// Bit flag instructions

int BIT_r8(cpu_t *cpu, reg_8bits_t reg, int b)
{
    uint8_t r = get_8bit_register(cpu, reg);
    if (bit(b, r) == 0) {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 1);
    return 2;
}
int BIT_HL(cpu_t *cpu, uint8_t b)
{
    uint8_t r = read_memory(cpu, *cpu->registers->HL);
    if (bit(b, r) == 0) {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);
    }
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 1);
    return 3;
}
int RES_r8(cpu_t *cpu, reg_8bits_t reg, uint8_t b)
{
    set_8bit_register(cpu, reg,(get_8bit_register(cpu, reg) & ~(1 << b)));
    return 2;
}
int RES_HL(cpu_t *cpu, uint8_t b)
{
    write_memory(cpu, *cpu->registers->HL, (read_memory(cpu, *cpu->registers->HL) | ~(1 << b)));
    return 4;
}
int SET_r8(cpu_t *cpu, reg_8bits_t reg, uint8_t b)
{
    set_8bit_register(cpu, reg,(get_8bit_register(cpu, reg) & (1 << b)));
    return 2;
}
int SET_HL(cpu_t *cpu, uint8_t b)
{
    write_memory(cpu, *cpu->registers->HL, (read_memory(cpu, *cpu->registers->HL) | (1 << b)));
    return 4;
}


// Bit shift instructions

int RL_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t old_r = get_8bit_register(cpu, reg);
    int b7 = bit(7, old_r);
    int carry = get_flag(cpu, CARRY);
    uint8_t new_r = (0x000000 | (old_r<<1)) |((uint8_t)(carry));
    set_8bit_register(cpu, reg, new_r);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b7 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_r == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 2;
}
int RL_HL(cpu_t *cpu)
{
    uint8_t old_HL = read_memory(cpu, *cpu->registers->HL);
    int b7 = bit(7, old_HL);
    int carry = get_flag(cpu, CARRY);
    uint8_t new_HL = (0x000000 | (old_HL<<1)) |((uint8_t)(carry));
    write_memory(cpu, *cpu->registers->HL, new_HL);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b7 == 1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_HL == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 4;
}
int RLA(cpu_t *cpu)
{
    uint8_t a = get_8bit_register(cpu, A);
    int b7 = bit(7, a);
    int carry = get_flag(cpu, CARRY);
    uint8_t new_A = (0x000000 | (a<<1)) |((uint8_t)(carry));
    set_8bit_register(cpu, A, new_A);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b7 == 1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    return 1;
}
int RLC_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t old_r = get_8bit_register(cpu, reg);
    int b7 = bit(7, old_r);
    uint8_t new_r = (0x000000 | (old_r<<1)) |((uint8_t)(b7));
    set_8bit_register(cpu, reg, new_r);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b7 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_r == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 2;    
}
int RLC_HL(cpu_t *cpu)
{
    uint8_t old_HL = read_memory(cpu, *cpu->registers->HL);
    int b7 = bit(7, old_HL);
    uint8_t new_HL = (0x000000 | (old_HL<<1)) |((uint8_t)(b7));
    write_memory(cpu, *cpu->registers->HL, new_HL);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b7 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_HL == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 4;
}
int RLCA(cpu_t *cpu)
{
    uint8_t a = get_8bit_register(cpu, A);
    int b7 = bit(7, a);
    uint8_t new_A = (0x000000 | (a<<1)) |((uint8_t)(b7));
    set_8bit_register(cpu, A, new_A);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b7 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    return 1;
}
int RR_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t old_r = get_8bit_register(cpu, reg);
    int b0 = bit(0, old_r);
    int carry = get_flag(cpu, CARRY);
    uint8_t new_r = (0x000000 | (old_r>>1)) |((uint8_t)(carry<<7));
    set_8bit_register(cpu, reg, new_r);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b0 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_r == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 2;  
}
int RR_HL(cpu_t *cpu)
{
    uint8_t old_HL = read_memory(cpu, *cpu->registers->HL);
    int b0 = bit(0, old_HL);
    int carry = get_flag(cpu, CARRY);
    uint8_t new_HL = (0x000000 | (old_HL>>1)) |((uint8_t)(carry<<7));
    write_memory(cpu, *cpu->registers->HL, new_HL);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b0 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_HL == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 4;
}
int RRA(cpu_t *cpu)
{
    uint8_t a = get_8bit_register(cpu, A);
    int b0 = bit(0, a);
    int carry = get_flag(cpu, CARRY);
    uint8_t new_A = (0x000000 | (a>>1)) |((uint8_t)(carry<<7));
    set_8bit_register(cpu, A, new_A);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b0 == 1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    return 1;
}
int RRC_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t old_r = get_8bit_register(cpu, reg);
    int b0 = bit(0, old_r);
    uint8_t new_r = (0x000000 | (old_r>>1)) |((uint8_t)(b0<<7));
    set_8bit_register(cpu, reg, new_r);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b0 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_r == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 2;  
}
int RRC_HL(cpu_t *cpu)
{
    uint8_t old_HL = read_memory(cpu, *cpu->registers->HL);
    int b0 = bit(0, old_HL);
    uint8_t new_HL = (0x000000 | (old_HL>>1)) |((uint8_t)(b0<<7));
    write_memory(cpu, *cpu->registers->HL, new_HL);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b0 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    if (new_HL == 0)
    {
        set_flag(cpu, ZERO, 1);
    }  else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 4;
}
int RRCA(cpu_t *cpu)
{
    uint8_t a = get_8bit_register(cpu, A);
    int b0 = bit(0, a);
    uint8_t new_A = (0x000000 | (a>>1)) |((uint8_t)(b0<<7));
    set_8bit_register(cpu, A, new_A);
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (b0 ==1) 
    {
        set_flag(cpu, CARRY, 1);
    } else {
        set_flag(cpu, CARRY, 0);
    }
    return 1;
}
int SLA_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t r = get_8bit_register(cpu, reg);
    int b7 = bit(7, r);
    r = r<<1;
    set_8bit_register(cpu, reg, r);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    if (r==0){
        set_flag(cpu, ZERO, 1); 
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    if (b7 == 0)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    return 2;
}
int SLA_HL(cpu_t *cpu)
{
    uint8_t hl = read_memory(cpu, *cpu->registers->HL);
    int b7 = bit(7, hl);
    hl = hl<<1;
    write_memory(cpu, *cpu->registers->HL, hl);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    if (hl==0){
        set_flag(cpu, ZERO, 1); 
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    if (b7 == 0)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    return 4;
}
int SRA_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t r = get_8bit_register(cpu, reg);
    int b0 = bit(0, r);
    r = r>>1;
    set_8bit_register(cpu, reg, r);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    if (r==0){
        set_flag(cpu, ZERO, 1); 
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    if (b0 == 0)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    return 2;
}
int SRA_HL(cpu_t *cpu)
{
    uint8_t hl = read_memory(cpu, *cpu->registers->HL);
    int b0 = bit(0, hl);
    hl = hl>>1;
    write_memory(cpu, *cpu->registers->HL, hl);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    if (hl==0){
        set_flag(cpu, ZERO, 1); 
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    if (b0 == 0)
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    return 4;
}
int SRL_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t r = get_8bit_register(cpu, reg);
    int b0 = bit(0, r);
    r = r>>1;
    set_8bit_register(cpu, reg, r);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (r == 0) 
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    if (b0 == 1) 
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    return 2;
}
int SRL_HL(cpu_t *cpu)
{
    uint8_t r = read_memory(cpu, *cpu->registers->HL);
    int b0 = bit(0, r);
    r = r>>1;
    write_memory(cpu, *cpu->registers->HL, r);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (r == 0) 
    {
        set_flag(cpu, ZERO, 1);
    } else
    {
        set_flag(cpu, ZERO, 0);
    }
    if (b0 == 1) 
    {
        set_flag(cpu, CARRY, 1);
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    return 4;
}
int SWAP_r8(cpu_t *cpu, reg_8bits_t reg)
{
    uint8_t r_value = get_8bit_register(cpu, reg);
    uint8_t high_nibble = r_value>>4;
    uint8_t low_nibble = r_value<<4;
    r_value = high_nibble | low_nibble;
    set_8bit_register(cpu, reg, r_value);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (r_value == 0) 
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 2;
}
int SWAP_HL(cpu_t *cpu) 
{
    uint8_t hl_value = read_memory(cpu, *cpu->registers->HL);
    uint8_t high_nibble = hl_value>>4;
    uint8_t low_nibble = hl_value<<4;
    hl_value = high_nibble | low_nibble;
    write_memory(cpu, *cpu->registers->HL, hl_value);
    set_flag(cpu, SUB, 0);
    set_flag(cpu, CARRY, 0);
    set_flag(cpu, HALF_CARRY, 0);
    if (hl_value == 0) 
    {
        set_flag(cpu, ZERO, 1);
    } else 
    {
        set_flag(cpu, ZERO, 0);
    }
    return 4;
}



// Jumps and subroutine instructions
int CALL_n16(cpu_t *cpu)
{
    uint8_t nn_lsb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t nn_msb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint16_t nn = unsigned_16(nn_msb, nn_lsb);
    cpu->SP--;
    write_memory(cpu, cpu->SP, msb(cpu->PC)); cpu->SP--;
    write_memory(cpu, cpu->SP, lsb(cpu->PC));
    cpu->PC = nn;
    return 6;
}

int CALL_cc_n16(cpu_t *cpu, bool cc)
{
    uint8_t nn_lsb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t nn_msb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint16_t nn = unsigned_16(nn_msb, nn_lsb);
    if (cc)
    {
        cpu->SP--;
        write_memory(cpu, cpu->SP, msb(cpu->PC)); cpu->SP--;
        write_memory(cpu, cpu->SP, lsb(cpu->PC));
        cpu->PC = nn;
        return 6;
    }
    return 3;
}

int JP_HL(cpu_t *cpu)
{
    cpu->PC = *cpu->registers->HL;
    return 1;
}
int JP_n16(cpu_t *cpu)
{
    uint8_t nn_lsb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t nn_msb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint16_t nn = unsigned_16(nn_msb, nn_lsb);
    cpu->PC = nn;
    return 4;
}
int JP_cc_n16(cpu_t *cpu, bool cc)
{
    uint8_t nn_lsb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t nn_msb = read_memory(cpu, cpu->PC); cpu->PC++;
    uint16_t nn = unsigned_16(nn_msb, nn_lsb);
    if (cc) 
    {
        cpu->PC = nn;
        return 4;
    }
    return 3;
}
int JR_e(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    bool Z_sign = bit(7, Z);
    operation_result_t result = add(Z, lsb(cpu->PC));
    Z = result.result;
    int8_t adj;
    if (result.carry && !Z_sign) {
        adj = 1; 
    } else if (!result.carry && Z_sign) {
        adj = -1;  
    } else {
        adj = 0;   
    }
    uint8_t W = msb(cpu->PC) + adj;
    cpu->PC = unsigned_16(W, Z);
    return 3;
}
int JR_cc_e(cpu_t *cpu, bool cc)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    bool Z_sign = bit(7, Z);
    operation_result_t result = add(Z, lsb(cpu->PC));
    Z = result.result;
    int8_t adj;
    if (result.carry && !Z_sign) {
        adj = 1; 
    } else if (!result.carry && Z_sign) {
        adj = -1;  
    } else {
        adj = 0;   
    }
    uint8_t W = msb(cpu->PC) + adj;
    if (cc){
        cpu->PC = unsigned_16(W, Z);
        return 3;
    }
    return 2;
}
int RET_cc(cpu_t *cpu, bool cc)
{
    if (cc)
    {
        uint8_t lsb = read_memory(cpu, cpu->SP); cpu->SP++;
        uint8_t msb = read_memory(cpu, cpu->SP); cpu->SP++;
        cpu->PC = unsigned_16(msb, lsb);
        return 5;
    }
    return 2;
}
int RET(cpu_t *cpu)
{
    uint8_t lsb = read_memory(cpu, cpu->SP); cpu->SP++;
    uint8_t msb = read_memory(cpu, cpu->SP); cpu->SP++;
    cpu->PC = unsigned_16(msb, lsb);
    return 4;
}
int RETI(cpu_t *cpu)
{
    uint8_t lsb = read_memory(cpu, cpu->SP); cpu->SP++;
    uint8_t msb = read_memory(cpu, cpu->SP); cpu->SP++;
    cpu->PC = unsigned_16(msb, lsb);
    cpu->IME = true;
    return 4;
}
int RST_n(cpu_t *cpu, uint8_t n)
{
    cpu->SP--;
    write_memory(cpu, cpu->SP, msb(cpu->PC)); cpu->SP--;
    write_memory(cpu, cpu->SP, lsb(cpu->PC));
    cpu->PC = unsigned_16(0x00, n);
    return 4;
}

// Carry flag instructions
int CCF(cpu_t *cpu)
{
    int current_carry = get_flag(cpu, CARRY);
    set_flag(cpu, CARRY, !current_carry);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    return 1;
}
int SCF(cpu_t *cpu)
{
    set_flag(cpu, CARRY, 1);
    set_flag(cpu, HALF_CARRY, 0);
    set_flag(cpu, SUB, 0);
    return 1;
}


// Stack manipulation instructions
int ADD_SP_e8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = add(lsb(cpu->SP), Z);
    if (result.carry) {
        set_flag(cpu, CARRY, 1);    
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    if (result.halfcarry) {
        set_flag(cpu, HALF_CARRY, 1);    
    } else 
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    uint8_t adj;
    if (bit(7, Z)) 
    {
        adj = 0xFF;
    } else 
    {
        adj = 0x00;
    }
    uint8_t W = msb(cpu->SP) + adj + get_flag(cpu, CARRY);
    uint16_t WZ = unsigned_16(W, Z);
    cpu->SP = WZ;
    return 4;
}
int DEC_SP(cpu_t *cpu)
{
    return DEC_r16(cpu, cpu->registers->SP);
}


int INC_r16(cpu_t *cpu, uint16_t *reg)
{
    (*reg)++;
    return 2;
}

int LD_n16_SP(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    uint16_t WZ = unsigned_16(W,Z);

    write_memory(cpu, WZ, lsb(cpu->SP)); WZ++;
    write_memory(cpu, WZ, msb(cpu->SP));
    return 5;
}
int LD_SP_HL(cpu_t *cpu)
{
    cpu->SP = *cpu->registers->HL;
    return 2;
}
int LD_HL_SP_e8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = add(lsb(cpu->SP), Z);
    set_8bit_register(cpu, L, result.result);
    if (result.carry) {
        set_flag(cpu, CARRY, 1);    
    } else
    {
        set_flag(cpu, CARRY, 0);
    }
    if (result.halfcarry) {
        set_flag(cpu, HALF_CARRY, 1);    
    } else 
    {
        set_flag(cpu, HALF_CARRY, 0);
    }
    set_flag(cpu, ZERO, 0);
    set_flag(cpu, SUB, 0);
    uint8_t adj;
    if (bit(7, Z)) 
    {
        adj = 0xFF;
    } else 
    {
        adj = 0x00;
    }
    uint8_t r = msb(cpu->SP) + adj + get_flag(cpu, CARRY);
    set_8bit_register(cpu, H, r);
    return 3;
}
int POP_r16(cpu_t *cpu, uint16_t *reg)
{
    uint8_t Z = read_memory(cpu, cpu->SP); cpu->SP++;
    uint8_t W = read_memory(cpu, cpu->SP); cpu->SP++;
    *reg = unsigned_16(W,Z);
    return 3;
}
int PUSH_r16(cpu_t *cpu, uint16_t *reg)
{
    cpu->SP--;
    write_memory(cpu, cpu->SP, msb(*reg)); cpu->SP--;
    write_memory(cpu, cpu->SP, lsb(*reg));
    return 4;
}

// Interrupt-related instructions
int DI(cpu_t *cpu)
{
    cpu->IME = false;
    cpu->IME_pending = false;
    return 1;
}
int EI(cpu_t *cpu)
{
    cpu->IME_pending = true;
    return 1; 
}
int HALT(cpu_t *cpu); //TODO


// Miscellaneous instructions
int DAA(cpu_t *cpu); //TODO
int NOP(cpu_t *cpu)
{
    return 1;
}
int STOP(cpu_t *cpu); //No licensed rom makes use of STOP outside of CGB speed switching



// =================================================================================
//                          Utils
// =================================================================================


uint16_t unsigned_16(uint8_t msb, uint8_t lsb)
{
    return (((uint16_t) msb)<<8) & lsb; 
}

uint8_t lsb(uint16_t u) 
{
    return (uint8_t) (u & 0x00FF);
}

uint8_t msb(uint16_t u) 
{
    return (uint8_t) (u>>8);
}


operation_result_t add(uint8_t a, uint8_t b)
{
    uint8_t result = a + b;
    uint16_t result16 = (uint16_t) a + (uint16_t) b;
    bool carry = result16 > UINT8_MAX; 
    bool halfcarry = ((a & 0x0F) +  (b & 0x0F))>0x0F;
    operation_result_t output = {
        result,
        carry,
        halfcarry
    };
    return output;
}

operation_result_t add_16(uint16_t a, uint16_t b)
{
    uint16_t result = a + b;
    uint32_t result32 = (uint32_t) a + (uint32_t) b;
    bool carry = result32 > UINT16_MAX; 
    bool halfcarry = ((a & 0x00FF) + (b & 0x00FF))>0x00FF;
    operation_result_t output = {
        result,
        carry,
        halfcarry
    };
    return output;
}

operation_result_t sub(uint8_t a, uint8_t b)
{
    uint8_t result = a - b;
    bool carry = b > a;
    bool halfcarry = (b & 0x0F) > (a & 0x0F);
    
    operation_result_t output = {
        result,
        carry,
        halfcarry
    };
    return output;
}
/**
 * @i: bit position in the uint_8
 */
int bit(int i, uint8_t n)
{
    if (i > 7)
    {
        printf("Trying to access %dth bit of an uint8", i);
        exit(1);
    }
    return (int) ((n>>i) & 1);
}
