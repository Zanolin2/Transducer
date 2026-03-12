#include "ADC.h"
#include "mcc_generated_files/dma.h"
#include "mcc_generated_files/adc1.h"

#define FCY 40535000UL
#include <libpic30.h>

#define F_SAMPLING                 1039359.0f
#define VELOCIDAD_SONIDO           1500.0f
#define UMBRAL_ECO                 150
#define ZONA_MUERTA_TOTAL_SAMPLES  500

uint16_t bufferA[ADC_BLOCK_SIZE] __attribute__((aligned(1024)));
uint16_t bufferB[ADC_BLOCK_SIZE] __attribute__((aligned(1024)));

static volatile adc_state_t g_state = ADC_STATE_IDLE;
static volatile uint32_t g_total_samples_processed = 0;
static volatile float g_distancia_detectada = 0.0;
static volatile bool procesar_buffer_A = true;
static volatile uint16_t nivel_base = 100;
static volatile uint32_t base_suma_final = 0;
static volatile uint16_t base_count_final = 0;
static volatile uint16_t duracion_bloque = 0;
static volatile uint16_t primer_sample = 0;
static volatile uint16_t sample_maximo = 0;
static volatile uint32_t indice_maximo = 0;

void ADC_Init(void) {
    g_state = ADC_STATE_IDLE;
}

void ADC_ResetState(void) {
    if (g_state == ADC_STATE_FOUND) {
        g_state = ADC_STATE_IDLE;
    }
}

void ADC_StartCapture(void) {
    if (g_state != ADC_STATE_IDLE) return;

    g_state = ADC_STATE_SEARCHING;
    g_total_samples_processed = 0;
    g_distancia_detectada = 0.0;
    procesar_buffer_A = true;
    base_suma_final = 0;
    base_count_final = 0;
    sample_maximo = 0;
    indice_maximo = 0;

    DMA0STAL = (uint16_t)&bufferA;
    DMA0STAH = 0;
    DMA0STBL = (uint16_t)&bufferB;
    DMA0STBH = 0;
    DMA0CNT = ADC_BLOCK_SIZE - 1;

    IFS0bits.DMA0IF = 0;
    DMA0CONbits.CHEN = 1;
    AD1CON1bits.ADON = 1;
    AD1CON1bits.ASAM = 1;
}

bool ProcesarBloque(uint16_t* buffer, uint32_t offset_muestras) {
    uint16_t t_inicio = TMR2;

    for (int i = 0; i < ADC_BLOCK_SIZE; i++) {
        uint32_t indice_absoluto = offset_muestras + i;

        if (indice_absoluto > 8000) {
            duracion_bloque = TMR2 - t_inicio;
            return true;
        }

        // Medir base en zona de silencio (~5ms)
        if (indice_absoluto >= 5000 && indice_absoluto < 5100) {
            base_suma_final += buffer[i];
            base_count_final++;
            if (base_count_final == 100) {
                nivel_base = (uint16_t)(base_suma_final / 100);
            }
        }

        // Ignorar zona muerta
        if (indice_absoluto < ZONA_MUERTA_TOTAL_SAMPLES) continue;

        // Capturar primer sample valido post zona muerta
        if (indice_absoluto == ZONA_MUERTA_TOTAL_SAMPLES) {
            primer_sample = buffer[i];
        }

        // Rastrear sample maximo entre muestra 500 y 2000
        // El eco real a ~900us deberia ser el pico mas alto en esa zona
        if (indice_absoluto < 2000) {
            if (buffer[i] > sample_maximo) {
                sample_maximo = buffer[i];
                indice_maximo = indice_absoluto;
            }
        }

        // Buscar eco
        if (g_distancia_detectada == 0.0f) {
            if (buffer[i] > nivel_base + UMBRAL_ECO) {
                float tiempo = (float)indice_absoluto / F_SAMPLING;
                tiempo = tiempo - 0.000040f;
                if (tiempo < 0) tiempo = 0.0f;
                g_distancia_detectada = (tiempo * VELOCIDAD_SONIDO) / 2.0f;

                if (g_distancia_detectada == 0.0f)
                    g_distancia_detectada = 0.001f;
            }
        }
    }

    duracion_bloque = TMR2 - t_inicio;
    return false;
}

void DMA_Channel0_CallBack(void) {
    if (g_state != ADC_STATE_SEARCHING) return;

    bool encontrado = false;

    if (procesar_buffer_A) {
        encontrado = ProcesarBloque(bufferA, g_total_samples_processed);
    } else {
        encontrado = ProcesarBloque(bufferB, g_total_samples_processed);
    }

    procesar_buffer_A = !procesar_buffer_A;
    g_total_samples_processed += ADC_BLOCK_SIZE;

    if (encontrado || g_total_samples_processed > 13000) {
        AD1CON1bits.ASAM = 0;
        DMA0CONbits.CHEN = 0;
        AD1CON1bits.ADON = 0;
        g_state = ADC_STATE_FOUND;
    }
}

float ADC_GetDistancia(void) {
    return g_distancia_detectada;
}

adc_state_t ADC_GetState(void) {
    return g_state;
}

uint16_t ADC_GetNivelBase(void) {
    return nivel_base;
}

uint16_t ADC_GetDuracionBloque(void) {
    return duracion_bloque;
}

uint16_t ADC_GetPrimerSample(void) {
    return primer_sample;
}

uint16_t ADC_GetSampleMax(void) {
    return sample_maximo;
}

uint32_t ADC_GetIndiceMax(void) {
    return indice_maximo;
}

void ADC_Tasks(void) {}