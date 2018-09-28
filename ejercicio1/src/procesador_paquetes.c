#include "procesador_paquetes.h"

static void realizarOperacion(uint8_t* buffer, QMPool* pool);

static header_t headerEnProceso;

static estado_proceso_rx_t estado_proceso_rx;

QMPool* pool = NULL;
uint8_t* buffer;

void procesarByteRecibido(uint8_t dato)
{
	static uint16_t bufferIndex;
	uint8_t sprintfLength;

	gpioToggle(LED3);

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
		else if (dato == STACK || dato == HEAP)
		{
			headerEnProceso.op = dato;

			pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);
			buffer = (uint8_t*) QMPool_get(pool, 0);

			if (buffer != NULL)
			{
				buffer[STX_POS] = headerEnProceso.stx;
				buffer[OP_POS] = headerEnProceso.op;
				estado_proceso_rx = ESPERANDO_ETX;
			}
			else
			{
				// no hay lugar para almacenar los datos a recibir
				estado_proceso_rx = ESPERANDO_STX;
			}

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
			pool = getPool(headerEnProceso.tam + HEADER_TAIL_LENGTH);
			buffer = (uint8_t*) QMPool_get(pool, 0);
			if (buffer != NULL)
			{
				buffer[STX_POS] = headerEnProceso.stx;
				buffer[OP_POS] = headerEnProceso.op;
				buffer[TAM_POS] = headerEnProceso.tam;
				bufferIndex = 3;
				estado_proceso_rx = ESPERANDO_DATOS;
			}
			else
			{
				// no hay lugar para almacenar los datos a recibir
				estado_proceso_rx = ESPERANDO_STX;
			}

		}

		break;
	case ESPERANDO_DATOS:

		buffer[bufferIndex] = dato;

		bufferIndex++;
		if (bufferIndex == (headerEnProceso.tam + HEADER_LENGTH))
		{
			// ya recibi todos los datos, no voy a esperar datos
			estado_proceso_rx = ESPERANDO_ETX;
		}

		break;

	case ESPERANDO_ETX:
		if (dato == ETX)
		{
			gpioToggle(LED3);
			buffer[bufferIndex] = ETX;
			realizarOperacion(buffer, pool);
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

void procesador_paquetes_init(void)
{
	estado_proceso_rx = ESPERANDO_STX;
}

QMPool* getPool(uint32_t size)
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

static void realizarOperacion(uint8_t* buffer, QMPool* pool)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	mensaje_entre_tareas_t mensajeEntreTareas;
	uint8_t sprintfLength;

	mensajeEntreTareas.buffer = buffer;
	mensajeEntreTareas.pool = pool;

	switch (buffer[OP_POS])
	{
	case MAYUSCULA:
		gpioToggle(LED1);
		mensajeEntreTareas.length = mensajeEntreTareas.buffer[TAM_POS] + HEADER_TAIL_LENGTH;
		xQueueSendFromISR(queMayusculizar, &mensajeEntreTareas, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
		;
		break;
	case MINUSCULA:
		gpioToggle(LED2);
		mensajeEntreTareas.length = mensajeEntreTareas.buffer[TAM_POS] + HEADER_TAIL_LENGTH;
		xQueueSendFromISR(queMinusculizar, &mensajeEntreTareas, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
		;
		break;
	case STACK:

		sprintfLength = sprintf(&buffer[DATA_POS], "%u", uxTaskGetStackHighWaterMark(NULL));

		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		buffer[TAM_POS] = sprintfLength;
		buffer[DATA_POS + sprintfLength] = ETX;

		xQueueSendFromISR(queEnvioUART, &mensajeEntreTareas, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
		break;
	case HEAP:

		sprintfLength = sprintf(&buffer[DATA_POS], "%u", xPortGetFreeHeapSize());

		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		buffer[TAM_POS] = sprintfLength;
		buffer[DATA_POS + sprintfLength] = ETX;

		xQueueSendFromISR(queEnvioUART, &mensajeEntreTareas, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken)

		break;
	case APP:
		break;
	default:
		break;
	}

}

