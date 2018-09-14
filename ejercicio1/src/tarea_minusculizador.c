#include "tarea_minusculizador.h"

static ascii_message_t ascii_message;

static void minusculizar(ascii_message_t ascii_message);

// Implementacion de funcion de la tarea
void minusculizador_task(void* taskParmPtr)
{
	// ---------- CONFIGURACIONES ------------------------------
	Paquete mensaje;
	// ---------- REPETIR POR SIEMPRE --------------------------
	while (TRUE)
	{
		xQueueReceive(queMinusculizar, &mensaje, portMAX_DELAY);
		minusculizar(&mensaje);
		xQueueSend(queMinusculizados, &mensaje, portMAX_DELAY);
	}
}

static void minusculizar(Paquete *original)
{
	int desp='a'-'A';

	for (size_t i = 0; i < original->tam; i++) {
		if(original->datos[i]>='A'&&original->datos[i]<='Z'){
			original->datos[i]=original->datos[i]+desp;;
		}
		else
			original->datos[i]=original->datos[i];
	}
}
