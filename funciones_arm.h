// funciones_arm.h
#ifndef FUNCIONES_ARM_H
#define FUNCIONES_ARM_H

#include <stdint.h>

void disp_binary(uint8_t value);
void delay(volatile uint32_t ticks);
void leds(uint8_t value);

// Agregar estas funciones si estarán escritas en Assembly:
void secuencia_prop_alg(void);
void secuencia_tabla(void);

#endif
