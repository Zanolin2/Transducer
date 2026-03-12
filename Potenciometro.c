#include "Potenciometro.h"
#include "mcc_generated_files/spi2.h"
#include "mcc_generated_files/pin_manager.h"


/////////////////// --- CONSTANTES DE CONFIGURACIÓN --- ////////////////////////
#define POT_WRITE_COMMAND 0x11 // Comando para escribir en el pote

void Potentiometer_SetValue(uint8_t value)
{
    // 1. Activar el chip esclavo (bajar la línea CS)
    CS_POT_SetLow();

    // 2. Enviar el byte de comando (0x11)
    SPI2_Exchange8bit(POT_WRITE_COMMAND);

    // 3. Enviar el byte con el valor de la resistencia
    SPI2_Exchange8bit(value);

    // 4. Desactivar el chip esclavo (subir la línea CS)
    CS_POT_SetHigh();
}