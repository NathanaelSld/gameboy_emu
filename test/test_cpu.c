#include <assert.h>
#include <stdio.h>

#include "../src/cpu.h"

// ==================================================================================
//                                  Test Registers
// ==================================================================================

void test_get_8bit_register() {
        
    registers_t reg = {
        .AF = (uint16_t[]){0x1234},
        .BC = (uint16_t[]){0x5678},
        .DE = (uint16_t[]){0x9ABC},
        .HL = (uint16_t[]){0xDEF0},
        .SP = (uint16_t[]){0x1111},
        .PC = (uint16_t[]){0x2222}
    };    
    // Test getting each register
    assert(get_8bit_register(&reg, A) == 0x12);
    assert(get_8bit_register(&reg, B) == 0x56);
    assert(get_8bit_register(&reg, C) == 0x78);
    assert(get_8bit_register(&reg, D) == 0x9A);
    assert(get_8bit_register(&reg, E) == 0xBC);
    assert(get_8bit_register(&reg, H) == 0xDE);
    assert(get_8bit_register(&reg, L) == 0xF0);
}

void test_set_8bit_register() {
        
    registers_t reg = {
        .AF = (uint16_t[]){0x1234},
        .BC = (uint16_t[]){0x5678},
        .DE = (uint16_t[]){0x9ABC},
        .HL = (uint16_t[]){0xDEF0},
        .SP = (uint16_t[]){0x1111},
        .PC = (uint16_t[]){0x2222}
    };        
    // Test setting each register
    set_8bit_register(&reg, A, 0x01);
    assert(get_8bit_register(&reg, A) == 0x01);
    
    set_8bit_register(&reg, B, 0x02);
    assert(get_8bit_register(&reg, B) == 0x02);
    
    set_8bit_register(&reg, C, 0x03);
    assert(get_8bit_register(&reg, C) == 0x03);
    
    set_8bit_register(&reg, D, 0x04);
    assert(get_8bit_register(&reg, D) == 0x04);
    
    set_8bit_register(&reg, E, 0x05);
    assert(get_8bit_register(&reg, E) == 0x05);
    
    set_8bit_register(&reg, H, 0x07);
    assert(get_8bit_register(&reg, H) == 0x07);
    
    set_8bit_register(&reg, L, 0x08);
    assert(get_8bit_register(&reg, L) == 0x08);
}

// ==================================================================================
//                                  Test Flags
// ==================================================================================

void test_get_flag() {
        
    registers_t reg = {
        .AF = (uint16_t[]){0x1200},
        .BC = (uint16_t[]){0x5678},
        .DE = (uint16_t[]){0x9ABC},
        .HL = (uint16_t[]){0xDEF0},
        .SP = (uint16_t[]){0x1111},
        .PC = (uint16_t[]){0x2222}
    };        
    
    // Test getting flags
    assert(get_flag(&reg, ZERO) == 0); // Assuming initial value is not zero
    assert(get_flag(&reg, SUB) == 0);
    assert(get_flag(&reg, HALF_CARRY) == 0);
    assert(get_flag(&reg, CARRY) == 0);
}

void test_set_flag() {
        
    registers_t reg = {
        .AF = (uint16_t[]){0x1234},
        .BC = (uint16_t[]){0x5678},
        .DE = (uint16_t[]){0x9ABC},
        .HL = (uint16_t[]){0xDEF0},
        .SP = (uint16_t[]){0x1111},
        .PC = (uint16_t[]){0x2222}
    };        
    // Test setting flags
    set_flag(&reg, ZERO, 1);
    assert(get_flag(&reg, ZERO) == 1);
    
    set_flag(&reg, SUB, 1);
    assert(get_flag(&reg, SUB) == 1);
    
    set_flag(&reg, HALF_CARRY, 1);
    assert(get_flag(&reg, HALF_CARRY) == 1);
    
    set_flag(&reg, CARRY, 1);
    assert(get_flag(&reg, CARRY) == 1);
}


