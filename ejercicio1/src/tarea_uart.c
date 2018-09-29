#include "tarea_uart.h"

static tarea_uart_tx_complete_callback_t txCompleteCallback;

static void uartRxCallback(void);															// Callback de recepcion por UART
void uartTxCallback(void);																	// Callback de transmision por UART
static void uartTxSendByte(uint8_t byte);													// Transmitir Byte por UART
static void uartSend(uint8_t* buffer);														// Enviar por UART

static uint32_t txIndex;																	// Indice de transmision
static mensaje_entre_tareas_t mensajeParaTx;												// Estructura para transmision
SemaphoreHandle_t xSemaphoreTX;																// Semaforo de transmision

// Implementacion de funcion de la tarea
void uart_task(void* taskParmPtr)
{
	// Configuracion
	txCompleteCallback = (tarea_uart_tx_complete_callback_t) taskParmPtr;					//
	xSemaphoreTX = xSemaphoreCreateBinary();												// Creacion de semaforo binario para transmision

	uartInit( UART_USADA, UART_USADA_SPEED);												// Inicializacion de UART

	uartRxInterruptCallbackSet( UART_USADA, (callBackFuncPtr_t) uartRxCallback);			// Agregar funcion a llamar cuando se interrumpa por recepcion
	uartTxInterruptCallbackSet( UART_USADA, (callBackFuncPtr_t) uartTxCallback);			// Agregar funcion a llamar cuando se interrumpa por transmision

	uartRxInterruptSet(UART_USB, true);														// Definir que UART dispara la interrupcion
	uartTxInterruptSet(UART_USB, true);														// Definir que UART dispara la interrupcion

	while (TRUE)	// ---------- REPETIR POR SIEMPRE --------------------------
	{
		xQueueReceive(queEnvioUART, &mensajeParaTx, portMAX_DELAY);							// Desencolar dato a transmitir
		uartSend(mensajeParaTx.buffer);														// Publicar dato desencolado
		xSemaphoreTake(xSemaphoreTX, portMAX_DELAY);										// No iniciar transmision hasta que se termine el buffer entero
	}
}

static void uartSend(uint8_t* buffer)
{
	txIndex = 0;																			// Indice de transmision
	uartTxSendByte(buffer[txIndex++]);														// Enviar byte
}

static uint8_t uartGetTxData(uint8_t* txData)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;											//
	uint8_t result = 0;																		// Resultado de transmision

	if (txIndex < mensajeParaTx.length)														// Si el indice es menor al largo del mensaje
	{
		*txData = mensajeParaTx.buffer[txIndex];											// Guardar valor del elemento en txData
		txIndex++;																			// Incrementar indice de transmision
		result = 1;																			// Actualizar resultado de transmision
	}
	else																					// Paquete transmitido completo
	{
		if (txCompleteCallback != NULL)
		{
			txCompleteCallback(mensajeParaTx.buffer, mensajeParaTx.pool);					// Transmision completa
		}

		xSemaphoreGiveFromISR(xSemaphoreTX, &xHigherPriorityTaskWoken);						// Liberar semaforo de transmision desde interrupcion
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	return result;																			// Devolver resultado de transmision
}

static void uartTxSendByte(uint8_t byte)
{
	uartTxWrite(UART_USADA, byte);															// Imprimir byte por UART
}

static void uartRxCallback(void)
{
	procesarByteRecibido(uartRxRead(UART_USADA));											// Al recibir dato por UART procesarlo
}

void uartTxCallback(void)
{
	uint8_t txData;

	if (uartGetTxData(&txData))																// Obtener dato de UART
	{
		uartTxSendByte(txData);																// Enviar dato por UART
	}
}
