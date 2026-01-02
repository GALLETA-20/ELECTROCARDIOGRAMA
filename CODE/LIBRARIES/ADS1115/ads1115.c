#include "ads1115.h"
#include <stdio.h>           // LIBRERIA PARA IMPRIMIR MENSAJES DE TEXTO Y ERROR 
#include <stdlib.h>          // LIBRERIA EXIT Y SYSTEM
#include <stdint.h>          // LIBRERIA PARA VARIABLES UNIVERSALES TIPO INT
#include <unistd.h>          // FUNCIONES DEL SISTEMA OPERATIVO
#include <fcntl.h>           // FUNCIONE Y CONSTANTES PARA ABRIR ARCHIVOS
#include <sys/ioctl.h>       // FUNCIÓN I/O CONTROL PARA COMUNICACIÓN I2C
#include <linux/i2c-dev.h>   // FUNCION DE CONSTANTES Y MACROS I2C
#include <linux/i2c.h>       // FUNCIÓN DE SECUENCIAS I2C


const float PGA     = 4.096;
const float VAL_ADC = 32767.0;
const float ADC_PGA = PGA / VAL_ADC;


static int ADC = -1;


int ads1115_init(void){
    
    if(abrir_i2c() != 0){
        return -1;
    }

    if(config_ads() != 0){
        return -1;
    }

    usleep(10000);
    return 0;
}


int abrir_i2c(void){
    // Abrimos el archivo I2C
    ADC = open(I2C, O_RDWR);
    if(ADC < 0) {
        perror("open");
        return 1;
    }

    // Seleccionamos la dirección del ADS1115.
    if(ioctl(ADC,I2C_SLAVE, ADDR) < 0){
        perror("ioctl I2C_SLAVE");
        close(ADC);
        return 2;
    }

    return 0;
}



int config_ads(void){
/*  Construimos el registro de configuración del ADS1115.
        - SO - DESPERTAMOS EL ADS1115 COMO MODO SINGLE - SHOT
        - MUX - SELECCIONAMOS LA ENTRADA ANALOGICA QUE QUEREMOS LEER.
        - PGA - ESCALA DE LECTURA (GANANCIA).
        - MODE - CONTINUO O LECTURA ÚNICA.
        - DR - SAMPLES POR SEGUNDOS SPS.
        - COMP_MODE: MODO COMPARADOS.
        - COMP_POL: PLARIDAD DE COMPARADOR.
        - COMP_LAT: ALERTA DE COMPARADOR.
        - COMP_QUE: HABILITA O DESHABILITA EL COMPARADOR.
*/

    // Construimos los byte de configuración.
    unsigned short config = 0;
    
    config |= (1 << 15);     // DESPERTAMOS EL ADS1115;
    config |= (0x4 << 12);   // MUX: SELECCIONAMOS LA ENTRADA AIN0 VS GND.
    config |= (0x1 << 9);    // VOLTAJE BASE: ±4.096V.
    config |= (0 << 8);      // MODO DE OPERACIÓN CONTINUO.
    config |= (0x7 << 5);    // MUESTRAS POR SEGUNDO 860SPS.
    config |= (0 << 4);      // MODO COMPARADOR TRADICIONAL
    config |= (0 << 3);      // POLARIDAD DEL COMPARADOR DESACTIVADO.
    config |= (0 << 2);      // LATCH DESACTIVADO.
    config |= (0x3 << 0);    // COMPRADOR DESACTIVADO


    // ENVIAMOS LA CONFIGURACIÓN
    unsigned char outbuff [3];
    outbuff [0] = ADS1115_REG_CONFIG; // REGISTRO DE CONFIGURACIÓN.
    outbuff [1] = (config >> 8) & 0xFF;
    outbuff [2] = config & 0xFF;

    if (write(ADC, outbuff, 3) != 3){
        perror("Error escribiendo config");
        return -1;
    }

    
    return 0;
}




float leer_adc(void){

    uint8_t reg = ADS1115_REG_LECTURA;
    if(write(ADC, &reg, 1) != 1){
        perror("write lectura");
        return -100.0;
    }

    //usleep(1000);

    unsigned char msg[2];

    ssize_t r = read(ADC, msg, 2);

    if(r != 2){
        perror("read");
        return -100.0;
    }
    int16_t VAL_ADC = (msg[0] << 8) | msg[1];

    float VOLT = VAL_ADC * (ADC_PGA);
    
    return VOLT;
}


void cerrar_i2c(void) {
    if (ADC >= 0) {
        close(ADC);
        ADC = -1;
    }
}
