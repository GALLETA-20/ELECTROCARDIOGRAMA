#include "text.h"
#include <stdint.h>
#include "~/ElectroCardiograma/ST7789/st7789.h"
#include "font.h"

uint8_t pixel_texto[2]; 
uint8_t pixel_fondo[2];


void dibujar_Numero(uint16_t x_pos, uint16_t y_pos, uint8_t num, uint8_t escala, uint16_t COLOR_TEXTO, uint16_t COLOR_FONDO) {
    
    if (num > 9) return; // Protección

    uint8_t buf[4];
    uint8_t pixel[2];

    // Dimensiones Base (10x14) - ACTUALIZADO A TAMAÑO PERFECTO
    int ancho_base = 13;
    int alto_base = 14;
    
    int ancho_final = ancho_base * escala;
    int alto_final = alto_base * escala;

    // 1. Configurar Ventana X
    st7789_write_cmd(0x2A);
    buf[0] = x_pos >> 8; 
    buf[1] = x_pos & 0xFF;
    buf[2] = (x_pos + ancho_final - 1) >> 8; 
    buf[3] = (x_pos + ancho_final - 1) & 0xFF;
    st7789_write_data_bytes(buf, 4);

    // 2. Configurar Ventana Y
    st7789_write_cmd(0x2B);
    buf[0] = y_pos >> 8; 
    buf[1] = y_pos & 0xFF;
    buf[2] = (y_pos + alto_final - 1) >> 8; 
    buf[3] = (y_pos + alto_final - 1) & 0xFF;
    st7789_write_data_bytes(buf, 4);

    // 3. Escribir
    st7789_write_cmd(0x2C);

    pixel_fondo[0] = COLOR_FONDO >> 8;
    pixel_fondo[1] = COLOR_FONDO & 0xFF;
    pixel_texto[0] = COLOR_TEXTO >> 8;
    pixel_texto[1] = COLOR_TEXTO & 0xFF;

    for(int y = 0; y < alto_final; y++) {
        for(int x = 0; x < ancho_final; x++) {
            
            int x_org = x / escala;
            int y_org = y / escala;

            // Accedemos al array del número específico
            // El array 'numeros' ahora es [10][28] porque son 14 filas * 2 bytes
            int index = (y_org * 2) + (x_org / 8);
            int bit = 7 - (x_org % 8);
            
            // Leemos del bloque correspondiente a 'num'
            int encendido = (numeros[num][index] >> bit) & 0x01;

            if(encendido){
                st7789_write_data_bytes(pixel_texto, 2);
            } else {
                st7789_write_data_bytes(pixel_fondo, 2);
            }
        }
    }
}


void dibujar_Punto(uint16_t x, uint16_t y, uint16_t COLOR_TEXTO, uint16_t COLOR_FONDO) {
    uint8_t buf[4], pixel[2];
    // Punto es 4x14 (para mantener altura de linea, aunque el dibujo es abajo)
    int w = 4; int h = 14; 

    st7789_write_cmd(0x2A);
    buf[0]=x>>8; buf[1]=x&0xFF; buf[2]=(x+w-1)>>8; buf[3]=(x+w-1)&0xFF;
    st7789_write_data_bytes(buf, 4);

    st7789_write_cmd(0x2B);
    buf[0]=y>>8; buf[1]=y&0xFF; buf[2]=(y+h-1)>>8; buf[3]=(y+h-1)&0xFF;
    st7789_write_data_bytes(buf, 4);

    st7789_write_cmd(0x2C);

    pixel_fondo[0] = COLOR_FONDO >> 8;
    pixel_fondo[1] = COLOR_FONDO & 0xFF;
    pixel_texto[0] = COLOR_TEXTO >> 8;
    pixel_texto[1] = COLOR_TEXTO & 0xFF;

    for(int py=0; py<h; py++) {
        for(int px=0; px<w; px++) {
            int bit = 7 - px; // 4 pixeles caben en 1 byte
            if((punto_bmp[py] >> bit) & 0x01) {
                
                st7789_write_data_bytes(pixel_texto, 2);
            } else {
                st7789_write_data_bytes(pixel_fondo, 2);
            }
        }
    }
}


