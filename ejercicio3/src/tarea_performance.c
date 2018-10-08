#include "tarea_performance.h"

static void performance(mensaje_entre_tareas_t* mensajeEntreTareas);							// Funcion mayusculizadora
void stackReport(void);																	// Reporte de stack disponible
void sizeReport(void);

// Implementacion de funcion de la tarea
void performance_task(void* taskParmPtr)
{
	mensaje_entre_tareas_t mensajeEntreTareas;

	while (TRUE) // ---------- REPETIR POR SIEMPRE --------------------------
	{
		xQueueReceive(queMedirPerformance, &mensajeEntreTareas, portMAX_DELAY);						// Desencolar dato
		performance(&mensajeEntreTareas);														// Mayusculizar dato
		xQueueSend(queTransmision, &mensajeEntreTareas, portMAX_DELAY);							// Encolar resultado
		stackReport();																			// Reportar stack disponible
		sizeReport();
	}
}

static void performance(mensaje_entre_tareas_t* mensajeEntreTareas)
{
	for (uint32_t dataIndex = HEADER_LENGTH; dataIndex < (mensajeEntreTareas->length + HEADER_LENGTH); dataIndex++)
	{	// Iterar entre la porcion de paquete correspondiente al payload

		if (mensajeEntreTareas->buffer[dataIndex] >= LETRA_a && mensajeEntreTareas->buffer[dataIndex] <= LETRA_z)
		{	// Si se encuentra entre 'a' y 'z' entonces pasar a mayuscula restando la diferencia entre mayus y minus.
			mensajeEntreTareas->buffer[dataIndex] = mensajeEntreTareas->buffer[dataIndex] - DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA;
		}
	}
}
