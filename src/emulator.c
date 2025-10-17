#include "emulator.h"
#include "cpu.h"

#include <stdlib.h>



emulator_t *new_emulator() 
{
    emulator_t *emulator = (emulator_t *)malloc(sizeof(emulator_t));
    if (!emulator) {
        return NULL;     
    }

    cpu_t *cpu = new_cpu();
    if (!cpu) {
        free(emulator);
        return NULL;
    }

    emulator->cpu = cpu;


    return emulator;
}

void free_emulator(emulator_t *emulator)
{
    if (emulator) {
        free_cpu(emulator->cpu);
        free(emulator);
        emulator = NULL;
    }
    return;
}



void tick_emulator(emulator_t *emulator) {
    if (emulator == NULL) {
        return;
    }

    const int MAX_FPS = 60;
    const int MAX_TICKS_PER_SECOND = 4194304;
    const int MAX_CYCLES = MAX_TICKS_PER_SECOND / MAX_FPS;

    int cycle_count = 0;

    while (cycle_count < MAX_CYCLES) {
        // Execute one instruction on the CPU
        int intruction_timing = execute_next_instruction(emulator->cpu);
        cycle_count += intruction_timing;
        //update_timer()
        //upudate_graphics()
        //do_interrupts() A creuser 
    }

}