
#include "tarea_mayusculizador.h"

static ascii_message_t ascii_message;

static void mayusculizar(ascii_message_t ascii_message);

// Implementacion de funcion de la tarea
void mayusculizador_task(void* taskParmPtr)
{
	// ---------- CONFIGURACIONES ------------------------------
	// ---------- REPETIR POR SIEMPRE --------------------------
	while (TRUE)
	{
		xQueueReceive(queMayusculizar, &ascii_message, portMAX_DELAY);
		mayusculizar(&ascii_message);
		xQueueSend(queMayusculizados, &ascii_message, portMAX_DELAY);
	}
}

static void mayusculizar(ascii_message_t* ascii_message)
{
	for (uint32_t i = 0; i < ascii_message->length; i++)
	{
		if (ascii_message->dataPtr[i] >= 97 && ascii_message->dataPtr[i] <= 122)
		{
			ascii_message->dataPtr[i] = ascii_message->dataPtr[i] - DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA;
		}

	}
}