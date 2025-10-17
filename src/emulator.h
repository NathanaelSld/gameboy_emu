#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include "cpu.h"  




typedef struct Emulator
{
    cpu_t *cpu;
    // Add other components of the emulator here, such as memory, input/output, etc.
    // For example:
    // memory_t memory;
    // input_t input;
    // output_t output;
} emulator_t;



emulator_t *new_emulator();
void free_emulator(emulator_t *emulator);


void tick_emulator(emulator_t *emulator);
void reset_emulator(emulator_t *emulator);

void load_rom(emulator_t *emulator, const char *rom_path);

#endif