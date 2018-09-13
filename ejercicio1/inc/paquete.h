#ifndef PAQUETE_H
#define PAQUETE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define STX 0x02
#define ETX 0x03

typedef enum  {
ESPERANDO_STX,
ESPERANDO_OP,
ESPERANDO_TAM,
ESPERANDO_DATOS,
ESPERANDO_ETX,
COMPLETO
}ESTADO_PAQUETE;

typedef enum {
MAYUSCULA,
MINUSCULA,
STACK,
HEAP,
APP
}OPERACION;

typedef struct {
  uint8_t stx;
  OPERACION op;
  uint8_t tam;
  uint8_t datos[256];
  uint8_t etx;
  uint8_t cantDatos;
  ESTADO_PAQUETE estado;
}Paquete;

int agregarCaracter(Paquete* paquete,uint8_t* caracter);

void limpiarPaquete(Paquete* paquete);
void toString(Paquete* paquete);
void toStringModificado(Paquete* paquete);

void mayusculizar(Paquete *original,Paquete *mayusculizado);
void minusculizar(Paquete *original,Paquete *minusculizado);

void realizarOperacion(Paquete *original,Paquete *minusculizado);
#endif
