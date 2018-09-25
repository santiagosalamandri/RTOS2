#include "tarea_uart.h"

static tarea_uart_rx_callback_t rx_callback;

static void uartRxCallback(void);
void uartTxCallback(void);
static void uartTxSendByte(uint8_t byte);
static void uartSend(uint8_t* buffer);

static uint32_t txIndex;
static mensaje_entre_tareas_t mensajeParaTx;

// Implementacion de funcion de la tarea
void uart_task(void* taskParmPtr)
{

	// ---------- CONFIGURACIONES ------------------------------
	// ---------- REPETIR POR SIEMPRE --------------------------

	rx_callback = (tarea_uart_rx_callback_t) taskParmPtr;

	uartInit( UART_USADA, UART_USADA_SPEED);

	uartRxInterruptCallbackSet( UART_USADA, (callBackFuncPtr_t) uartRxCallback);
	uartTxInterruptCallbackSet( UART_USADA, (callBackFuncPtr_t) uartTxCallback);

	uartRxInterruptSet(UART_USB, true);
	uartTxInterruptSet(UART_USB, true);

	while (TRUE)
	{
		xQueueReceive(queEnvioUART, &mensajeParaTx, portMAX_DELAY);
		uartSend(mensajeParaTx.buffer);
	}
}

static void uartSend(uint8_t* buffer)
{
	txIndex = 0;

	uartTxSendByte(buffer[0]);
}

static uint8_t uartGetTxData(uint8_t* txData)
{
	uint8_t result = 0;

	if (txIndex < (mensajeParaTx.buffer[TAM_POS] + HEADER_TAIL_LENGTH))
	{
		*txData = mensajeParaTx.buffer[txIndex];
		txIndex++;
		result = 1;
	}

	return result;
}

static void uartTxSendByte(uint8_t byte)
{
	uartTxWrite(UART_USADA, byte);
}

static void uartRxCallback(void)
{

	if (rx_callback != NULL)
	{
		rx_callback(uartRxRead(UART_USADA));
	}
}

void uartTxCallback(void)
{
	uint8_t txData;

	if (uartGetTxData(&txData))
	{
		uartTxSendByte(txData);
	}
}