// ==================================================================================
//                                  Test LOAD Instructions
// ==================================================================================
void test_LD_r8_r8(cpu_t *cpu)
{
    printf("Testing LD_r8_r8...\n");
    uint16_t old_PC = cpu->PC;
    // LD B, A
    cpu->registers->AF = (uint16_t[]){0x12FF};
    cpu->registers->BC = (uint16_t[]){0x3400};
    int timing = LD_r8_r8(cpu, B, A);
    assert(get_8bit_register(cpu->registers, B) == 0x12);
    assert(cpu->PC == old_PC);
    assert(timing == 1);
}
void test_LD_r8_n8(cpu_t *cpu)
{
    printf("Testing LD_r8_n8...\n");
    uint16_t old_PC = cpu->PC;
    // Prepare memory with immediate value
    cpu->memorybus[old_PC] = 0x56; // Immediate value to load into register
    // LD C, n8
    cpu->registers->AF = (uint16_t[]){0x1200};
    cpu->registers->BC = (uint16_t[]){0x3400};
    int timing = LD_r8_n8(cpu, C);
    assert(get_8bit_register(cpu->registers, C) == 0x56);
    assert(cpu->PC == old_PC + 1);
    assert(timing == 2);
}
void test_LD_r16_n16(cpu_t *cpu)
{
    printf("Testing LD_r16_n16...\n");
    uint16_t old_PC = cpu->PC;
    // Prepare memory with immediate value
    cpu->memorybus[old_PC] = 0x78; // Low byte
    cpu->memorybus[old_PC + 1] = 0x56; // High byte
    // LD BC, n16
    cpu->registers->AF = (uint16_t[]){0x1200};
    cpu->registers->BC = (uint16_t[]){0x0000};
    int timing = LD_r16_n16(cpu, cpu->registers->BC);
    assert(*cpu->registers->BC == 0x5678);
    assert(cpu->PC == old_PC + 2);
    assert(timing == 3);
}
void test_LD_HL_r8(cpu_t *cpu)
{
    printf("Testing LD_HL_r8...\n");
    uint16_t old_PC = cpu->PC;
    // LD HL, 0x2000
    cpu->registers->HL = (uint16_t[]){0x2000};
    // LD A, 0x9A
    cpu->registers->AF = (uint16_t[]){0x9A00};
    int timing = LD_HL_r8(cpu, A);
    assert(cpu->memorybus[0x2000] == 0x9A);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}
void test_LD_HL_n8(cpu_t *cpu)
{
    printf("Testing LD_HL_n8...\n");
    uint16_t old_PC = cpu->PC;
    // Prepare memory with immediate value
    cpu->memorybus[old_PC] = 0xBC; // Immediate value to load into memory
    // LD HL, 0x2000
    cpu->registers->HL = (uint16_t[]){0x2000};
    int timing = LD_HL_n8(cpu);
    assert(cpu->memorybus[0x2000] == 0xBC);
    assert(cpu->PC == old_PC + 1);
    assert(timing == 3);
}
void test_LD_r8_HL(cpu_t *cpu)
{
    printf("Testing LD_r8_HL...\n");
    uint16_t old_PC = cpu->PC;
    // LD HL, 0x2000
    cpu->registers->HL = (uint16_t[]){0x2000};
    // Prepare memory with value at address HL
    cpu->memorybus[0x2000] = 0xDE; // Value to load into register
    // LD A, (HL)
    cpu->registers->AF = (uint16_t[]){0x0000};
    int timing = LD_r8_HL(cpu, A);
    assert(get_8bit_register(cpu->registers, A) == 0xDE);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}
void test_LD_r16_A(cpu_t *cpu)
{
    printf("Testing LD_r16_A...\n");
    uint16_t old_PC = cpu->PC;
    // LD A, 0x34
    cpu->registers->AF = (uint16_t[]){0x3400};
    // LD DE, 0x2000
    cpu->registers->DE = (uint16_t[]){0x2000};
    int timing = LD_r16_A(cpu, cpu->registers->DE);
    assert(cpu->memorybus[0x2000] == 0x34);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}
void test_LD_n16_A(cpu_t *cpu)
{
    printf("Testing LD_n16_A...\n");
    uint16_t old_PC = cpu->PC;
    // Prepare memory with immediate address
    cpu->memorybus[old_PC] = 0x00; // Low byte
    cpu->memorybus[old_PC + 1] = 0x20; // High byte
    // LD A, 0x78
    cpu->registers->AF = (uint16_t[]){0x7800};
    int timing = LD_n16_A(cpu);
    assert(cpu->memorybus[0x2000] == 0x78);
    assert(cpu->PC == old_PC + 2);
    assert(timing == 4);
}
void test_LDH_n16_A(cpu_t *cpu)
{
    printf("Testing LDH_n16_A...\n");
    uint16_t old_PC = cpu->PC;
    // Prepare memory with immediate address
    cpu->memorybus[old_PC] = 0xDE; // Low byte
    cpu->registers->AF = (uint16_t[]){0x7800};
    int timing = LDH_n16_A(cpu);
    assert(cpu->memorybus[0xFFDE] == 0x78);
    assert(cpu->PC == old_PC + 1);
    assert(timing == 2);
}

void test_LDH_C_A(cpu_t *cpu)
{
    printf("Testing LDH_C_A...\n");
    uint16_t old_PC = cpu->PC;
    cpu->registers->AF = (uint16_t[]){0x9A00};
    cpu->registers->BC = (uint16_t[]){0x0010};
    int timing = LDH_C_A(cpu);
    assert(cpu->memorybus[0xFF10] == 0x9A);
    assert(cpu->PC == old_PC + 1);
    assert(timing == 2);
}

void test_LD_A_r16(cpu_t *cpu)
{
    printf("Testing LD_A_r16...\n");
    uint16_t old_PC = cpu->PC;
    cpu->registers->AF = (uint16_t[]){0x0000};
    cpu->registers->BC = (uint16_t[]){0x1234};
    cpu->memorybus[0x1234] = 0xAB;

    int timing = LD_A_r16(cpu, cpu->registers->BC);
    assert(get_8bit_register(cpu, A) == 0xAB);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}   

