#include "procesador_paquetes.h"
#include "tiempoLeds.h"

void realizarOperacion(uint8_t* buffer, QMPool* pool);

extern header_t headerEnProceso;

estado_proceso_rx_t estado_proceso_rx;

QMPool* pool = NULL;
uint8_t* buffer;

void procesarByteRecibido(uint8_t dato)
{
	static uint16_t bufferIndex;
	uint8_t sprintfLength;

	//gpioToggle(LED3);

	switch (estado_proceso_rx)
	{
	case ESPERANDO_STX:																				// Esperando caracter de inicializacion
		if (dato == STX)
		{
			headerEnProceso.stx = dato;																// Copiar caracter de inicializacion
			headerEnProceso.tiempo_de_llegada = xTaskGetTickCount() / portTICK_RATE_MS;
			estado_proceso_rx = ESPERANDO_OP;														// Esperando operacion
		}
		else																						// Se recibio un caracter de inico no valido, sigo esperando STX
		{ }
		break;
	case ESPERANDO_OP:
		if (dato<OPERACIONESPERMITIDAS)							// Comando valido, debo esperar mas datos
		{
			headerEnProceso.op = dato;																// Copiar caracter de operacion
			headerEnProceso.tiempo_de_inicio = xTaskGetTickCount() / portTICK_RATE_MS;
			estado_proceso_rx = ESPERANDO_TAM;														// Esperando tamaño de dato
		}
		/*
		 else if (dato == STACK || dato == HEAP)														// Si la operacion es calcular heap o stack
		{
			headerEnProceso.op = dato;

			pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);
			buffer = (uint8_t*) QMPool_get(pool, 0);

			if (buffer != NULL)																		// Si el buffer no es nulo guardar datos stx,op
			{
				buffer[STX_POS] = headerEnProceso.stx;
				buffer[OP_POS] = headerEnProceso.op;
				estado_proceso_rx = ESPERANDO_ETX;													// Esperando caracter de finalizacion
			}

		else																					// No hay lugar para almacenar los datos a recibir
			{
				estado_proceso_rx = ESPERANDO_STX;													// Esperando caracter de inicializacion
			}
		}

		 */

		else																						// Se recibio un caracter de operacion no valido, vuelco a esperar STX
		{
			estado_proceso_rx = ESPERANDO_STX;														// Esperando caracter de inicializacion
		}
		break;
	case ESPERANDO_TAM:
		headerEnProceso.tam = dato;

		if (headerEnProceso.tam == 0)																// Se recibio tamaño 0, no voy a esperar datos
		{
			estado_proceso_rx = ESPERANDO_ETX;														// Esperando caracter de finalizacion
		}
		else																						// Se recibio un tamaño correcto
		{
			pool = getPool(headerEnProceso.tam + HEADER_TAIL_LENGTH);
			buffer = (uint8_t*) QMPool_get(pool, 0);
			if (buffer != NULL)																		// Si el buffer no es nulo guardar datos stx,op,tam
			{
				buffer[STX_POS] = headerEnProceso.stx;
				buffer[OP_POS] = headerEnProceso.op;
				buffer[TAM_POS] = headerEnProceso.tam;
				bufferIndex = 3;
				estado_proceso_rx = ESPERANDO_DATOS;												// Esperando recepcion de datos
			}
			else																					// No hay lugar para almacenar los datos a recibir
			{
				estado_proceso_rx = ESPERANDO_STX;													// Esperando caracter de inicializacion
			}
		}
		break;
	case ESPERANDO_DATOS:																			// Esperand recepcion de datos
		buffer[bufferIndex] = dato;

		bufferIndex++;
		if (bufferIndex == (headerEnProceso.tam + HEADER_LENGTH))									// Ya recibi todos los datos, no voy a esperar datos
		{
			estado_proceso_rx = ESPERANDO_ETX;
		}
		break;

	case ESPERANDO_ETX:																				// Esperando caracter de finalizacion
		if(dato == ETX)
		{
			//gpioToggle(LED3);
			buffer[bufferIndex] = ETX;																// Paso el caracter al buffer
			headerEnProceso.tiempo_de_recepcion = xTaskGetTickCount() / portTICK_RATE_MS;
			headerEnProceso.largo_del_paquete = HEADER_LENGTH+headerEnProceso.tam+HEADER_TAIL_LENGTH;
			realizarOperacion(buffer, pool);														// Realizar operacion
			estado_proceso_rx = ESPERANDO_STX;														// Esperando caracter de inicio
		}
		else																						// Recepción incorrecta, descarto trama
		{
			estado_proceso_rx = ESPERANDO_STX;
		}
		break;
	}
}

void procesador_paquetes_init(void)
{
	estado_proceso_rx = ESPERANDO_STX;																// Primer proceso esperando caracter de inicio
}

