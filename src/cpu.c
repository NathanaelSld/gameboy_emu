#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"




uint8_t get_8bit_register(registers_t *registers, reg_8bits_t reg_8bits) 
{
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

void set_8bit_register(registers_t *registers, reg_8bits_t reg_8bits, uint8_t value) 
{
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


int get_flag(registers_t *reg, flag_t flag)
{
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

void set_flag(registers_t *reg, flag_t flag, int value)
{
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
    
    uint8_t instruction_byte = read_memory(cpu, cpu->PC);
    bool prefixed = instruction_byte == 0xCB;
    if (prefixed) 
    {
        instruction_byte = read_memory(cpu, cpu->PC + 1);
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
        //case 0x00: // NOP
        //case 0x01: // LD BC,nn
        //case 0x02: // LD (BC),A
        //case 0x03: // INC BC
        //case 0x04: // INC B
        //case 0x05: // DEC B
        //case 0x06: // LD B,n
        //case 0x07: // RLCA
        //case 0x08: // LD (nn),SP
        //case 0x09: // ADD HL,BC
        //case 0x0A: // LD A,(BC)
        //case 0x0B: // DEC BC
        //case 0x0C: // INC C
        //case 0x0D: // DEC C
        //case 0x0E: // LD C,n
        //case 0x0F: // RRCA
        //case 0x10: // STOP
        //case 0x11: // LD DE,nn
        //case 0x12: // LD (DE),A
        //case 0x13: // INC DE
        //case 0x14: // INC D
        //case 0x15: // DEC D
        //case 0x16: // LD D,n
        //case 0x17: // RLA
        //case 0x18: // JR n
        //case 0x19: // ADD HL,DE
        //case 0x1A: // LD A,(DE)
        //case 0x1B: // DEC DE
        //case 0x1C: // INC E
        //case 0x1D: // DEC E
        //case 0x1E: // LD E,n
        //case 0x1F: // RRA
        //case 0x20: // JR NZ,n
        //case 0x21: // LD HL,nn
        //case 0x22: // LD (HL+),A
        //case 0x23: // INC HL
        //case 0x24: // INC H
        //case 0x25: // DEC H
        //case 0x26: // LD H,n
        //case 0x27: // DAA
        //case 0x28: // JR Z,n
        //case 0x29: // ADD HL,HL
        //case 0x2A: // LD A,(HL+)
        //case 0x2B: // DEC HL
        //case 0x2C: // INC L
        //case 0x2D: // DEC L
        //case 0x2E: // LD L,n
        //case 0x2F: // CPL
        //case 0x30: // JR NC,n
        //case 0x31: // LD SP,nn
        //case 0x32: // LD (HL-),A
        //case 0x33: // INC SP
        //case 0x34: // INC (HL)
        //case 0x35: // DEC (HL)
        //case 0x36: // LD (HL),n
        //case 0x37: // SCF
        //case 0x38: // JR C,n
        //case 0x39: // ADD HL,SP   
        //case 0x3A: // LD A,(HL-)
        //case 0x3B: // DEC SP
        //case 0x3C: // INC A
        //case 0x3D: // DEC A
        //case 0x3E: // LD A,n
        //case 0x3F: // CCF
        //case 0x40: // LD B,B
        //case 0x41: // LD B,C
        //case 0x42: // LD B,D
        //case 0x43: // LD B,E
        //case 0x44: // LD B,H
        //case 0x45: // LD B,L
        //case 0x46: // LD B,(HL)
        //case 0x47: // LD B,A
        //case 0x48: // LD C,B
        //case 0x49: // LD C,C
        //case 0x4A: // LD C,D
        //case 0x4B: // LD C,E
        //case 0x4C: // LD C,H
        //case 0x4D: // LD C,L
        //case 0x4E: // LD C,(HL)
        //case 0x4F: // LD C,A
        //case 0x50: // LD D,B
        //case 0x51: // LD D,C
        //case 0x52: // LD D,D
        //case 0x53: // LD D,E
        //case 0x54: // LD D,H
        //case 0x55: // LD D,L
        //case 0x56: // LD D,(HL)
        //case 0x57: // LD D,A
        //case 0x58: // LD E,B
        //case 0x59: // LD E,C
        //case 0x5A: // LD E,D
        //case 0x5B: // LD E,E
        //case 0x5C: // LD E,H
        //case 0x5D: // LD E,L
        //case 0x5E: // LD E,(HL)
        //case 0x5F: // LD E,A
        //case 0x60: // LD H,B
        //case 0x61: // LD H,C
        //case 0x62: // LD H,D
        //case 0x63: // LD H,E
        //case 0x64: // LD H,H
        //case 0x65: // LD H,L
        //case 0x66: // LD H,(HL)
        //case 0x67: // LD H,A
        //case 0x68: // LD L,B
        //case 0x69: // LD L,C
        //case 0x6A: // LD L,D
        //case 0x6B: // LD L,E
        //case 0x6C: // LD L,H
        //case 0x6D: // LD L,L
        //case 0x6E: // LD L,(HL)
        //case 0x6F: // LD L,A
        //case 0x70: // LD (HL),B
        //case 0x71: // LD (HL),C
        //case 0x72: // LD (HL),D
        //case 0x73: // LD (HL),E
        //case 0x74: // LD (HL),H
        //case 0x75: // LD (HL),L
        //case 0x76: // HALT
        //case 0x77: // LD (HL),A
        //case 0x78: // LD A,B
        //case 0x79: // LD A,C
        //case 0x7A: // LD A,D
        //case 0x7B: // LD A,E
        //case 0x7C: // LD A,H
        //case 0x7D: // LD A,L
        //case 0x7E: // LD A,(HL)
        //case 0x7F: // LD A,A
        //case 0x80: // ADD A,B
        //case 0x81: // ADD A,C
        //case 0x82: // ADD A,D
        //case 0x83: // ADD A,E
        //case 0x84: // ADD A,H
        //case 0x85: // ADD A,L
        //case 0x86: // ADD A,(HL)
        //case 0x87: // ADD A,A
        //case 0x88: // ADC A,B
        //case 0x89: // ADC A,C
        //case 0x8A: // ADC A,D
        //case 0x8B: // ADC A,E
        //case 0x8C: // ADC A,H
        //case 0x8D: // ADC A,L
        //case 0x8E: // ADC A,(HL)
        //case 0x8F: // ADC A,A
        //case 0x90: // SUB B
        //case 0x91: // SUB C
        //case 0x92: // SUB D
        //case 0x93: // SUB E
        //case 0x94: // SUB H
        //case 0x95: // SUB L
        //case 0x96: // SUB (HL)
        //case 0x97: // SUB A
        //case 0x98: // SBC A,B
        //case 0x99: // SBC A,C
        //case 0x9A: // SBC A,D
        //case 0x9B: // SBC A,E
        //case 0x9C: // SBC A,H
        //case 0x9D: // SBC A,L
        //case 0x9E: // SBC A,(HL)
        //case 0x9F: // SBC A,A
        //case 0xA0: // AND A,B
        //case 0xA1: // AND A,C
        //case 0xA2: // AND A,D
        //case 0xA3: // AND A,E
        //case 0xA4: // AND A,H
        //case 0xA5: // AND A,L
        //case 0xA6: // AND A,(HL)
        //case 0xA7: // AND A,A
        //case 0xA8: // XOR A,B
        //case 0xA9: // XOR A,C
        //case 0xAA: // XOR A,D
        //case 0xAB: // XOR A,E
        //case 0xAC: // XOR A,H
        //case 0xAD: // XOR A,L
        //case 0xAE: // XOR A,(HL)
        //case 0xAF: // XOR A,A 
        //case 0xB0: // OR A,B
        //case 0xB1: // OR A,C
        //case 0xB2: // OR A,D
        //case 0xB3: // OR A,E
        //case 0xB4: // OR A,H
        //case 0xB5: // OR A,L  
        //case 0xB6: // OR A,(HL)
        //case 0xB7: // OR A,A
        //case 0xB8: // CP A,B
        //case 0xB9: // CP A,C
        //case 0xBA: // CP A,D
        //case 0xBB: // CP A,E
        //case 0xBC: // CP A,H
        //case 0xBD: // CP A,L
        //case 0xBE: // CP A,(HL)
        //case 0xBF: // CP A,A
        //case 0xC0: // RET NZ
        //case 0xC1: // POP BC
        //case 0xC2: // JP NZ,nn
        //case 0xC3: // JP nn
        //case 0xC4: // CALL NZ,nn
        //case 0xC5: // PUSH BC
        //case 0xC6: // ADD A,n
        //case 0xC7: // RST 00H
        //case 0xC8: // RET Z
        //case 0xC9: // RET
        //case 0xCA: // JP Z,nn
        //case 0xCB: // PREFIX
        //case 0xCC: // CALL Z,nn
        //case 0xCD: // CALL nn
        //case 0xCE: // ADC A,n
        //case 0xCF: // RST 08H
        //case 0xD0: // RET NC
        //case 0xD1: // POP DE
        //case 0xD2: // JP NC,nn
        //case 0xD3: // Blank
        //case 0xD4: // CALL NC,nn
        //case 0xD5: // PUSH DE
        //case 0xD6: // SUB A,n
        //case 0xD7: // RST 10H
        //case 0xD8: // RET C
        //case 0xD9: // RETI
        //case 0xDA: // JP C,nn
        //case 0xDB: // Blank
        //case 0xDC: // CALL C,nn
        //case 0xDD: // Blank
        //case 0xDE: // SBC A,n
        //case 0xDF: // RST 18H
        //case 0xE0: // LDH (n),A
        //case 0xE1: // POP HL
        //case 0xE2: // LD (C),A
        //case 0xE3: // Blank
        //case 0xE4: // Blank
        //case 0xE5: // PUSH HL
        //case 0xE6: // AND A,n
        //case 0xE7: // RST 20H
        //case 0xE8: // ADD SP,n
        //case 0xE9: // JP (HL)
        //case 0xEA: // LD (nn),A
        //case 0xEB: // Blank
        //case 0xEC: // Blank
        //case 0xED: // Blank
        //case 0xEE: // XOR A,n
        //case 0xEF: // RST 28H
        //case 0xF0: // LDH A,(n)
        //case 0xF1: // POP AF
        //case 0xF2: // LD A,(C)
        //case 0xF3: // DI
        //case 0xF4: // Blank
        //case 0xF5: // PUSH AF
        //case 0xF6: // OR A,n
        //case 0xF7: // RST 30H
        //case 0xF8: // LD HL,SP+n
        //case 0xF9: // LD SP,HL
        //case 0xFA: // LD A,(nn)
        //case 0xFB: // EI
        //case 0xFC: // Blank
        //case 0xFD: // Blank
        //case 0xFE: // CP A,n
        //case 0xFF: // RST 38H
        
        default:
            break;
        }
    } else 
    {
        switch (instruction_byte)
        {
        //case 0x00: // RLC B
        //case 0x01: // RLC C
        //case 0x02: // RLC D
        //case 0x03: // RLC E
        //case 0x04: // RLC H
        //case 0x05: // RLC L
        //case 0x06: // RLC (HL)
        //case 0x07: // RLC A
        //case 0x08: // RRC B
        //case 0x09: // RRC C
        //case 0x0A: // RRC D
        //case 0x0B: // RRC E
        //case 0x0C: // RRC H
        //case 0x0D: // RRC L
        //case 0x0E: // RRC (HL)
        //case 0x0F: // RRC A
        //case 0x10: // RL B
        //case 0x11: // RL C
        //case 0x12: // RL D
        //case 0x13: // RL E
        //case 0x14: // RL H
        //case 0x15: // RL L
        //case 0x16: // RL (HL)
        //case 0x17: // RL A
        //case 0x18: // RR B
        //case 0x19: // RR C
        //case 0x1A: // RR D
        //case 0x1B: // RR E
        //case 0x1C: // RR H
        //case 0x1D: // RR L
        //case 0x1E: // RR (HL)
        //case 0x1F: // RR A
        //case 0x20: // SLA B
        //case 0x21: // SLA C
        //case 0x22: // SLA D
        //case 0x23: // SLA E
        //case 0x24: // SLA H
        //case 0x25: // SLA L
        //case 0x26: // SLA (HL)
        //case 0x27: // SLA A
        //case 0x28: // SRA B
        //case 0x29: // SRA C
        //case 0x2A: // SRA D
        //case 0x2B: // SRA E
        //case 0x2C: // SRA H
        //case 0x2D: // SRA L
        //case 0x2E: // SRA (HL)
        //case 0x2F: // SRA A
        //case 0x30: // SWAP B
        //case 0x31: // SWAP C
        //case 0x32: // SWAP D
        //case 0x33: // SWAP E
        //case 0x34: // SWAP H
        //case 0x35: // SWAP L
        //case 0x36: // SWAP (HL)
        //case 0x37: // SWAP A
        //case 0x38: // SRL B
        //case 0x39: // SRL C
        //case 0x3A: // SRL D
        //case 0x3B: // SRL E
        //case 0x3C: // SRL H
        //case 0x3D: // SRL L
        //case 0x3E: // SRL (HL)
        //case 0x3F: // SRL A
        //case 0x3E: // SRL (HL)
        //case 0x3F: // SRL A
        //case 0x40: // BIT 0,B
        //case 0x41: // BIT 0,C
        //case 0x42: // BIT 0,D
        //case 0x43: // BIT 0,E
        //case 0x44: // BIT 0,H
        //case 0x45: // BIT 0,L
        //case 0x46: // BIT 0,(HL)
        //case 0x47: // BIT 0,A
        //case 0x48: // BIT 1,B
        //case 0x49: // BIT 1,C
        //case 0x4A: // BIT 1,D
        //case 0x4B: // BIT 1,E
        //case 0x4C: // BIT 1,H
        //case 0x4D: // BIT 1,L
        //case 0x4E: // BIT 1,(HL)
        //case 0x4F: // BIT 1,A
        //case 0x50: // BIT 2,B
        //case 0x51: // BIT 2,C
        //case 0x52: // BIT 2,D
        //case 0x53: // BIT 2,E
        //case 0x54: // BIT 2,H
        //case 0x55: // BIT 2,L
        //case 0x56: // BIT 2,(HL)
        //case 0x57: // BIT 2,A
        //case 0x58: // BIT 3,B
        //case 0x59: // BIT 3,C
        //case 0x5A: // BIT 3,D
        //case 0x5B: // BIT 3,E
        //case 0x5C: // BIT 3,H
        //case 0x5D: // BIT 3,L
        //case 0x5E: // BIT 3,(HL)
        //case 0x5F: // BIT 3,A
        //case 0x60: // BIT 4,B
        //case 0x61: // BIT 4,C
        //case 0x62: // BIT 4,D
        //case 0x63: // BIT 4,E
        //case 0x64: // BIT 4,H
        //case 0x65: // BIT 4,L
        //case 0x66: // BIT 4,(HL)
        //case 0x67: // BIT 4,A
        //case 0x68: // BIT 5,B
        //case 0x69: // BIT 5,C
        //case 0x6A: // BIT 5,D
        //case 0x6B: // BIT 5,E
        //case 0x6C: // BIT 5,H
        //case 0x6D: // BIT 5,L
        //case 0x6E: // BIT 5,(HL)
        //case 0x6F: // BIT 5,A
        //case 0x70: // BIT 6,B
        //case 0x71: // BIT 6,C
        //case 0x72: // BIT 6,D
        //case 0x73: // BIT 6,E
        //case 0x74: // BIT 6,H
        //case 0x75: // BIT 6,L
        //case 0x76: // BIT 6,(HL)
        //case 0x77: // BIT 6,A
        //case 0x78: // BIT 7,B
        //case 0x79: // BIT 7,C
        //case 0x7A: // BIT 7,D
        //case 0x7B: // BIT 7,E
        //case 0x7C: // BIT 7,H
        //case 0x7D: // BIT 7,L
        //case 0x7E: // BIT 7,(HL)
        //case 0x7F: // BIT 7,A
        //case 0x80: // RES 0,B
        //case 0x81: // RES 0,C
        //case 0x82: // RES 0,D
        //case 0x83: // RES 0,E
        //case 0x84: // RES 0,H
        //case 0x85: // RES 0,L
        //case 0x86: // RES 0,(HL)
        //case 0x87: // RES 0,A
        //case 0x88: // RES 1,B
        //case 0x89: // RES 1,C
        //case 0x8A: // RES 1,D
        //case 0x8B: // RES 1,E
        //case 0x8C: // RES 1,H
        //case 0x8D: // RES 1,L
        //case 0x8E: // RES 1,(HL)
        //case 0x8F: // RES 1,A
        //case 0x90: // RES 2,B
        //case 0x91: // RES 2,C
        //case 0x92: // RES 2,D
        //case 0x93: // RES 2,E
        //case 0x94: // RES 2,H
        //case 0x95: // RES 2,L
        //case 0x96: // RES 2,(HL)
        //case 0x97: // RES 2,A
        //case 0x98: // RES 3,B
        //case 0x99: // RES 3,C
        //case 0x9A: // RES 3,D
        //case 0x9B: // RES 3,E
        //case 0x9C: // RES 3,H
        //case 0x9D: // RES 3,L
        //case 0x9E: // RES 3,(HL)
        //case 0x9F: // RES 3,A
        //case 0xA0: // RES 4,B
        //case 0xA1: // RES 4,C
        //case 0xA2: // RES 4,D
        //case 0xA3: // RES 4,E
        //case 0xA4: // RES 4,H
        //case 0xA5: // RES 4,L
        //case 0xA6: // RES 4,(HL)
        //case 0xA7: // RES 4,A
        //case 0xA8: // RES 5,B
        //case 0xA9: // RES 5,C
        //case 0xAA: // RES 5,D
        //case 0xAB: // RES 5,E
        //case 0xAC: // RES 5,H
        //case 0xAD: // RES 5,L
        //case 0xAE: // RES 5,(HL)
        //case 0xAF: // RES 5,A
        //case 0xB0: // RES 6,B
        //case 0xB1: // RES 6,C
        //case 0xB2: // RES 6,D
        //case 0xB3: // RES 6,E
        //case 0xB4: // RES 6,H
        //case 0xB5: // RES 6,L
        //case 0xB6: // RES 6,(HL)
        //case 0xB7: // RES 6,A
        //case 0xB8: // RES 7,B
        //case 0xB9: // RES 7,C
        //case 0xBA: // RES 7,D
        //case 0xBB: // RES 7,E
        //case 0xBC: // RES 7,H
        //case 0xBD: // RES 7,L
        //case 0xBE: // RES 7,(HL)
        //case 0xBF: // RES 7,A
        //case 0xC0: // SET 0,B
        //case 0xC1: // SET 0,C
        //case 0xC2: // SET 0,D
        //case 0xC3: // SET 0,E
        //case 0xC4: // SET 0,H
        //case 0xC5: // SET 0,L
        //case 0xC6: // SET 0,(HL)
        //case 0xC7: // SET 0,A
        //case 0xC8: // SET 1,B
        //case 0xC9: // SET 1,C
        //case 0xCA: // SET 1,D
        //case 0xCB: // SET 1,E
        //case 0xCC: // SET 1,H
        //case 0xCD: // SET 1,L
        //case 0xCE: // SET 1,(HL)
        //case 0xCF: // SET 1,A
        //case 0xD0: // SET 2,B
        //case 0xD1: // SET 2,C
        //case 0xD2: // SET 2,D
        //case 0xD3: // SET 2,E
        //case 0xD4: // SET 2,H
        //case 0xD5: // SET 2,L
        //case 0xD6: // SET 2,(HL)
        //case 0xD7: // SET 2,A
        //case 0xD8: // SET 3,B
        //case 0xD9: // SET 3,C
        //case 0xDA: // SET 3,D
        //case 0xDB: // SET 3,E
        //case 0xDC: // SET 3,H
        //case 0xDD: // SET 3,L
        //case 0xDE: // SET 3,(HL)
        //case 0xDF: // SET 3,A
        //case 0xE0: // SET 4,B
        //case 0xE1: // SET 4,C
        //case 0xE2: // SET 4,D
        //case 0xE3: // SET 4,E
        //case 0xE4: // SET 4,H
        //case 0xE5: // SET 4,L
        //case 0xE6: // SET 4,(HL)
        //case 0xE7: // SET 4,A
        //case 0xE8: // SET 5,B
        //case 0xE9: // SET 5,C
        //case 0xEA: // SET 5,D
        //case 0xEB: // SET 5,E
        //case 0xEC: // SET 5,H
        //case 0xED: // SET 5,L
        //case 0xEE: // SET 5,(HL)
        //case 0xEF: // SET 5,A
        //case 0xF0: // SET 6,B
        //case 0xF1: // SET 6,C
        //case 0xF2: // SET 6,D
        //case 0xF3: // SET 6,E
        //case 0xF4: // SET 6,H
        //case 0xF5: // SET 6,L
        //case 0xF6: // SET 6,(HL)
        //case 0xF7: // SET 6,A
        //case 0xF8: // SET 7,B
        //case 0xF9: // SET 7,C
        //case 0xFA: // SET 7,D
        //case 0xFB: // SET 7,E
        //case 0xFC: // SET 7,H
        //case 0xFD: // SET 7,L
        //case 0xFE: // SET 7,(HL)
        //case 0xFF: // SET 7,A
        default:
            break;
        }

    }
}

// =================================================================================
//                          Instructions
// =================================================================================


//https://rgbds.gbdev.io/docs/v0.9.4/gbz80.7


// Load instructions
int LD_r8_r8(cpu_t *cpu, reg_8bits_t reg_dest, reg_8bits_t reg_src)
{
    set_8bit_register(cpu->registers, reg_dest, get_8bit_register(cpu->registers, reg_src));
    cpu->PC ++;
    return 1;
}
int LD_r8_n8(cpu_t *cpu, reg_8bits_t reg_dest) 
{
    uint8_t Z = read_memory(cpu, cpu->PC);
    cpu->PC ++;
    set_8bit_register(cpu->registers, reg_dest, Z);
    cpu->PC ++;
    return 2;
}
int LD_r16_n16(cpu_t *cpu, uint16_t *reg)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    cpu->PC ++;
    *reg = unsigned_16(W, Z);
    return 3;
}
int LD_HL_r8(cpu_t *cpu, reg_8bits_t reg)
{
    write_memory(cpu, *cpu->registers->HL, get_8bit_register(cpu->registers, reg));
    cpu->PC++;
    return 2;
}
int LD_HL_n8(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    write_memory(cpu, *cpu->registers->HL, Z);
    cpu->PC++;
    return 3; 
}
int LD_r8_HL(cpu_t *cpu, reg_8bits_t reg_dest) 
{
    set_8bit_register(cpu->registers, reg_dest, read_memory(cpu, *cpu->registers->HL));
    cpu->PC ++;
    return 1;
}
int LD_r16_A(cpu_t *cpu, uint16_t *reg)
{
    write_memory(cpu, *reg, get_8bit_register(cpu->registers, A));
    cpu->PC++;
    return 2;
}
int LD_n16_A(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    write_memory(cpu, unsigned_16(W,Z), get_8bit_register(cpu->registers, A));
    cpu->PC ++;
    return 4;    
}
int LDH_n16_A(cpu_t *cpu) 
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    write_memory(cpu, unsigned_16(0xFF,Z), get_8bit_register(cpu->registers, A));
    cpu->PC++;
    return 3;
}
int LDH_C_A(cpu_t *cpu)
{
    write_memory(cpu, unsigned_16(0xFF, get_8bit_register(cpu->registers, C)), get_8bit_register(cpu->registers, A));
    cpu->PC++;
    return 2;
}
int LD_A_r16(cpu_t *cpu, uint16_t *reg)
{
    uint8_t Z = read_memory(cpu, *reg);
    set_8bit_register(cpu->registers, A, Z);
    cpu->PC ++;
    return 2; 
}
int LD_A_n16(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    Z = read_memory(cpu, unsigned_16(W,Z));
    cpu->PC ++;
    set_8bit_register(cpu->registers, A, Z);
    return 4;
}
int LDH_A_n16(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    Z = read_memory(cpu, unsigned_16(0xFF,Z));
    cpu->PC++;
    set_8bit_register(cpu->registers, A, Z);
    return 3;
}
int LDH_A_C(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, unsigned_16(0xFF, get_8bit_register(cpu->registers, C)));
    cpu->PC++;
    set_8bit_register(cpu->registers, A , Z);
    return 2;
}
int LD_HLI_A(cpu_t *cpu)
{
    write_memory(cpu, *cpu->registers->HL, get_8bit_register(cpu->registers, A)); cpu->registers->HL++;
    cpu->PC++;
    return 2;
}
int LD_HLD_A(cpu_t *cpu)
{
    write_memory(cpu, *cpu->registers->HL, get_8bit_register(cpu->registers, A)); cpu->registers->HL--;
    cpu->PC++;
    return 2;
}
int LD_A_HLI(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL); cpu->registers->HL++;
    cpu->PC++; set_8bit_register(cpu->registers, A, Z);
    return 2;
}
int LD_A_HLD(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, *cpu->registers->HL); cpu->registers->HL--;
    cpu->PC++; set_8bit_register(cpu->registers, A, Z);
    return 2;
}

