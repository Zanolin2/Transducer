#include "mcc_generated_files/system.h"
#include "Potenciometro.h"
#include "Excitacion.h"
#include "ADC.h"
#include "LCD_I2C.h"
#include <stdio.h>
#include <string.h>
#define FCY 40535000UL
#include <libpic30.h>

int main(void)
{
    SYSTEM_Initialize();
    Excitation_Init();
    LCD_Init();
    ADC_Init();

    LCD_PrintWrapped("Transducer UNMDP", "Iniciando...");
    __delay_ms(2000);

    char debug[17];
    char debug_anterior[17] = "";
    uint16_t ciclos = 0;

    while (1)
    {
         // En main.c, agregá esta función de debug
if (ADC_GetState() == ADC_STATE_FOUND)
{
    ciclos++;
    if (ciclos >= 5)
    {
        char debug[17];
        sprintf(debug, "Mx:%d I:%lu", ADC_GetSampleMax(), ADC_GetIndiceMax());
        LCD_PrintWrapped("Debug:", debug);
        ciclos = 0;
    }
    ADC_ResetState();
}
    }
    return 0;
}