void test_LD_A_n16(cpu_t *cpu)
{
    printf("Testing LD_N_r16...\n");
    uint16_t old_PC = cpu->PC;
    cpu->registers->AF = (uint16_t[]){0x0000};
    cpu->memorybus[cpu->PC] = 0xCD;
    cpu->memorybus[cpu->PC+1] = 0xAB;
    cpu->memorybus[0xABCD] = 0xEF;

    int timing = LD_A_n16(cpu);
    assert(get_8bit_register(cpu, A) == 0xEF);
    assert(cpu->PC == old_PC + 2);
    assert(timing == 4);
}

void test_LDH_A_C(cpu_t *cpu)
{
    printf("Testing LDH_A_C...\n");
    uint16_t old_PC = cpu->PC;
    cpu->memorybus[0xFF20] = 0xCD; 
    cpu->registers->BC = (uint16_t[]){0x0020};
    
    cpu->registers->AF = (uint16_t[]){0x0000};
    int timing = LDH_A_C(cpu);
    assert(get_8bit_register(cpu->registers, A) == 0xCD);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}
void test_LD_HLI_A(cpu_t *cpu)
{
    uint16_t old_PC = cpu->PC;
    cpu->registers->HL = (uint16_t[]){0x3000};
    set_8bit_register(cpu->registers, A, 0xEF);
    int timing = LD_HLD_A(cpu);
    assert(cpu->memorybus[0x3000] == 0xEF);
    assert(*cpu->registers->HL == 0x3100);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}

void test_LD_HLD_A(cpu_t *cpu)
{
    uint16_t old_PC = cpu->PC;
    cpu->registers->HL = (uint16_t[]){0x3000};
    set_8bit_register(cpu->registers, A, 0xEF);
    int timing = LD_HLD_A(cpu);
    assert(cpu->memorybus[0x3000] == 0xEF);
    assert(*cpu->registers->HL == 0x2FFF);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}
void test_LD_A_HLI(cpu_t *cpu)
{
    uint16_t old_PC = cpu->PC;
    cpu->registers->HL = (uint16_t[]){0x3000};
    set_8bit_register(cpu->registers, A, 0xEF);
    cpu->memorybus[0x3000] = 0xAB;
    int timing = LD_A_HLI(cpu);
    assert(get_8bit_register(cpu->registers, A) == 0xAB);
    assert(*cpu->registers->HL == 0x3001);
    assert(cpu->PC == old_PC);
    assert(timing == 2);

}
void test_LD_A_HLD(cpu_t *cpu)
{
    uint16_t old_PC = cpu->PC;
    cpu->registers->HL = (uint16_t[]){0x3000};
    cpu->memorybus[0x3000] = 0xCD;
    int timing = LD_A_HLD(cpu);
    assert(get_8bit_register(cpu->registers, A) == 0xCD);
    assert(*cpu->registers->HL == 0x2FFF);
    assert(cpu->PC == old_PC);
    assert(timing == 2);
}



void test_load_instructions()
{
    cpu_t cpu;
    uint8_t memory[0xFFFF] = {0};
    uint16_t AF = 0x0000, BC = 0x0000, DE = 0x0000, HL = 0x0000, SP = 0xFFFE, PC = 0x0000;
    registers_t registers = {
        .AF = &AF,
        .BC = &BC,
        .DE = &DE,
        .HL = &HL,
        .SP = &SP,
        .PC = &PC
    };
    cpu.registers = &registers;
    cpu.memorybus = memory;
    cpu.PC = PC;
    cpu.SP = SP;

    test_LD_r8_r8(&cpu);
    test_LD_r8_n8(&cpu);
    test_LD_r16_n16(&cpu);
    test_LD_HL_r8(&cpu);
    test_LD_HL_n8(&cpu);
    test_LD_r8_HL(&cpu);
    test_LD_r16_A(&cpu);
    test_LD_n16_A(&cpu);
    test_LDH_n16_A(&cpu);
    test_LDH_C_A(&cpu);
    test_LD_A_r16(&cpu);
    test_LD_A_n16(&cpu);
    test_LDH_A_n16(&cpu);
    test_LDH_A_C(&cpu);
    test_LD_HLI_A(&cpu);
    test_LD_HLD_A(&cpu);
    test_LD_A_HLI(&cpu);
    test_LD_A_HLD(&cpu);
}

// ==================================================================================
//                          Test 8-bit Arithmetic  Instructions
// ==================================================================================

