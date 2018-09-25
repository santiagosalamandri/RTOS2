
#ifndef PROCESADOR_PAQUETES_H
#define PROCESADOR_PAQUETES_H

#include "sapi.h"
#include "comunicacionEntreTareas.h"
#include "qmpool.h"



#define STX 0x55
#define ETX 0xAA

#define HEADER_TAIL_LENGTH 	4
#define HEADER_LENGTH 		3

typedef enum
{
	MAYUSCULA, MINUSCULA, STACK, HEAP, APP
} operacion_t;

typedef enum
{
	ESPERANDO_STX, ESPERANDO_OP, ESPERANDO_TAM, ESPERANDO_DATOS, ESPERANDO_ETX, COMPLETO
} estado_proceso_rx_t;

typedef enum
{
	STX_POS = 0,
	OP_POS,
	TAM_POS
} protocol_header_pos_t;

typedef struct {
  uint8_t stx;
  uint8_t op;
  uint8_t tam;
} __attribute__((__packed__)) header_t;


void procesarByteRecibido(uint8_t dato);
void proceso_init(void);


#endif
