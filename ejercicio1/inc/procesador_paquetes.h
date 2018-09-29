
#ifndef PROCESADOR_PAQUETES_H
#define PROCESADOR_PAQUETES_H

#include "sapi.h"
#include "comunicacionEntreTareas.h"
#include "qmpool.h"

#define STX 0x55																											// Caracter de inicializacion
#define ETX 0xAA																											// Caracter de finalizacion

#define HEADER_TAIL_LENGTH 			4																						// Largo de cabecera final
#define HEADER_LENGTH 				3																						// Largo de cabecera inicial

#define MAX_UINT32_STRING_LENGTH	10

typedef enum { MAYUSCULA, MINUSCULA, STACK, HEAP, APP } operacion_t;														// Operaciones disponibles

typedef enum { ESPERANDO_STX, ESPERANDO_OP, ESPERANDO_TAM, ESPERANDO_DATOS, ESPERANDO_ETX, COMPLETO } estado_proceso_rx_t;	// Estados disponibles

typedef enum { STX_POS = 0, OP_POS, TAM_POS, DATA_POS } protocol_header_pos_t;												// Orden de campos en paquete

typedef struct {
  uint8_t stx;
  uint8_t op;
  uint8_t tam;
} __attribute__((__packed__)) header_t;

void procesarByteRecibido(uint8_t dato);
void procesador_paquetes_init(void);
QMPool* getPool(uint32_t size);

#endif