void test_ADC_r8(cpu_t *cpu)
{
    printf("Testing ADC_r8...\n");

    printf("Basic behaviour without carry\n");
    cpu->registers->BC = (uint16_t[]){0x0200};
    cpu->registers->AF = (uint16_t[]){0x0200};
    uint16_t old_pc = cpu->PC;
    int timing = ADC_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x04);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    printf("With carry and half-carry\n");
    cpu->registers->BC = (uint16_t[]){0xFF00};
    cpu->registers->AF = (uint16_t[]){0xFF00 | 0x10}; // Set carry flag
    uint16_t old_pc2 = cpu->PC;
    int timing2 = ADC_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing2 == 1);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 1);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_ADC_HL(cpu_t *cpu)
{
    printf("Testing ADC_HL...\n");
    cpu->registers->HL = (uint16_t[]){0x0002};
    cpu->memorybus[0x0002] = 0x02;
    cpu->registers->AF = (uint16_t[]){0x0200};
    uint16_t old_pc = cpu->PC;
    int timing = ADC_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x04);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_ADC_n8(cpu_t *cpu);

void test_ADD_r8(cpu_t *cpu)
{
    printf("Testing ADD_r8...\n");

    printf("Basic behaviour\n");
    cpu->registers->BC = (uint16_t[]){0x0200};
    cpu->registers->AF = (uint16_t[]){0x0200};
    uint16_t old_pc = cpu->PC;
    int timing = ADD_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x04);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    printf("With carry and half-carry\n");
    cpu->registers->BC = (uint16_t[]){0xFF00};
    cpu->registers->AF = (uint16_t[]){0xFF00};
    old_pc = cpu->PC;
    timing = ADD_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0xFE);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 1);
    assert(get_flag(cpu->registers, SUB) == 0);

    printf("Zero result\n");
    cpu->registers->BC = (uint16_t[]){0x8000};
    cpu->registers->AF = (uint16_t[]){0x8000};
    old_pc = cpu->PC;
    timing = ADD_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 1);
    assert(get_flag(cpu->registers, SUB) == 0);
}

void test_ADD_HL(cpu_t *cpu);
void test_ADD_n8(cpu_t *cpu);
void test_CP_r8(cpu_t *cpu)
{
    printf("Testing CP_r8...\n");
    printf("Basic behaviour\n");
    cpu->registers->BC = (uint16_t[]){0x0300};
    cpu->registers->AF = (uint16_t[]){0x0500};
    uint16_t old_pc = cpu->PC;
    int timing = CP_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x05);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);

    printf("Zero result\n");
    cpu->registers->BC = (uint16_t[]){0x0500};
    cpu->registers->AF = (uint16_t[]){0x0500};
    old_pc = cpu->PC;
    timing = CP_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x05);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);
}
void test_CP_HL(cpu_t *cpu)
{
    printf("Testing CP_HL...\n");
    cpu->registers->HL = (uint16_t[]){0x0300};
    cpu->memorybus[0x0300] = 0x02;
    cpu->registers->AF = (uint16_t[]){0x0500};
    uint16_t old_pc = cpu->PC;
    int timing = CP_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x05);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);
}
void test_CP_A_n8(cpu_t *cpu);
void test_DEC_r8(cpu_t *cpu)
{
    printf("Testing DEC_r8...\n");

    printf("Basic behaviour\n");
    cpu->registers->BC = (uint16_t[]){0xFF00};
    uint16_t old_pc = cpu->PC;
    int timing = DEC_r8(cpu, B);
    assert(*cpu->registers->BC == 0xFE00);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);

    printf("Zero result\n");
    cpu->registers->BC = (uint16_t[]){0x0100};
    old_pc = cpu->PC;
    timing = DEC_r8(cpu, B);
    assert(*cpu->registers->BC == 0x0000);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
}
void test_DEC_HL(cpu_t *cpu);

void test_INC_r8(cpu_t *cpu)
{
    printf("Testing INC_r8...\n");

    printf("Basic behaviour\n");
    cpu->registers->BC = (uint16_t[]){0x0000};
    uint16_t old_pc = cpu->PC;
    int timing = INC_r8(cpu, B);
    assert(*cpu->registers->BC == 0x0100);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    printf("Zero result\n");
    cpu->registers->BC = (uint16_t[]){0xFF00};
    old_pc = cpu->PC;
    timing = INC_r8(cpu, B);
    assert(*cpu->registers->BC == 0x0000);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
}

void test_INC_HL(cpu_t *cpu)
{
    printf("Testing INC_HL...\n");

    printf("Basic behaviour\n");
    cpu->registers->HL = (uint16_t[]){0x0000};
    uint16_t old_pc = cpu->PC;
    int timing = INC_HL(cpu);
    assert(*cpu->registers->HL == 0x0001);
    assert(timing == 3);
    assert(cpu->PC == (old_pc++));

    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}

