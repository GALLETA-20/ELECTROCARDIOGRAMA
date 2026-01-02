#include <stdlib.h> // Para abs()
#include "graph.h"
#include "~/ElectroCardiograma/ST7789/st7789.h" // Necesitamos acceso a las funciones de bajo nivel

// Dibuja un píxel individual
void gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= 320 || y >= 170) return; // Protección de bordes

    uint8_t buf[2] = {color >> 8, color & 0xFF};
    
    // Configuramos una ventana de 1x1 píxel
    st7789_set_window(x, y, x, y); 
    st7789_write_cmd(0x2C); // RAMWR
    st7789_write_data_bytes(buf, 2);
}

// Algoritmo de Bresenham para líneas
void gfx_draw_line(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        gfx_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Borra una columna vertical eficientemente
void gfx_borrar_barra_vertical(uint16_t x, uint16_t y_min, uint16_t y_max, uint16_t color) {
    if(x >= 320) return;

    int alto = y_max - y_min + 1;
    
    // Configuramos una ventana vertical de 1 píxel de ancho
    st7789_set_window(x, y_min, x, y_max);
    st7789_write_cmd(0x2C);

    // Preparamos el color
    uint8_t high = color >> 8;
    uint8_t low = color & 0xFF;
    uint8_t pixel[2] = {high, low};

    // Enviamos el color repetidamente para llenar la columna
    // Esto es más rápido que llamar a draw_pixel muchas veces
    for(int i = 0; i < alto; i++){
        st7789_write_data_bytes(pixel, 2);
    }
}