QMPool* getPool(uint32_t size)
{
	QMPool* pool = NULL;																			// Inicializo el puntero nulo

	// Asignacion de tamaño segun 'size'
	if (size > BLOQUE_POOL_MEDIANO)
	{
		pool = &qmPoolGrande;
		headerEnProceso.memoria_alojada = BLOQUE_POOL_GRANDE;
	}
	else if (size > BLOQUE_POOL_CHICO)
	{
		pool = &qmPoolMediano;
		headerEnProceso.memoria_alojada = BLOQUE_POOL_MEDIANO;
	}
	else
	{
		pool = &qmPoolChico;
		headerEnProceso.memoria_alojada = BLOQUE_POOL_CHICO;
	}

	return pool;
}

void realizarOperacion(uint8_t* buffer, QMPool* pool)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	mensaje_entre_tareas_t mensajeEntreTareas;
	uint8_t sprintfLength;

	mensajeEntreTareas.buffer = buffer;
	mensajeEntreTareas.pool = pool;

	switch (buffer[OP_POS])
	{
	case MAYUSCULA:																					// Mayusculizar
		//gpioToggle(LED1);
		mensajeEntreTareas.length = mensajeEntreTareas.buffer[TAM_POS] + HEADER_TAIL_LENGTH;
		xQueueSendFromISR(queMayusculizar, &mensajeEntreTareas, &xHigherPriorityTaskWoken);			// Encolar puntero a la estructura con el dato a mayusculizar
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		break;
	case MINUSCULA:																					// Minusculizar
		//gpioToggle(LED2);
		mensajeEntreTareas.length = mensajeEntreTareas.buffer[TAM_POS] + HEADER_TAIL_LENGTH;
		xQueueSendFromISR(queMinusculizar, &mensajeEntreTareas, &xHigherPriorityTaskWoken);			// Encolar puntero a la estructura con el dato a minusculizar
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		break;
	case STACK:																						// Calcular stack
//		sprintfLength = sprintf(&buffer[DATA_POS], "Stack:%u", uxTaskGetStackHighWaterMark(NULL));
		sprintfLength = sprintf(&buffer[DATA_POS], "Stack:%u", xPortGetFreeHeapSize());

		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		buffer[TAM_POS] = sprintfLength;
		buffer[DATA_POS + sprintfLength] = ETX;

		xQueueSendFromISR(queTransmision, &mensajeEntreTareas, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		break;
	case HEAP:																						// Calcular heap
		sprintfLength = sprintf(&buffer[DATA_POS], "Heap:%u", xPortGetFreeHeapSize());

		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		buffer[TAM_POS] = sprintfLength;
		buffer[DATA_POS + sprintfLength] = ETX;

		xQueueSendFromISR(queTransmision, &mensajeEntreTareas, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		break;
	case APP:
		//TODO
		break;
	case PERFORMANCE:
		//gpioToggle(LED3);
		headerEnProceso.id_de_paquete ++;
		mensajeEntreTareas.length = mensajeEntreTareas.buffer[TAM_POS] + HEADER_TAIL_LENGTH;
		xQueueSendFromISR(queMedirPerformance, &mensajeEntreTareas, &xHigherPriorityTaskWoken);			// Encolar puntero a la estructura con el dato a minusculizar
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		break;
	case TIEMPOPULSACION:

	//	headerEnProceso.tiempo_de_fin =  xTaskGetTickCount() / portTICK_RATE_MS;
		sprintfLength = sprintf(&buffer[DATA_POS], "T1:%u mS,T2:%u mS,T3:%u mS,T4:%u mS",getTiempoPulsador(0),getTiempoPulsador(1),getTiempoPulsador(2),getTiempoPulsador(3));
		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		buffer[TAM_POS] = sprintfLength;
		buffer[DATA_POS + sprintfLength] = ETX;

		xQueueSendFromISR(queTransmision, &mensajeEntreTareas, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		inicializarTiemposPulsadores();
		break;
	default:
		break;
	}

}

void stackReport(void)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	QMPool* pool = NULL;																// Inicializo nulo el puntero al pool
	uint8_t* buffer;																	// Buffer arreglo de uint8
	uint8_t sprintfLength;																// Largo necesario para el sprintf

	pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);						// Calculando cantidad de memoria
	buffer = (uint8_t*) QMPool_get(pool, 0);											// Asignando memoria al buffer

	if (buffer != NULL)																	// Si el puntero no es nulo
	{
		// Asignacion de datos del paquete a procesar
		buffer[STX_POS] = STX;															// Cabecera del paquete
		buffer[OP_POS] = STACK;															// Operacion del paquete

		sprintfLength = sprintf(&buffer[DATA_POS], "%u", uxTaskGetStackHighWaterMark(NULL));

		buffer[TAM_POS] = sprintfLength;												// Tamaño del paquete
		buffer[DATA_POS + sprintfLength] = ETX;											// Final del paquete

		// Pasaje de datos a la estructura
		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		mensajeEntreTareas.buffer = buffer;
		mensajeEntreTareas.pool = pool;

		xQueueSend(queTransmision, &mensajeEntreTareas, portMAX_DELAY);					// Encolado de puntero a la estructura
	}
}

