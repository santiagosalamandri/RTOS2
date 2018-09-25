#include "tarea_minusculizador.h"

static void minusculizar(mensaje_entre_tareas_t* mensajeEntreTareas);

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
		xQueueSend(queMinusculizados, &mensajeEntreTareas, portMAX_DELAY);
	}
}

static void minusculizar(mensaje_entre_tareas_t* mensajeEntreTareas)
{

	for (uint32_t dataIndex = HEADER_LENGTH; dataIndex < mensajeEntreTareas->buffer[TAM_POS]; dataIndex++)
	{

		if (mensajeEntreTareas->buffer[dataIndex] >= 65 && mensajeEntreTareas->buffer[dataIndex] <= 90)
		{
			mensajeEntreTareas->buffer[dataIndex] = mensajeEntreTareas->buffer[dataIndex] + DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA;
		}

	}
}
