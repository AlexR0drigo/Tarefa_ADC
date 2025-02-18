// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ---------- //
// Tarefa_ADC //
// ---------- //

#define Tarefa_ADC_wrap_target 0
#define Tarefa_ADC_wrap 3
#define Tarefa_ADC_pio_version 0

#define Tarefa_ADC_T1 3
#define Tarefa_ADC_T2 3
#define Tarefa_ADC_T3 4

static const uint16_t Tarefa_ADC_program_instructions[] = {
            //     .wrap_target
    0x6321, //  0: out    x, 1            side 0 [3] 
    0x1223, //  1: jmp    !x, 3           side 1 [2] 
    0x1200, //  2: jmp    0               side 1 [2] 
    0xa242, //  3: nop                    side 0 [2] 
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program Tarefa_ADC_program = {
    .instructions = Tarefa_ADC_program_instructions,
    .length = 4,
    .origin = -1,
    .pio_version = Tarefa_ADC_pio_version,
#if PICO_PIO_VERSION > 0
    .used_gpio_ranges = 0x0
#endif
};

static inline pio_sm_config Tarefa_ADC_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + Tarefa_ADC_wrap_target, offset + Tarefa_ADC_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

#include "hardware/clocks.h"
static inline void Tarefa_ADC_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = Tarefa_ADC_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    int cycles_per_bit = Tarefa_ADC_T1 + Tarefa_ADC_T2 + Tarefa_ADC_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

#endif