void test_SBC_r8(cpu_t *cpu)
{
    printf("Testing SBC_r8...\n");

    printf("Basic behaviour without borrow\n");
    cpu->registers->BC = (uint16_t[]){0x0500};
    cpu->registers->AF = (uint16_t[]){0x0A00};
    uint16_t old_pc = cpu->PC;
    int timing = SBC_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x05);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);

    printf("With borrow and half-borrow\n");
    cpu->registers->BC = (uint16_t[]){0x0F00};
    cpu->registers->AF = (uint16_t[]){0x0A00 | 0x10}; // Set carry flag
    uint16_t old_pc2 = cpu->PC;
    int timing2 = SBC_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0xFB);
    assert(timing2 == 1);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 1);
    assert(get_flag(cpu->registers, SUB) == 1);
}
void test_SBC_HL(cpu_t *cpu)
{
    printf("Testing SBC_HL...\n");
    cpu->registers->HL = (uint16_t[]){0x0300};
    cpu->memorybus[0x0300] = 0x02;
    cpu->registers->AF = (uint16_t[]){0x0500};
    uint16_t old_pc = cpu->PC;
    int timing = SBC_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x03);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);
}
void test_SBC_A_n8(cpu_t *cpu);
void test_SUB_r8(cpu_t *cpu)
{
    printf("Testing SUB_r8...\n");

    printf("Basic behaviour\n");
    cpu->registers->BC = (uint16_t[]){0x0500};
    cpu->registers->AF = (uint16_t[]){0x0A00};
    uint16_t old_pc = cpu->PC;
    int timing = SUB_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x05);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);

    printf("With borrow and half-borrow\n");
    cpu->registers->BC = (uint16_t[]){0x0F00};
    cpu->registers->AF = (uint16_t[]){0x0A00};
    uint16_t old_pc2 = cpu->PC;
    int timing2 = SUB_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0xFB);
    assert(timing2 == 1);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 1);
    assert(get_flag(cpu->registers, SUB) == 1);
}
void test_SUB_HL(cpu_t *cpu)
{
    printf("Testing SUB_HL...\n");
    cpu->registers->HL = (uint16_t[]){0x0300};
    cpu->memorybus[0x0300] = 0x02;
    cpu->registers->AF = (uint16_t[]){0x0500};
    uint16_t old_pc = cpu->PC;
    int timing = SUB_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x03);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 1);
}
void test_SUB_A_n8(cpu_t *cpu);

void test_arithmetic_instructions()
{
    cpu_t cpu;
    uint8_t memory[0xFFFF] = {0};
    uint16_t AF = 0x0000, BC = 0x0000, DE = 0x0000, HL = 0x0000, SP = 0xFFFE, PC = 0x0000;
    registers_t registers = {
        .AF = &AF,
        .BC = &BC,
        .DE = &DE,
        .HL = &HL,
        .SP = &SP,
        .PC = &PC
    };
    cpu.registers = &registers;
    cpu.memorybus = memory;
    cpu.PC = PC;
    cpu.SP = SP;

    test_ADD_r8(&cpu);
    test_ADD_HL(&cpu);
    test_ADD_n8(&cpu);
    test_CP_A_HL(&cpu);
    test_CP_A_n8(&cpu);
    test_DEC_r8(&cpu);
    test_DEC_HL(&cpu);
    test_INC_r8(&cpu);
    test_INC_HL(&cpu);
    test_SBC_r8(&cpu);
    test_SBC_HL(&cpu);
    test_SBC_A_n8(&cpu);
    test_SUB_r8(&cpu);
    test_SUB_A_HL(&cpu);
    test_SUB_A_n8(&cpu);
}


// ==================================================================================
//                         Test 16-bit Arithmetic  Instructions
// ==================================================================================

void test_ADD_HL_r16(cpu_t *cpu);
void test_DEC_r16(cpu_t *cpu);
void test_INC_r16(cpu_t *cpu)
{
    printf("Testing INC_r16...\n");
    cpu->registers->BC = (uint16_t[]){0x0000};
    uint16_t old_pc = cpu->PC;
    int timing = INC_r16(cpu, cpu->registers->BC);
    assert(*cpu->registers->BC == 0x0000);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
}

void test_16bit_arithmetic_instructions()
{
    cpu_t cpu;
    uint8_t memory[0xFFFF] = {0};
    uint16_t AF = 0x0000, BC = 0x0000, DE = 0x0000, HL = 0x0000, SP = 0xFFFE, PC = 0x0000;
    registers_t registers = {
        .AF = &AF,
        .BC = &BC,
        .DE = &DE,
        .HL = &HL,
        .SP = &SP,
        .PC = &PC
    };
    cpu.registers = &registers;
    cpu.memorybus = memory;
    cpu.PC = PC;
    cpu.SP = SP;

    test_ADD_HL_r16(&cpu);
    test_DEC_r16(&cpu);
    test_INC_r16(&cpu);
}

// ==================================================================================
//                         Test Bitwise Logic Instructions
// ==================================================================================