// 8-bit arithmetic instructions


int ADC_r8(cpu_t *cpu, reg_8bits_t reg);
int ADC_HL(cpu_t *cpu);
int ADC_n8(cpu_t *cpu, uint8_t value);
int ADD_r8(cpu_t *cpu, reg_8bits_t reg);
int ADD_HL(cpu_t *cpu);
int ADD_n8(cpu_t *cpu, uint8_t value);
int CP_A_HL(cpu_t *cpu);
int CP_A_n8(cpu_t *cpu, reg_8bits_t reg);
int DEC_r8(cpu_t *cpu, reg_8bits_t reg);
int DEC_HL(cpu_t *cpu);
int INC_r8(cpu_t *cpu, reg_8bits_t reg);
int INC_HL(cpu_t *cpu);
int SBC_A_r8(cpu_t *cpu, reg_8bits_t reg);
int SBC_A_HL(cpu_t *cpu);
int SBC_A_n8(cpu_t *cpu, uint8_t value);
int SUB_r8(cpu_t *cpu, reg_8bits_t reg);
int SUB_A_HL(cpu_t *cpu);
int SUB_A_n8(cpu_t *cpu, uint8_t value);
// 16-bit arithmetic instructions

int ADD_HL_r16(cpu_t *cpu, uint16_t *reg);
int DEC_r16(cpu_t *cpu, uint16_t *reg);
int INC_r16(cpu_t *cpu, uint16_t *reg);