void timeReport(void)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	QMPool* pool = NULL;																// Inicializo nulo el puntero al pool
	uint8_t* buffer;																	// Buffer arreglo de uint8
	uint8_t sprintfLength;																// Largo necesario para el sprintf

	pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);						// Calculando cantidad de memoria
	buffer = (uint8_t*) QMPool_get(pool, 0);											// Asignando memoria al buffer

	if (buffer != NULL)																	// Si el puntero no es nulo
	{
		// Asignacion de datos del paquete a procesar
		buffer[STX_POS] = STX;															// Cabecera del paquete
		buffer[OP_POS] = PERFORMANCE;															// Operacion del paquete

		sprintfLength = sprintf(&buffer[DATA_POS], "%u|%u|%u|%u|%u|%u",
				headerEnProceso.tiempo_de_llegada,
				headerEnProceso.tiempo_de_recepcion,
				headerEnProceso.tiempo_de_inicio,
				headerEnProceso.tiempo_de_fin,
				headerEnProceso.tiempo_de_salida,
				headerEnProceso.tiempo_de_transmision);

		buffer[TAM_POS] = sprintfLength;												// Tamaño del paquete
		buffer[DATA_POS + sprintfLength] = ETX;											// Final del paquete

		// Pasaje de datos a la estructura
		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		mensajeEntreTareas.buffer = buffer;
		mensajeEntreTareas.pool = pool;

		xQueueSend(queTransmision, &mensajeEntreTareas, portMAX_DELAY);					// Encolado de puntero a la estructura
	}
}

void sizeReport(void)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	QMPool* pool = NULL;																// Inicializo nulo el puntero al pool
	uint8_t* buffer;																	// Buffer arreglo de uint8
	uint8_t sprintfLength;																// Largo necesario para el sprintf

	pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);						// Calculando cantidad de memoria
	buffer = (uint8_t*) QMPool_get(pool, 0);											// Asignando memoria al buffer

	if (buffer != NULL)																	// Si el puntero no es nulo
	{
		// Asignacion de datos del paquete a procesar
		buffer[STX_POS] = STX;															// Cabecera del paquete
		buffer[OP_POS] = PERFORMANCE;															// Operacion del paquete

		sprintfLength = sprintf(&buffer[DATA_POS], "%u|%u", headerEnProceso.largo_del_paquete,headerEnProceso.memoria_alojada);

		buffer[TAM_POS] = sprintfLength;												// Tamaño del paquete
		buffer[DATA_POS + sprintfLength] = ETX;											// Final del paquete

		// Pasaje de datos a la estructura
		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		mensajeEntreTareas.buffer = buffer;
		mensajeEntreTareas.pool = pool;

		xQueueSend(queTransmision, &mensajeEntreTareas, portMAX_DELAY);					// Encolado de puntero a la estructura
	}
}

void heapReport(void)
{
	mensaje_entre_tareas_t mensajeEntreTareas;
	QMPool* pool = NULL;													// Inicializo nulo el puntero al pool
	uint8_t* buffer;														// Buffer arreglo de uint8
	uint8_t sprintfLength;													// Largo necesario para el sprintf

	pool = getPool(MAX_UINT32_STRING_LENGTH + HEADER_TAIL_LENGTH);			// Calculando cantidad de memoria
	buffer = (uint8_t*) QMPool_get(pool, 0);								// Asignando memoria al buffer

	if (buffer != NULL)														// Si el puntero no es nulo
	{
		// Asignacion de datos del paquete a procesar
		buffer[STX_POS] = STX;												// Cabecera del paquete
		buffer[OP_POS] = HEAP;												// Operacion del paquete

		sprintfLength = sprintf(&buffer[DATA_POS], "%u", xPortGetFreeHeapSize());

		buffer[TAM_POS] = sprintfLength;									// Tamaño del paquete
		buffer[DATA_POS + sprintfLength] = ETX;								// Final del paquete

		// Pasaje de datos a la estructura
		mensajeEntreTareas.length = sprintfLength + HEADER_TAIL_LENGTH;
		mensajeEntreTareas.buffer = buffer;
		mensajeEntreTareas.pool = pool;

		xQueueSend(queTransmision, &mensajeEntreTareas, portMAX_DELAY);		// Encolado de puntero a la estructura
	}
}