void test_AND_r8(cpu_t *cpu)
{
    printf("Testing AND_r8...\n");

    cpu->registers->BC = (uint16_t[]){0x0F00};
    cpu->registers->AF = (uint16_t[]){0xF0FF};
    uint16_t old_pc = cpu->PC;
    int timing = AND_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    cpu->registers->BC = (uint16_t[]){0x3C00};
    cpu->registers->AF = (uint16_t[]){0xF0FF};
    uint16_t old_pc2 = cpu->PC;
    int timing2 = AND_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x30);
    assert(timing2 == 1);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_AND_HL(cpu_t *cpu)
{
    printf("Testing AND_HL...\n");

    cpu->registers->HL = (uint16_t[]){0x2000};
    cpu->memorybus[0x2000] = 0x0F;
    cpu->registers->AF = (uint16_t[]){0xF0FF};
    uint16_t old_pc = cpu->PC;
    int timing = AND_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    cpu->registers->HL = (uint16_t[]){0x2000};
    cpu->memorybus[0x2000] = 0x3C;
    cpu->registers->AF = (uint16_t[]){0xF0FF};
    uint16_t old_pc2 = cpu->PC;
    int timing2 = AND_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x30);
    assert(timing2 == 2);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 1);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_AND_n8(cpu_t *cpu);
void test_CPL(cpu_t *cpu);
void test_OR_r8(cpu_t *cpu)
{
    printf("Testing OR_r8...\n");

    cpu->registers->BC = (uint16_t[]){0x0F00};
    cpu->registers->AF = (uint16_t[]){0xF0F0};
    uint16_t old_pc = cpu->PC;
    int timing = OR_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0xFF);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    cpu->registers->BC = (uint16_t[]){0x0000};
    cpu->registers->AF = (uint16_t[]){0x0000};
    uint16_t old_pc2 = cpu->PC;
    int timing2 = OR_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing2 == 1);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_OR_HL(cpu_t *cpu)
{
    printf("Testing OR_HL...\n");

    cpu->registers->HL = (uint16_t[]){0x4000};
    cpu->memorybus[0x4000] = 0x0F;
    cpu->registers->AF = (uint16_t[]){0xF0F0};
    uint16_t old_pc = cpu->PC;
    int timing = OR_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0xFF);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    cpu->registers->HL = (uint16_t[]){0x4000};
    cpu->memorybus[0x4000] = 0x00;
    cpu->registers->AF = (uint16_t[]){0x0000};
    uint16_t old_pc2 = cpu->PC;
    int timing2 = OR_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing2 == 2);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_OR_n8(cpu_t *cpu);
void test_XOR_r8(cpu_t *cpu)
{
    printf("Testing XOR_r8...\n");

    cpu->registers->BC = (uint16_t[]){0xFF00};
    cpu->registers->AF = (uint16_t[]){0x0F0F};
    uint16_t old_pc = cpu->PC;
    int timing = XOR_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0xF0);
    assert(timing == 1);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    cpu->registers->BC = (uint16_t[]){0xF000};
    cpu->registers->AF = (uint16_t[]){0xF000};
    uint16_t old_pc2 = cpu->PC;
    int timing2 = XOR_r8(cpu, B);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing2 == 1);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_XOR_HL(cpu_t *cpu)
{
    printf("Testing XOR_HL...\n");

    cpu->registers->HL = (uint16_t[]){0x3000};
    cpu->memorybus[0x3000] = 0xFF;
    cpu->registers->AF = (uint16_t[]){0x0F0F};
    uint16_t old_pc = cpu->PC;
    int timing = XOR_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0xF0);
    assert(timing == 2);
    assert(cpu->PC == (old_pc++));
    assert(get_flag(cpu->registers, ZERO) == 0);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);

    cpu->registers->HL = (uint16_t[]){0x3000};
    cpu->memorybus[0x3000] = 0xF0;
    cpu->registers->AF = (uint16_t[]){0xF000};
    uint16_t old_pc2 = cpu->PC;
    int timing2 = XOR_HL(cpu);
    assert(get_8bit_register(cpu, A) == 0x00);
    assert(timing2 == 2);
    assert(cpu->PC == (old_pc2++));
    assert(get_flag(cpu->registers, ZERO) == 1);
    assert(get_flag(cpu->registers, HALF_CARRY) == 0);
    assert(get_flag(cpu->registers, CARRY) == 0);
    assert(get_flag(cpu->registers, SUB) == 0);
}
void test_XOR_n8(cpu_t *cpu);

void test_bitwise_logic_instructions()
{
    cpu_t cpu;
    uint8_t memory[0xFFFF] = {0};
    uint16_t AF = 0x0000, BC = 0x0000, DE = 0x0000, HL = 0x0000, SP = 0xFFFE, PC = 0x0000;
    registers_t registers = {
        .AF = &AF,
        .BC = &BC,
        .DE = &DE,
        .HL = &HL,
        .SP = &SP,
        .PC = &PC
    };
    cpu.registers = &registers;
    cpu.memorybus = memory;
    cpu.PC = PC;
    cpu.SP = SP;

    test_AND_r8(&cpu);
    test_AND_HL(&cpu);
    test_AND_n8(&cpu);
    test_CPL(&cpu);
    test_OR_r8(&cpu);
    test_OR_HL(&cpu);
    test_OR_n8(&cpu);
    test_XOR_r8(&cpu);
    test_XOR_HL(&cpu);
    test_XOR_n8(&cpu);
}

// ==================================================================================
//                          Test Bit Flag Instructions
// ==================================================================================

