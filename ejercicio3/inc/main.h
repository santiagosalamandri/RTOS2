
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Includes de FreeRTOS
#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

// sAPI header
#include "sapi.h"
#include "qmpool.h"

//#include "ISR.h"
#include "procesador_paquetes.h"
#include "tarea_uart.h"
#include "tarea_mayusculizador.h"
#include "tarea_minusculizador.h"
#include "tarea_performance.h"

// Parametros para la creacion de tareas

#define UART_TASK_PRIORITY					(tskIDLE_PRIORITY + 2)
#define MAYUSCULIZADOR_TASK_PRIORITY		(tskIDLE_PRIORITY + 3)
#define MINUSCULIZADOR_TASK_PRIORITY		(tskIDLE_PRIORITY + 3)
#define PERFORMANCE_TASK_PRIORITY			(tskIDLE_PRIORITY + 3)

#define UART_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE * 3)
#define MAYUSCULIZADOR_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 3)
#define MINUSCULIZADOR_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 3)
#define PERFORMANCE_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 3)