// Bitwise logic instructions

int AND_r8(cpu_t *cpu, reg_8bits_t reg);
int AND_HL(cpu_t *cpu);
int AND_n8(cpu_t *cpu, uint8_t value);
int CPL(cpu_t *cpu);
int OR_r8(cpu_t *cpu, reg_8bits_t reg);
int OR_HL(cpu_t *cpu);
int OR_n8(cpu_t *cpu, uint8_t value);
int XOR_r8(cpu_t *cpu, reg_8bits_t reg);
int XOR_HL(cpu_t *cpu);
int XOR_n8(cpu_t *cpu, uint8_t value);


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
int CALL_cc_n16(cpu_t *cpu, bool condition);
int JP_HL(cpu_t *cpu);
int JP_n16(cpu_t *cpu);
int JP_cc_n16(cpu_t *cpu, bool condition);
int JR_n8(cpu_t *cpu, int8_t offset);
int JR_cc_n8(cpu_t *cpu, int8_t offset, bool condition);
int RET_cc(cpu_t *cpu, bool condition);
int RET(cpu_t *cpu);
int RETI(cpu_t *cpu);
int RST_vec(cpu_t *cpu, uint8_t vector);

// Carry flag instructions
int CCF(cpu_t *cpu);
int SCF(cpu_t *cpu);


