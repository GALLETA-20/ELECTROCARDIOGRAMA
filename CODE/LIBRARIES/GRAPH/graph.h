#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <stdint.h>

#define COLOR_FONDO_GRAFICA 0X000F
#define COLOR_LINEA         0x07E0
#define COLOR_GRID          0x18E3
#define COLOR_TEXTO_VALOR   0xFFFF



void gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void gfx_draw_line(int x0, int y0, int x1, int y1, uint16_t color);

void gfx_borrar_barra_vertical(uint16_t x, uint16_t y_min, uint16_t y_max, uint16_t color);

#endif