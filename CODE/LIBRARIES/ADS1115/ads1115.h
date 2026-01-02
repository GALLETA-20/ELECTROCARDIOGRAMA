#ifndef ADS1115_H
#define ADS1115_H


#include <stdint.h>          // LIBRERIA PARA VARIABLES UNIVERSALES TIPO INT


// DEFINICIÓN DE REGISTROS DEL ADS1115
#define ADS1115_REG_LECTURA         0x00 // LECTURA DEL A0.
#define ADS1115_REG_CONFIG          0x01 // REGISTRO PARA CONFIGURACION DEL ADS1115.
#define ADS1115_REG_LO_THRESH       0x02 // REGISTRO PARA VOLTAJE MINIMO.
#define ADS1115_REG_HI_THRESH       0x03 // REGISTRO PARA VOLTAJE MAXIMO.

#define I2C "/dev/i2c-4"
#define ADDR 0x48


int ads1115_init(void);
int abrir_i2c(void);
int config_ads(void);
float leer_adc(void);
void cerrar_i2c(void);


#endif