int dibujar_letras(uint16_t x_pos, uint16_t y_pos, char letra, uint8_t escala, uint16_t COLOR_TEXTO, uint16_t COLOR_FONDO){

    if(letra < 'A' || letra > 'z'){
        return 0;
    }

    uint8_t indice = letra - 'A';

    uint8_t buf[4];

    int ancho_base = 16;
    int alto_base = 16;

    int ancho_final = ancho_base * escala;
    int alto_final = ancho_base * escala;

    pixel_fondo[0] = COLOR_FONDO >> 8;
    pixel_fondo[1] = COLOR_FONDO & 0xFF;
    pixel_texto[0] = COLOR_TEXTO >> 8;
    pixel_texto[1] = COLOR_TEXTO & 0xFF;

    //CONFIGURAMOS LA VENTANAS

    st7789_write_cmd(0x2A);
    buf[0] = x_pos >> 8;
    buf[1] = x_pos & 0xFF;
    buf[2] = (x_pos + ancho_final -1) >> 8;
    buf[3] = (x_pos + ancho_final - 1) & 0xFF;
    st7789_write_data_bytes(buf, 4);

    st7789_write_cmd(0x2B);
    buf[0] = y_pos >> 8;
    buf[1] = y_pos & 0xFF;
    buf[2] = (y_pos + alto_final - 1) >> 8;
    buf[3] = (y_pos +  alto_final - 1) & 0xFF;
    st7789_write_data_bytes(buf, 4);

    st7789_write_cmd(0x2C);

    for(int y = 0; y < alto_final; y++){
        int y_zoom = y / escala;

        uint8_t byte1 = letras[indice][y_zoom * 2];
        uint8_t byte2 = letras[indice][y_zoom * 2 + 1];

        uint16_t fila_byte = (byte1 << 8) | byte2;
        
        for (int x = 0; x < ancho_final; x++){

            int x_zoom = x / escala;

            int pixel_prendido = (fila_byte >> (15 - x_zoom) & 0x01);

            if(pixel_prendido){
                st7789_write_data_bytes(pixel_texto, 2);
            } else {
                st7789_write_data_bytes(pixel_fondo, 2);
            }
        }
    }


    return 0;
}


void dibujar_Corazon(uint16_t x_pos, uint16_t y_pos, uint8_t escala, uint16_t COLOR_RELLENO, uint16_t COLOR_FONDO) {
    
    // Dimensiones fijas del icono
    int ancho_base = 10;
    int alto_base = 9; // O 8 si quieres quitar la punta vacía
    
    int ancho_final = ancho_base * escala;
    int alto_final = alto_base * escala;

    // Buffer para colores
    uint8_t p_fill[2] = {COLOR_RELLENO >> 8, COLOR_RELLENO & 0xFF};
    uint8_t p_bg[2]   = {COLOR_FONDO >> 8, COLOR_FONDO & 0xFF};

    // 1. Configurar Ventana X
    uint8_t buf[4];
    st7789_write_cmd(0x2A);
    buf[0] = x_pos >> 8; buf[1] = x_pos & 0xFF;
    buf[2] = (x_pos + ancho_final - 1) >> 8; buf[3] = (x_pos + ancho_final - 1) & 0xFF;
    st7789_write_data_bytes(buf, 4);

    // 2. Configurar Ventana Y
    st7789_write_cmd(0x2B);
    buf[0] = y_pos >> 8; buf[1] = y_pos & 0xFF;
    buf[2] = (y_pos + alto_final - 1) >> 8; buf[3] = (y_pos + alto_final - 1) & 0xFF;
    st7789_write_data_bytes(buf, 4);

    // 3. Escribir datos
    st7789_write_cmd(0x2C);

    for(int y = 0; y < alto_final; y++) {
        int y_org = y / escala;
        
        // Unimos los 2 bytes de la fila
        uint16_t fila_bits = (icono_corazon[y_org][0] << 8) | icono_corazon[y_org][1];

        for(int x = 0; x < ancho_final; x++) {
            int x_org = x / escala;
            
            // Revisamos el bit (leemos de izquierda a derecha desde el bit 15)
            int pixel_on = (fila_bits >> (15 - x_org)) & 0x01;

            if(pixel_on) {
                st7789_write_data_bytes(p_fill, 2); // Color del corazón
            } else {
                st7789_write_data_bytes(p_bg, 2);   // Color de fondo
            }
        }
    }
}