#ifndef ST7789_H
#define ST7789_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gpiod.h>


// Definición de la ruta SPI y GPIO
#define SPI_DEV_PATH "/dev/spidev1.0"
#define CHIP "/dev/gpiochip1"
#define CONSUMER "ST7789-C"

// Definición de pines GPIO
#define RESET 259
#define DC 260
#define BLK 76

// Tamaño de la pantalla
#define WIDTH 320
#define HEIGHT 170

// Offsets para la pantalla
#define XSTART 0
#define YSTART 35


// Declaración de funciones
int spi_init(void);
int gpio_init(void);
int dc_set(int is_data);
int st7789_write_cmd(uint8_t cmd);
int st7789_write_data_bytes(const uint8_t *data, size_t len);
int st7789_write_data_byte(uint8_t data);
int st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
int st7789_init(void);
int st7789_fill_color(uint16_t color);

#endif
