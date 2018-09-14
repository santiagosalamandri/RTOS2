
// Includes de FreeRTOS
#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

// sAPI header
#include "sapi.h"
#include "qmpool.h"
#include "paquete.h"

//#include "ISR.h"
#include "tarea_uart.h"



#define UART_TASK_PRIORITY				(tskIDLE_PRIORITY + 2)



#define UART_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 2)



