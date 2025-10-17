#ifndef TEST_CPU_H
#define TEST_CPU_H

#include <assert.h>
#include <stdio.h>

#include "../src/cpu.h"


void test_get_8bit_register();
void test_set_8bit_register();

void test_get_flag();

void test_set_flag();

void main_test_cpu();

//Test instructions
void test_LD_r8_r8(cpu_t *cpu);
void test_LD_r8_n8(cpu_t *cpu);
void test_LD_r16_n16(cpu_t *cpu);
void test_LD_HL_r8(cpu_t *cpu);
void test_LD_HL_n8(cpu_t *cpu);
void test_LD_r8_HL(cpu_t *cpu);
void test_LD_r16_A(cpu_t *cpu);
void test_LD_n16_A(cpu_t *cpu);
void test_LDH_n16_A(cpu_t *cpu);
void test_LDH_C_A(cpu_t *cpu);
void test_LD_A_r16(cpu_t *cpu);
void test_LD_A_n16(cpu_t *cpu);
void test_LDH_A_n16(cpu_t *cpu);
void test_LDH_A_C(cpu_t *cpu);
void test_LD_HLI_A(cpu_t *cpu);
void test_LD_HLD_A(cpu_t *cpu);
void test_LD_A_HLI(cpu_t *cpu);
void test_LD_A_HLD(cpu_t *cpu);



#endif