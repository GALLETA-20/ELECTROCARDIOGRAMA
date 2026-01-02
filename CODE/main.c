/*
 * Proyecto: Detección de Anomalías Cardíacas
 * Archivo: main.c
 *
 * Autor: URBINA ARÉVALO VITALY PAOLO (GALLETA).
 * Gmail: conchipsgalleta@gmail.com
 * Fecha de creación: 2026-01-01
 *
 * Descripción:
 *  Sistema de adquisición y procesamiento de señales ECG
 *  usando ADS1115 y pantalla ST7789.
 *
 * Licencia:
 * Este programa es software libre: puede redistribuirlo y/o modificarlo
 * bajo los términos de la GNU General Public License publicada por
 * la Free Software Foundation, ya sea la versión 3 de la Licencia, o
 * (a su elección) cualquier versión posterior.
 *
 * Este programa se distribuye con la esperanza de que sea útil,
 * pero SIN GARANTÍA ALGUNA; sin siquiera la garantía implícita de
 * COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR.
 * Consulte la GNU General Public License para más detalles.
 *
 * Debería haber recibido una copia de la GNU General Public License
 * junto con este programa. En caso contrario, consulte:
 * https://www.gnu.org/licenses/
 *  © 2026 DORGALL
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "ST7789/st7789.h"
#include "ADS1115/ads1115.h"
#include "FONT/text.h"
#include "GRAPH/graph.h"

// --- CONFIGURACIÓN DE ZONAS ---
#define GRAFICA_ALTO 110      // Altura para la gráfica (0 a 110 px)
#define V_MAX_GRAFICA 3.3     // Voltaje máximo

// UBICACION DEL MSJ
#define UBIXT 10
#define UBIYT 146

// UBICACION DEL NUMERO
#define UBIXEC 200
#define UBIYEC 135

// UBICACION DEL CORAZON
#define UBIXCO 295
#define UBIYCO 180

unsigned char msj[] = "ECG";

// --- VARIABLES GLOBALES ---
extern int spi_fd;
volatile sig_atomic_t corriendo = 1;

// COLORES
uint16_t RGB_TEXTO = 0x06E0;
uint16_t RGB_FONDO = 0x000E;
uint16_t RGB_LINEA = 0x07E0; // Verde neón para la gráfica

uint16_t separacion = 30;  // Ancho numero + espacio
uint16_t separacionT = 15; // Ancho letra + espacio

void handle_sigint(int sig) {
    printf("\nRecibido Ctrl+C. Cerrando...\n");
    corriendo = 0; 
}

// ==========================================
// MAIN
// ==========================================
int main(void) {
    // 1. Inicialización
    if (spi_init() < 0) return 1;
    if (gpio_init() < 0) return 1;
    if (st7789_init() < 0) return 1;
    if (ads1115_init() < 0) return 1;
    
    signal(SIGINT, handle_sigint);

    // 2. Pantalla y Elementos Estáticos
    st7789_fill_color(RGB_FONDO);
    dibujar_Corazon(UBIXCO, UBIYCO, 2, 0xF800, 0x000F);

    uint16_t xT = UBIXT;
    uint16_t yT = UBIYT + 35; 

    // Dibujar texto "ECG"
    for (int i = 0; i < strlen((char*)msj); i++){
        dibujar_letras(xT, yT, msj[i], 1, RGB_TEXTO, RGB_FONDO);
        xT += separacionT;
    }

    // --- VARIABLES PARA LA GRÁFICA ---
    uint16_t x_graph = 0;
    uint16_t y_graph = GRAFICA_ALTO / 2;
    uint16_t y_prev_graph = y_graph;

    printf("Iniciando gráfica y lectura...\n");

    while(corriendo){
        float voltaje = leer_adc();
        if (voltaje < 0) voltaje = 0;

        // ==========================================
        // 1. DIBUJAR GRÁFICA (Osciloscopio)
        // ==========================================
        
        // Mapeo inverso: 0V abajo (GRAFICA_ALTO), 3.3V arriba (0)
        y_graph = GRAFICA_ALTO - (int)((voltaje / V_MAX_GRAFICA) * GRAFICA_ALTO);
        
        // Límites
        if(y_graph >= GRAFICA_ALTO) y_graph = GRAFICA_ALTO - 2;
        if(y_graph < 0) y_graph = 0;

        // Barra de borrado: Limpia 5 píxeles adelante
        for(int k=1; k<=5; k++) {
            gfx_borrar_barra_vertical(x_graph + k, 0, GRAFICA_ALTO, RGB_FONDO);
        }

        // Dibujar línea
        if(x_graph > 0) {
            gfx_draw_line(x_graph - 1, y_prev_graph, x_graph, y_graph, RGB_LINEA);
            gfx_draw_line(x_graph - 1, y_prev_graph + 1, x_graph, y_graph + 1, RGB_LINEA);
        } else {
            gfx_draw_pixel(x_graph, y_graph, RGB_LINEA);
            gfx_draw_pixel(x_graph, y_graph + 1, RGB_LINEA);
        }

        // Avanzar gráfica
        y_prev_graph = y_graph;
        x_graph++;
        if(x_graph >= 320) x_graph = 0; // Vuelta al inicio

        // ==========================================
        // 2. DIBUJAR NÚMEROS
        // ==========================================
        
        // Solo actualizamos números cada 10 píxeles para que la gráfica fluya rápido
        // Si quieres que actualice siempre, quita el 'if'
        if (x_graph % 10 == 0) {
            uint8_t volt[5];
            volt[0] = ((int)voltaje / 10) % 10;
            volt[1] = ((int)voltaje) % 10;
            volt[2] = (((int)(voltaje * 10)) % 10);
            volt[3] = ((int)(voltaje * 100)) % 10;
            volt[4] = ((int)(voltaje * 1000)) % 10;

            uint16_t x = UBIXEC;
            uint16_t y = UBIYEC + 35;

            for(int i = 0; i < 3; i++) {
                dibujar_Numero(x, y, volt[i+1], 2, RGB_TEXTO, RGB_FONDO);
                x += separacion;
            }
        }

        // Pequeño retardo si la gráfica va demasiado rápido
        // usleep(1000); 
    }

    printf("CERRANDO\n");
    cerrar_i2c();
    // close(spi_fd);
    return 0;
}


