#ifndef Excitacion_H
#define Excitacion_H

/**
 * @brief Inicializa el módulo de excitación.
 * Configura el timer, la interrupción y el estado inicial de los pines.
 */
void Excitation_Init(void);

/**
 * @brief Ejecuta la máquina de estados de excitación.
 * Esta función debe ser llamada repetidamente en el bucle principal (while(1)).
 */
void Excitation_Tasks(void);

#endif 