#include "procesador_paquetes.h"

static void realizarOperacion(uint8_t* buffer);
static QMPool* getPool(uint32_t size);

static header_t headerEnProceso;

static estado_proceso_rx_t estado_proceso_rx;

QMPool* pool = NULL;
uint8_t* buffer = NULL;
uint16_t bufferIndex;

void procesarByteRecibido(uint8_t dato)
{
	gpioToggle( LEDB );

	switch (estado_proceso_rx)
	{
	case ESPERANDO_STX:
		if (dato == STX)
		{
			headerEnProceso.stx = dato;
			estado_proceso_rx = ESPERANDO_OP;
		}
		else
		{
			// se recibio un caracter de inico no valido, sigo esperando STX
		}
		break;
	case ESPERANDO_OP:
		if (dato <= MINUSCULA)
		{
			// comando valido, debo esperar mas datos
			headerEnProceso.op = dato;
			estado_proceso_rx = ESPERANDO_TAM;
		}
		else if (dato <= APP)
		{
			// comando de operación, NO debo esperar datos a convertir ni longitud

		}
		else
		{
			// se recibio un caracter de operacion no valido, vuelco a esperar STX
			estado_proceso_rx = ESPERANDO_STX;
		}
		break;
	case ESPERANDO_TAM:
		headerEnProceso.tam = dato;

		if (headerEnProceso.tam == 0)
		{
			// se recibio tamaño 0, no voy a esperar datos
			estado_proceso_rx = ESPERANDO_ETX;
		}
		else
		{
			// se recibio un tamaño correcto
			//pool = getPool(headerEnProceso.tam + HEADER_TAIL_LENGTH);
			buffer = QMPool_get(&qmPoolChico, 0);
			/*if (buffer != NULL)
			{
				buffer[0] = headerEnProceso.stx;
				buffer[1] = headerEnProceso.op;
				buffer[2] = headerEnProceso.tam;
				bufferIndex = 3;
				estado_proceso_rx = ESPERANDO_DATOS;
			}
			else
			{
				// no hay lugar para almacenar los datos a recibir
				estado_proceso_rx = ESPERANDO_STX;
			}*/
		}

		break;
	case ESPERANDO_DATOS:

		buffer[bufferIndex++] = dato;

		if (bufferIndex == (headerEnProceso.tam + HEADER_LENGTH))
		{
			// ya recibi todos los datos, no voy a esperar datos
			estado_proceso_rx = ESPERANDO_ETX;
		}

		break;
	case ESPERANDO_ETX:
		if (dato == ETX)
		{
			buffer[bufferIndex] = ETX;
			realizarOperacion(buffer);
			estado_proceso_rx = ESPERANDO_STX;
		}
		else
		{
			// recepción incorrecta, descarto trama
			estado_proceso_rx = ESPERANDO_STX;
		}
		break;

	}

}

void proceso_init(void)
{
	estado_proceso_rx = ESPERANDO_STX;
}

static QMPool* getPool(uint32_t size)
{
	QMPool* pool = NULL;

	if (size > BLOQUE_POOL_MEDIANO)
	{
		pool = &qmPoolGrande;

	}
	else if (size > BLOQUE_POOL_CHICO)
	{
		pool = &qmPoolMediano;
	}
	else
	{
		pool = &qmPoolChico;
	}

	return pool;
}

static void realizarOperacion(uint8_t* buffer)
{
	mensaje_entre_tareas_t mensajeEntreTareas;

	mensajeEntreTareas.buffer = buffer;

	switch (buffer[OP_POS])
	{
	case MAYUSCULA:
		xQueueSend(queMayusculizar, &mensajeEntreTareas, portMAX_DELAY);
		break;
	case MINUSCULA:
		xQueueSend(queMinusculizar, &mensajeEntreTareas, portMAX_DELAY);
		break;
	case STACK:
		break;
	case HEAP:
		break;
	case APP:
		break;
	default:
		break;
	}
}

