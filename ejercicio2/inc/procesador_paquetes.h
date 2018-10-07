
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

typedef enum { MAYUSCULA, MINUSCULA, STACK, HEAP, APP,PERFORMANCE} operacion_t;														// Operaciones disponibles

typedef enum { ESPERANDO_STX, ESPERANDO_OP, ESPERANDO_TAM, ESPERANDO_DATOS, ESPERANDO_ETX, COMPLETO } estado_proceso_rx_t;	// Estados disponibles

typedef enum { STX_POS = 0, OP_POS, TAM_POS, DATA_POS } protocol_header_pos_t;												// Orden de campos en paquete

typedef struct {
  uint8_t stx;
  uint8_t op;
  uint8_t tam;
  uint32_t id_de_paquete;
  uint32_t tiempo_de_llegada;
  uint32_t tiempo_de_recepcion;
  uint32_t tiempo_de_inicio;
  uint32_t tiempo_de_fin;
  uint32_t tiempo_de_salida;
  uint32_t tiempo_de_transmision;
  uint16_t largo_del_paquete;
  uint16_t memoria_alojada;
} __attribute__((__packed__)) header_t;

void procesarByteRecibido(uint8_t dato);
void procesador_paquetes_init(void);
void stackReport(void);																	// Reporte de stack disponible
void sizeReport(void);
void heapReport(void);
QMPool* getPool(uint32_t size);

extern QueueHandle_t queMayusculizar;
extern QueueHandle_t queMinusculizar;
extern QueueHandle_t queTransmision;
extern QueueHandle_t queMedirPerformance;

#endif