// Stack manipulation instructions
int ADD_HL_Sp(cpu_t *cpu);

int ADD_SP_e8(cpu_t *cpu, int8_t offset)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = add(lsb(cpu->SP), Z);
    if (result.carry) {
        set_flag(cpu->registers, CARRY, 1);    
    } else
    {
        set_flag(cpu->registers, CARRY, 0);
    }
    if (result.halfcarry) {
        set_flag(cpu->registers, HALF_CARRY, 1);    
    } else 
    {
        set_flag(cpu->registers, HALF_CARRY, 0);
    }
    set_flag(cpu->registers, ZERO, 0);
    set_flag(cpu->registers, SUB, 0);
    uint8_t adj;
    if (bit(7, Z)) 
    {
        adj = 0xFF;
    } else 
    {
        adj = 0x00;
    }
    uint8_t W = msb(cpu->SP) + adj + get_flag(cpu->registers, CARRY);
    uint16_t WZ = unsigned_16(W, Z);
    cpu->SP = WZ;
    cpu->PC++;
    return 4;
}
int DEC_SP(cpu_t *cpu)
{
    //TODO
    return 1;
}
int INC_r8(cpu_t *cpu, reg_8bits_t reg)
{
    //TODO
    return 1;
}
int INC_r16(cpu_t *cpu, uint16_t *reg)
{
    //TODO
    return 1;
}

