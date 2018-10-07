
#pragma once

#include "sapi.h"
#include "comunicacionEntreTareas.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define UART_USADA			UART_USB												// Puerto UART utilizado
#define UART_USADA_SPEED	115200													// Baudrate para lectura/escritura del puerto

typedef void (*tarea_uart_tx_complete_callback_t)(uint8_t* buffer, QMPool* pool);

void uart_task(void* taskParmPtr);


typedef struct {
  uint8_t stx;																		// Caracter de inicializacion
  operacion_t op;																	// Caracter de operacion
  uint8_t tam;																		// Tamaño del dato
  uint8_t* buffer;																	// Buffer para almacenar el dato
  uint8_t etx;																		// Caracter de finalizacion
}uart_tx_paquete;
