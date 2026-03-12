#ifndef ADC_H
#define ADC_H
#include <xc.h>
#include <stdbool.h>

#define ADC_BLOCK_SIZE  1024  // 1024 muestras = 1ms por bloque, eco siempre dentro

typedef enum {
    ADC_STATE_IDLE,
    ADC_STATE_SEARCHING,
    ADC_STATE_FOUND
} adc_state_t;

void ADC_Init(void);
void ADC_StartCapture(void);
void ADC_ResetState(void);
void ADC_Tasks(void);
adc_state_t ADC_GetState(void);
float ADC_GetDistancia(void);
uint16_t ADC_GetNivelBase(void);
uint16_t ADC_GetDuracionBloque(void);
uint16_t ADC_GetPrimerSample(void);
uint16_t ADC_GetSampleMax(void);
uint32_t ADC_GetIndiceMax(void);
#endif