
#pragma once

#include "sapi.h"
#include "comunicacionEntreTareas.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define UART_USADA			UART_USB
#define UART_USADA_SPEED	115200

typedef void (*tarea_uart_rx_callback_t)(uint8_t dato);

void uart_task(void* taskParmPtr);
void setRxCallback(tarea_uart_rx_callback_t tarea_uart_rx_callback);
