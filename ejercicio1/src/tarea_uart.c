#include "tarea_uart.h"

static ascii_message_t ascii_message;
static SemaphoreHandle_t tx_semaforo;
static tarea_uart_rx_callback_t rx_callback;

static void uartRxCallback(void);
static void uartTxCallback(void);
static void uartSend(ascii_message_t ascii_message);

// Implementacion de funcion de la tarea
void uart_task(void* taskParmPtr)
{
	// ---------- CONFIGURACIONES ------------------------------
	// ---------- REPETIR POR SIEMPRE --------------------------

	tx_semaforo = xSemaphoreCreateBinary();

	if (tx_semaforo == NULL)
	{
		// implementar tratamiento de error
	}

	uartInit( UART_USADA, UART_USADA_SPEED);
	uartRxInterruptCallbackSet( UART_USADA, uartRxCallback);
	uartTxInterruptCallbackSet( UART_USADA, uartTxCallback);

	while (TRUE)
	{
		xQueueReceive(queEnvioUART, &ascii_message, portMAX_DELAY);
		uartSend(&ascii_message);
	}
}

void setRxCallback(tarea_uart_rx_callback_t tarea_uart_rx_callback)
{
	rx_callback = tarea_uart_rx_callback;
}

static void uartSend(ascii_message_t* ascii_message)
{
	uint32_t txIndex = 0;
	while (txIndex < ascii_message->length)
	{
		uartTxWrite(UART_USADA, ascii_message->dataPtr[txIndex]);
		xSemaphoreTake(tx_semaforo, portMAX_DELAY);
		txIndex++;
	}
}

static void uartRxCallback(void)
{
	if (rx_callback != NULL)
	{
		rx_callback(uartRxRead(UART_USADA));
	}
}

static void uartTxCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(tx_semaforo, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