void test_BIT_r8(cpu_t *cpu);
void test_BIT_HL(cpu_t *cpu);
void test_RES_r8(cpu_t *cpu);
void test_RES_HL(cpu_t *cpu);
void test_SET_r8(cpu_t *cpu);
void test_SET_HL(cpu_t *cpu);

void test_bit_flag_instructions()
{
    cpu_t cpu;
    uint8_t memory[0xFFFF] = {0};
    uint16_t AF = 0x0000, BC = 0x0000, DE = 0x0000, HL = 0x0000, SP = 0xFFFE, PC = 0x0000;
    registers_t registers = {
        .AF = &AF,
        .BC = &BC,
        .DE = &DE,
        .HL = &HL,
        .SP = &SP,
        .PC = &PC
    };
    cpu.registers = &registers;
    cpu.memorybus = memory;
    cpu.PC = PC;
    cpu.SP = SP;

    test_BIT_r8(&cpu);
    test_BIT_HL(&cpu);
    test_RES_r8(&cpu);
    test_RES_HL(&cpu);
    test_SET_r8(&cpu);
    test_SET_HL(&cpu);
}

// ==================================================================================
//                                  Test Bit Shift Instructions
// ==================================================================================

void test_RL_r8(cpu_t *cpu);
void test_RL_HL(cpu_t *cpu);
void test_RLA(cpu_t *cpu);
void test_RLC_r8(cpu_t *cpu);
void test_RLC_HL(cpu_t *cpu);
void test_RLCA(cpu_t *cpu);
void test_RR_r8(cpu_t *cpu);
void test_RR_HL(cpu_t *cpu);
void test_RRA(cpu_t *cpu);
void test_RRC_r8(cpu_t *cpu);
void test_RRC_HL(cpu_t *cpu);
void test_RRCA(cpu_t *cpu);
void test_SLA_r8(cpu_t *cpu);
void test_SLA_HL(cpu_t *cpu);
void test_SRA_r8(cpu_t *cpu);
void test_SRA_HL(cpu_t *cpu);
void test_SRL_r8(cpu_t *cpu);
void test_SRL_HL(cpu_t *cpu);
void test_SWAP_r8(cpu_t *cpu);
void test_SWAP_HL(cpu_t *cpu);

void test_bit_shift_instructions()
{
    cpu_t cpu;
    uint8_t memory[0xFFFF] = {0};
    uint16_t AF = 0x0000, BC = 0x0000, DE = 0x0000, HL = 0x0000, SP = 0xFFFE, PC = 0x0000;
    registers_t registers = {
        .AF = &AF,
        .BC = &BC,
        .DE = &DE,
        .HL = &HL,
        .SP = &SP,
        .PC = &PC
    };
    cpu.registers = &registers;
    cpu.memorybus = memory;
    cpu.PC = PC;
    cpu.SP = SP;

    test_RL_r8(&cpu);
    test_RL_HL(&cpu);
    test_RLA(&cpu);
    test_RLC_r8(&cpu);
    test_RLC_HL(&cpu);
    test_RLCA(&cpu);
    test_RR_r8(&cpu);
    test_RR_HL(&cpu);
    test_RRA(&cpu);
    test_RRC_r8(&cpu);
    test_RRC_HL(&cpu);
    test_RRCA(&cpu);
    test_SLA_r8(&cpu);
    test_SLA_HL(&cpu);
    test_SRA_r8(&cpu);
    test_SRA_HL(&cpu);
    test_SRL_r8(&cpu);
    test_SRL_HL(&cpu);
    test_SWAP_r8(&cpu);
    test_SWAP_HL(&cpu);
}

// ==================================================================================
//                         Test Jumps and Subroutine Instructions
// ==================================================================================

void test_CALL_n16(cpu_t *cpu);
void test_CALL_cc_n16(cpu_t *cpu);
void test_JP_HL(cpu_t *cpu);
void test_JP_n16(cpu_t *cpu);
void test_JP_cc_n16(cpu_t *cpu);
void test_JR_n8(cpu_t *cpu);
void test_JR_cc_n8(cpu_t *cpu);
void test_RET_cc(cpu_t *cpu);
void test_RET(cpu_t *cpu);
void test_RETI(cpu_t *cpu);
void test_RST_vec(cpu_t *cpu);


// ==================================================================================
//                                 Test Stack manipulation Instructions
// ==================================================================================
void test_ADD_HL_Sp(cpu_t *cpu);
void test_ADD_SP_e8(cpu_t *cpu);
void test_DEC_SP(cpu_t *cpu);
void test_INC_SP(cpu_t *cpu);
void test_LD_SP_n16(cpu_t *cpu);
void test_LD_n16_SP(cpu_t *cpu)
{
    printf("Testing LD_n16_SP..\n");
    cpu->PC = 0x0000;
    cpu->memorybus[0000] = 0x34;
    cpu->memorybus[cpu->PC+1] = 0x12;
    cpu->SP = 0xABCD;
    cpu->memorybus[cpu->SP] = 0xEF;
    int timing = LD_n16_SP(cpu);
    assert(cpu->memorybus[0x1234] == 0xEF);
    assert(timing == 5);
}
void test_LD_SP_HL(cpu_t *cpu);
void test_LD_HL_SP_e8(cpu_t *cpu);
void test_POP_r16(cpu_t *cpu);
void test_PUSH_r16(cpu_t *cpu);
void test_PUSH_AF(cpu_t *cpu);

