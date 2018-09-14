#include "tarea_mayusculizador.h"

static ascii_message_t ascii_message;

static void mayusculizar(ascii_message_t ascii_message);

// Implementacion de funcion de la tarea
void mayusculizador_task(void* taskParmPtr)
{
	// ---------- CONFIGURACIONES ------------------------------
	Paquete mensaje;
	// ---------- REPETIR POR SIEMPRE --------------------------
	while (TRUE)
	{
		xQueueReceive(queMayusculizar, &mensaje, portMAX_DELAY);
		mayusculizar(mensaje);
		xQueueSend(queMayusculizados, &mensaje, portMAX_DELAY);
	}
}

static void mayusculizar(Paquete *original) // USAR ESTA
{
	int desp='a'-'A';

	for (size_t i = 0; i < original->tam; i++) {
		if(original->datos[i]>='A'&& original->datos[i]<='Z'){
			original->datos[i]=original->datos[i]-desp;;
		}
		else
			original->datos[i]=original->datos[i];
	}
}
