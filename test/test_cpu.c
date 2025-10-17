#include <assert.h>
#include <stdio.h>

#include "../src/cpu.h"


// ==================================================================================
//                                  Utils Functions
// ==================================================================================



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
//                                  Test Instructions
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
    assert(cpu->PC == old_PC + 1);
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
    assert(cpu->PC == old_PC + 2);
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
    assert(cpu->PC == old_PC + 3);
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
    assert(cpu->PC == old_PC + 1);
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
    assert(cpu->PC == old_PC + 2);
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
    assert(cpu->PC == old_PC + 1);
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
    assert(cpu->PC == old_PC + 1);
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
    assert(cpu->PC == old_PC + 3);
    assert(timing == 4);
}
void test_LDH_n16_A(cpu_t *cpu)
{
//TODO
}

void test_LDH_C_A(cpu_t *cpu)
{
    printf("Testing LDH_C_A...\n");
    uint16_t old_PC = cpu->PC;
    // LD A, 0x9A
    cpu->registers->AF = (uint16_t[]){0x9A00};
    // LD C, 0x10
    cpu->registers->BC = (uint16_t[]){0x0010};
    int timing = LDH_C_A(cpu);
    assert(cpu->memorybus[0xFF10] == 0x9A);
    assert(cpu->PC == old_PC + 1);
    assert(timing == 2);
}

void test_LD_A_r16(cpu_t *cpu);
void test_LD_A_n16(cpu_t *cpu);
void test_LDH_A_n16(cpu_t *cpu);
void test_LDH_A_C(cpu_t *cpu)
{
    printf("Testing LDH_A_C...\n");
    uint16_t old_PC = cpu->PC;
    // Prepare memory with value at address 0xFF20
    cpu->memorybus[0xFF20] = 0xCD; // Value to load into register
    // LD C, 0x20
    cpu->registers->BC = (uint16_t[]){0x0020};
    // LD A, (0xFF00 + C)
    cpu->registers->AF = (uint16_t[]){0x0000};
    int timing = LDH_A_C(cpu);
    assert(get_8bit_register(cpu->registers, A) == 0xCD);
    assert(cpu->PC == old_PC + 1);
    assert(timing == 2);
}
void test_LD_HLI_A(cpu_t *cpu);
void test_LD_HLD_A(cpu_t *cpu);
void test_LD_A_HLI(cpu_t *cpu);
void test_LD_A_HLD(cpu_t *cpu);

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



void main_test_cpu() {

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