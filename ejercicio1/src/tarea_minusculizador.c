#include "tarea_minusculizador.h"

static void minusculizar(mensaje_entre_tareas_t* mensajeEntreTareas);
static void stackReport(void);

// Implementacion de funcion de la tarea
void minusculizador_task(void* taskParmPtr)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	// ---------- CONFIGURACIONES ------------------------------

	// ---------- REPETIR POR SIEMPRE --------------------------
	while (TRUE)
	{
		xQueueReceive(queMinusculizar, &mensajeEntreTareas, portMAX_DELAY);
		minusculizar(&mensajeEntreTareas);
		xQueueSend(queEnvioUART, &mensajeEntreTareas, portMAX_DELAY);
		stackReport();
	}
}

static void minusculizar(mensaje_entre_tareas_t* mensajeEntreTareas)
{
	for (uint32_t dataIndex = HEADER_LENGTH; dataIndex < (mensajeEntreTareas->length + HEADER_LENGTH); dataIndex++)
	{

		if (mensajeEntreTareas->buffer[dataIndex] >= 65 && mensajeEntreTareas->buffer[dataIndex] <= 90)
		{
			mensajeEntreTareas->buffer[dataIndex] = mensajeEntreTareas->buffer[dataIndex]
					+ DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA;
		}

	}
}

static void stackReport(void)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	QMPool* pool = NULL;
	uint8_t* buffer;

	uint8_t sprintfLength;

	pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);
	buffer = (uint8_t*) QMPool_get(pool, 0);

	if (buffer != NULL)
	{
		buffer[STX_POS] = STX;
		buffer[OP_POS] = STACK;

		sprintfLength = sprintf(&buffer[DATA_POS], "%u", uxTaskGetStackHighWaterMark(NULL));

		buffer[TAM_POS] = sprintfLength;
		buffer[DATA_POS + sprintfLength] = ETX;

		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		mensajeEntreTareas.buffer = buffer;
		mensajeEntreTareas.pool = pool;

		xQueueSend(queEnvioUART, &mensajeEntreTareas, portMAX_DELAY);

	}

}
