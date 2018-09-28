#include "tarea_uart.h"

static tarea_uart_tx_complete_callback_t txCompleteCallback;

static void uartRxCallback(void);
void uartTxCallback(void);
static void uartTxSendByte(uint8_t byte);
static void uartSend(uint8_t* buffer);

static uint32_t txIndex;
static mensaje_entre_tareas_t mensajeParaTx;
SemaphoreHandle_t xSemaphoreTX;

// Implementacion de funcion de la tarea
void uart_task(void* taskParmPtr)
{

	// ---------- CONFIGURACIONES ------------------------------
	// ---------- REPETIR POR SIEMPRE --------------------------

	txCompleteCallback = (tarea_uart_tx_complete_callback_t) taskParmPtr;

	xSemaphoreTX = xSemaphoreCreateBinary();

	uartInit( UART_USADA, UART_USADA_SPEED);

	uartRxInterruptCallbackSet( UART_USADA, (callBackFuncPtr_t) uartRxCallback);
	uartTxInterruptCallbackSet( UART_USADA, (callBackFuncPtr_t) uartTxCallback);

	uartRxInterruptSet(UART_USB, true);
	uartTxInterruptSet(UART_USB, true);

	while (TRUE)
	{
		xQueueReceive(queEnvioUART, &mensajeParaTx, portMAX_DELAY);
		uartSend(mensajeParaTx.buffer);
		// no vuelvo a iniciar una transmisión hasta tanto se termine de transmitir el buffer entero
		xSemaphoreTake(xSemaphoreTX, portMAX_DELAY);
	}
}

static void uartSend(uint8_t* buffer)
{
	txIndex = 0;

	uartTxSendByte(buffer[txIndex++]);
}

static uint8_t uartGetTxData(uint8_t* txData)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint8_t result = 0;

	if (txIndex < mensajeParaTx.length)
	{
		*txData = mensajeParaTx.buffer[txIndex];
		txIndex++;
		result = 1;
	}
	else
	{

		// paquete transmitido completo

		if (txCompleteCallback != NULL)
		{

			txCompleteCallback(mensajeParaTx.buffer, mensajeParaTx.pool);
		}

		xSemaphoreGiveFromISR(xSemaphoreTX, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	return result;
}

static void uartTxSendByte(uint8_t byte)
{
	uartTxWrite(UART_USADA, byte);

}

static void uartRxCallback(void)
{

	procesarByteRecibido(uartRxRead(UART_USADA));

}

void uartTxCallback(void)
{
	uint8_t txData;

	if (uartGetTxData(&txData))
	{
		uartTxSendByte(txData);
	}
}
