#include "tarea_minusculizador.h"

static void minusculizar(mensaje_entre_tareas_t* mensajeEntreTareas);							// Funcion minusculizadora
extern void stackReport(void);																	// Reporte de stack disponible
extern void sizeReport(void);
extern void timeReport(void);

extern header_t headerEnProceso;

// Implementacion de funcion de la tarea
void minusculizador_task(void* taskParmPtr)
{
	mensaje_entre_tareas_t mensajeEntreTareas;

	while (TRUE) // ---------- REPETIR POR SIEMPRE --------------------------
	{
		xQueueReceive(queMinusculizar, &mensajeEntreTareas, portMAX_DELAY);						// Desencolar dato
		headerEnProceso.tiempo_de_fin =  xTaskGetTickCount() / portTICK_RATE_MS;
		minusculizar(&mensajeEntreTareas);														// Mayusculizar dato
		xQueueSend(queTransmision, &mensajeEntreTareas, portMAX_DELAY);							// Encolar resultado
		stackReport();																			// Reportar stack disponible
		sizeReport();
		//timeReport();
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
