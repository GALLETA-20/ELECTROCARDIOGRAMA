#ifndef FUENTES_H
#define FUENTES_H

#include <stdint.h>


int dibujar_letras(uint16_t x_pos, uint16_t y_pos, char letra, uint8_t escala, uint16_t COLOR_TEXTO, uint16_t COLOR_FONDO);


void dibujar_Numero(uint16_t x_pos, uint16_t y_pos, uint8_t num, uint8_t escala, uint16_t COLOR_TEXTO, uint16_t COLOR_FONDO);
void dibujar_Punto(uint16_t x, uint16_t y, uint16_t COLOR_TEXTO, uint16_t COLOR_FONDO);
void dibujar_Corazon(uint16_t x_pos, uint16_t y_pos, uint8_t escala, uint16_t COLOR_RELLENO, uint16_t COLOR_FONDO);



#endif