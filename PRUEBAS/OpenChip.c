/* EN ESTE PROGRAMA LO QUE HAGO ES ABRIR Y CERRAR EL CHIP "gpiochip1". */

#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

#define CHIP "/dev/gpiochip1"

int main(){
  //ABRIMOS EL CHIP
  struct gpiod_chip *chip = gpiod_chip_open(CHIP);
  if(!chip){
    perror("Error al abrir el chipgpio1\n");
    return 1;
  } else {
    printf("Muy bien ALALAU, chip abierto exitosamente :D.\n");
  }

  //CERRAMOS EL CHIP
  gpiod_chip_close(chip);
  return 1;
}
