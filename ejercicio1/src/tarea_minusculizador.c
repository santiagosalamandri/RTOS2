#include "tarea_minusculizador.h"

static void minusculizar(mensaje_entre_tareas_t* mensajeEntreTareas);							// Funcion minusculizadora
static void stackReport(void);																	// Reporte de stack disponible

// Implementacion de funcion de la tarea
void minusculizador_task(void* taskParmPtr)
{
	mensaje_entre_tareas_t mensajeEntreTareas;

	while (TRUE) // ---------- REPETIR POR SIEMPRE --------------------------
	{
		xQueueReceive(queMinusculizar, &mensajeEntreTareas, portMAX_DELAY);						// Desencolar dato
		minusculizar(&mensajeEntreTareas);														// Mayusculizar dato
		xQueueSend(queEnvioUART, &mensajeEntreTareas, portMAX_DELAY);							// Encolar resultado
		stackReport();																			// Reportar stack disponible
	}
}

static void minusculizar(mensaje_entre_tareas_t* mensajeEntreTareas)
{
	for (uint32_t dataIndex = HEADER_LENGTH; dataIndex < (mensajeEntreTareas->length + HEADER_LENGTH); dataIndex++)
	{	// Iterar entre la porcion de paquete correspondiente al payload

		if (mensajeEntreTareas->buffer[dataIndex] >= LETRA_A && mensajeEntreTareas->buffer[dataIndex] <= LETRA_Z)
		{	// Si se encuentra entre 'A' y 'Z' entonces pasar a minuscula sumando la diferencia entre mayus y minus.
			mensajeEntreTareas->buffer[dataIndex] = mensajeEntreTareas->buffer[dataIndex] + DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA;
		}
	}
}

static void stackReport(void)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	QMPool* pool = NULL;																// Inicializo nulo el puntero al pool
	uint8_t* buffer;																	// Buffer arreglo de uint8
	uint8_t sprintfLength;																// Largo necesario para el sprintf

	pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);						// Calculando cantidad de memoria
	buffer = (uint8_t*) QMPool_get(pool, 0);											// Asignando memoria al buffer

	if (buffer != NULL)																	// Si el puntero no es nulo
	{
		// Asignacion de datos del paquete a procesar
		buffer[STX_POS] = STX;															// Cabecera del paquete
		buffer[OP_POS] = HEAP;															// Operacion del paquete

		sprintfLength = sprintf(&buffer[DATA_POS], "%u", uxTaskGetStackHighWaterMark(NULL));

		buffer[TAM_POS] = sprintfLength;												// Tamaño del paquete
		buffer[DATA_POS + sprintfLength] = ETX;											// Final del paquete

		// Pasaje de datos a la estructura
		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		mensajeEntreTareas.buffer = buffer;
		mensajeEntreTareas.pool = pool;

		xQueueSend(queEnvioUART, &mensajeEntreTareas, portMAX_DELAY);					// Encolado de puntero a la estructura
	}
}
