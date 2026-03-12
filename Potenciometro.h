#ifndef Potenciometro_H
#define Potenciometro_H

#include <stdint.h> // Necesario para el tipo uint8_t

/**
 * @brief Configura la resistencia del potenciómetro digital.
 * @param value El valor de resistencia a establecer (0-255).
 */
void Potentiometer_SetValue(uint8_t value);

#endif 