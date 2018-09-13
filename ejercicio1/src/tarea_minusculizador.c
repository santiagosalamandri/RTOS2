#include "tarea_minusculizador.h"

static ascii_message_t ascii_message;

static void minusculizar(ascii_message_t ascii_message);

// Implementacion de funcion de la tarea
void minusculizador_task(void* taskParmPtr)
{
	// ---------- CONFIGURACIONES ------------------------------
	// ---------- REPETIR POR SIEMPRE --------------------------
	while (TRUE)
	{
		xQueueReceive(queMinusculizar, &ascii_message, portMAX_DELAY);
		minusculizar(&ascii_message);
		xQueueSend(queMinusculizados, &ascii_message, portMAX_DELAY);
	}
}

static void minusculizar(ascii_message_t* ascii_message)
{
	for (uint32_t i = 0; i < ascii_message->length; i++)
	{
		if (ascii_message->dataPtr[i] >= 65 && ascii_message->dataPtr[i] <= 90)
		{
			ascii_message->dataPtr[i] = ascii_message->dataPtr[i] + DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA;
		}
	}
}
