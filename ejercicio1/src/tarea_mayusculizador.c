#include "tarea_mayusculizador.h"

static void mayusculizar(mensaje_entre_tareas_t* mensajeEntreTareas);

// Implementacion de funcion de la tarea
void mayusculizador_task(void* taskParmPtr)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	// ---------- CONFIGURACIONES ------------------------------

	// ---------- REPETIR POR SIEMPRE --------------------------
	while (TRUE)
	{
		xQueueReceive(queMayusculizar, &mensajeEntreTareas, portMAX_DELAY);
		mayusculizar(&mensajeEntreTareas);
		xQueueSend(queMayusculizados, &mensajeEntreTareas, portMAX_DELAY);
	}
}

static void mayusculizar(mensaje_entre_tareas_t* mensajeEntreTareas) // USAR ESTA
{
	for (uint32_t dataIndex = HEADER_LENGTH; dataIndex < mensajeEntreTareas->buffer[TAM_POS]; dataIndex++)
	{

		if (mensajeEntreTareas->buffer[dataIndex] >= 97 && mensajeEntreTareas->buffer[dataIndex] <= 122)
		{
			mensajeEntreTareas->buffer[dataIndex] = mensajeEntreTareas->buffer[dataIndex] - DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA;
		}

	}
}
