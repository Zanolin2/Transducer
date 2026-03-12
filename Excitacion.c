#include "Excitacion.h"
#include "ADC.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdint.h>

#define BURST_CYCLES      5
#define DEAD_TIME_MS      12
#define COUNTS_PER_BURST  (BURST_CYCLES * 2)
#define COUNTS_PER_BRAKE  (DEAD_TIME_MS * 100)

typedef enum {
    STATE_PWM_ON,
    STATE_BRAKE_ON
} state_t;

static volatile uint16_t contador = 0;
static volatile state_t current_state = STATE_PWM_ON;

static void TimerInterruptHandler(void)
{
    contador++;

    switch (current_state)
    {
        case STATE_PWM_ON:
            if (contador >= COUNTS_PER_BURST)
            {
                PWM_SetLow();
                BRAKE_SetHigh();
                contador = 0;
                current_state = STATE_BRAKE_ON;
            }
            break;

        case STATE_BRAKE_ON:
            if (contador >= COUNTS_PER_BRAKE)
            {
                PWM_SetHigh();
                BRAKE_SetLow();
                contador = 0;
                current_state = STATE_PWM_ON;

                ADC_StartCapture();
            }
            break;
    }
}

void Excitation_Init(void)
{
    IPC1bits.T2IP   = 4;
    IPC1bits.DMA0IP = 3;
    IPC3bits.AD1IP  = 3;

    TMR2_SetInterruptHandler(TimerInterruptHandler);

    PWM_SetHigh();
    BRAKE_SetLow();
    current_state = STATE_PWM_ON;
    contador = 0;
}

void Excitation_Tasks(void) {}