void test_stack_manipulation_instructions()
{
    cpu_t cpu;
    uint8_t memory[0xFFFF] = {0};
    uint16_t AF = 0x0000, BC = 0x0000, DE = 0x0000, HL = 0x0000, SP = 0xFFFE, PC = 0x0000;
    registers_t registers = {
        .AF = &AF,
        .BC = &BC,
        .DE = &DE,
        .HL = &HL,
        .SP = &SP,
        .PC = &PC
    };
    cpu.registers = &registers;
    cpu.memorybus = memory;
    cpu.PC = PC;
    cpu.SP = SP;

    test_ADD_HL_Sp(&cpu);
    test_ADD_SP_e8(&cpu);
    test_DEC_SP(&cpu);
    test_INC_SP(&cpu);
    test_LD_SP_n16(&cpu);
    test_LD_n16_SP(&cpu);
    test_LD_SP_HL(&cpu);
    test_LD_HL_SP_e8(&cpu);
    test_POP_r16(&cpu);
    test_PUSH_r16(&cpu);
    test_PUSH_AF(&cpu);
}

// ===================================================================================
//                          Test Interrupt-Related Instructions
// ===================================================================================

void test_DI(cpu_t *cpu);
void test_EI(cpu_t *cpu);
void test_HALT(cpu_t *cpu);

// ==================================================================================
//                         Test Miscellanous Instructions
// ==================================================================================

void test_DAA(cpu_t *cpu);
void test_NOP(cpu_t *cpu);
void test_STOP(cpu_t *cpu);

// ==================================================================================
//                                  Utility Test Function
// ==================================================================================

void test_unsigned_16()
{
    printf("Testing unsigned_16...\n");
    uint16_t result = unsigned_16(0x34, 0x12);
    assert(result == 0x1234);
}

void test_lsb()
{
    printf("Testing lsb...\n");
    uint8_t result = lsb(0x1234);
    assert(result == 0x34);
}

void test_msb()
{
    printf("Testing msb...\n");
    uint8_t result = msb(0x1234);
    assert(result == 0x12);
}

void test_add()
{
    printf("Testing add...\n");
    operation_result_t result = add(0x14, 0x28);
    assert(result.result == 0x3C);
    assert(result.carry == 0);
    assert(result.halfcarry == 0);

    result = add(0xFF, 0x02);
    assert(result.result == 0x01);
    assert(result.carry == 1);
    assert(result.halfcarry == 1);

    result = add(0x0F, 0x01);
    assert(result.result == 0x10);
    assert(result.carry == 0);
    assert(result.halfcarry == 1);
}

void test_sub()
{
    printf("Testing sub...\n");
    operation_result_t result = sub(0x28, 0x14);
    assert(result.result == 0x14);
    assert(result.carry == 0);
    assert(result.halfcarry == 0);

    result = sub(0x00, 0x01);
    assert(result.result == 0xFF);
    assert(result.carry == 1);
    assert(result.halfcarry == 1);

    result = sub(0x10, 0x01);
    assert(result.result == 0x0F);
    assert(result.carry == 0);
    assert(result.halfcarry == 1);
}

int test_bit()
{
    printf("Testing bit...\n");
    assert(bit(0, 0b00000001) == 1);
    assert(bit(1, 0b00000010) == 1);
    assert(bit(2, 0b00000100) == 1);
    assert(bit(3, 0b00001000) == 1);
    assert(bit(4, 0b00010000) == 1);
    assert(bit(5, 0b00100000) == 1);
    assert(bit(6, 0b01000000) == 1);
    assert(bit(7, 0b10000000) == 1);
    assert(bit(0, 0b00000000) == 0);
    assert(bit(7, 0b01111111) == 0);
    return 0;
}

void test_utility_functions()
{
    test_unsigned_16();
    test_lsb();
    test_msb();
    test_add();
    test_sub();
    test_bit();
}


// ==================================================================================
//                                  Main Test Function
// ==================================================================================


void main_test_cpu() {
    printf("Running Utility functions tests...\n");
    test_utility_functions();

    printf("Running Registers tests...\n");

    printf("get_8bit_register tests\n");
    test_get_8bit_register();
    printf("get_8bit_register tests passed!\n");
    
    printf("set_8bit_register tests\n");
    test_set_8bit_register();
    printf("set_8bit_register tests passed!\n");

    printf("Running Flags tests...\n");
    test_get_flag();
    printf("get_flag tests passed!\n");
    test_set_flag();
    printf("set_flag tests passed!\n");

    printf("Running instructions test");
    test_load_instructions();
    printf("Instructions tests passed!\n");
    
    
}