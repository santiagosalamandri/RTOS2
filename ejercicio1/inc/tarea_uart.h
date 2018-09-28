
#pragma once

#include "sapi.h"
#include "comunicacionEntreTareas.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define UART_USADA			UART_USB
#define UART_USADA_SPEED	115200

typedef void (*tarea_uart_tx_complete_callback_t)(uint8_t* buffer, QMPool* pool);

void uart_task(void* taskParmPtr);




typedef struct {
  uint8_t stx;
  operacion_t op;
  uint8_t tam;
  uint8_t* buffer;
  uint8_t etx;
}uart_tx_paquete;