int LD_n16_SP(cpu_t *cpu)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    uint8_t W = read_memory(cpu, cpu->PC); cpu->PC++;
    uint16_t WZ = unsigned_16(W,Z);

    write_memory(cpu, WZ, lsb(cpu->SP)); WZ++;
    write_memory(cpu, WZ, msb(cpu->SP));
    cpu->PC++;
    return 5;
}
int LD_SP_HL(cpu_t *cpu)
{
    cpu->SP = *cpu->registers->HL;
    cpu->PC++;
    return 2;
}
int LD_HL_SP_e8(cpu_t *cpu, int8_t offset)
{
    uint8_t Z = read_memory(cpu, cpu->PC); cpu->PC++;
    operation_result_t result = add(lsb(cpu->SP), Z);
    set_8bit_register(cpu->registers, L, result.result);
    if (result.carry) {
        set_flag(cpu->registers, CARRY, 1);    
    } else
    {
        set_flag(cpu->registers, CARRY, 0);
    }
    if (result.halfcarry) {
        set_flag(cpu->registers, HALF_CARRY, 1);    
    } else 
    {
        set_flag(cpu->registers, HALF_CARRY, 0);
    }
    set_flag(cpu->registers, ZERO, 0);
    set_flag(cpu->registers, SUB, 0);
    uint8_t adj;
    if (bit(7, Z)) 
    {
        adj = 0xFF;
    } else 
    {
        adj = 0x00;
    }
    uint8_t r = msb(cpu->SP) + adj + get_flag(cpu->registers, CARRY);
    set_8bit_register(cpu->registers, H, r);
    cpu->PC++;
    return 3;
}
int POP_r16(cpu_t *cpu, uint16_t *reg)
{
    uint8_t Z = read_memory(cpu, cpu->SP); cpu->SP++;
    uint8_t W = read_memory(cpu, cpu->SP); cpu->SP++;
    *reg = unsigned_16(W,Z);
    cpu->PC++;
    return 3;
}
int PUSH_r16(cpu_t *cpu, uint16_t *reg)
{
    cpu->SP--;
    write_memory(cpu, cpu->SP, msb(*reg)); cpu->SP--;
    write_memory(cpu, cpu->SP, lsb(*reg));
    cpu->PC++;
    return 4;
}

// Interrupt-related instructions
int DI(cpu_t *cpu);
int EI(cpu_t *cpu);
int HALT(cpu_t *cpu);


// Miscellaneous instructions
int DAA(cpu_t *cpu);
int NOP(cpu_t *cpu);
int STOP(cpu_t *cpu);



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
    bool carry = result > UINT8_MAX; 
    bool halfcarry = (a & 0x0F +  b & 0x0F)>0x0F;
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

int bit(int i, uint8_t n)
{
    if (i > 7)
    {
        printf("Trying to access %dth bit of an uint8", i);
        exit(1);
    }
    return (int) ((n>>i) & 1);
}
