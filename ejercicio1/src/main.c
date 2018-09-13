
/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

// sAPI header
#include "sapi.h"
#include "qmpool.h"

#include "paquete.h"

#define CANTIDAD_PAQUETES 4
#define OVERHEAD CANTIDAD_PAQUETES*2

Paquete paquete;
Paquete paqueteAModificar;


QMPool mem_pool_1; /* Estructura de control del Pool */
static uint8_t memoria_para_pool_1[(sizeof paquete)*CANTIDAD_PAQUETES+OVERHEAD]; /* Espacio de almacenamiento para el Pool */

DEBUG_PRINT_ENABLE;



void onRx( void *noUsado )
{
	char c = uartRxRead( UART_USB );
    agregarCaracter(&paquete,&c);

   //printf( "Recibimos <<%c>> por UART\r\n", c );
}

void myTask( void* taskParmPtr );

int main(void)
{

	boardConfig();
	debugPrintConfigUart( UART_USB, 115200 );
	debugPrintlnString( "Memory Pool con freeRTOS y sAPI." );

	gpioWrite( LED3, ON );

	limpiarPaquete(&paquete);
	limpiarPaquete(&paqueteAModificar);


	QMPool_init(&mem_pool_1,
	memoria_para_pool_1,
	sizeof(memoria_para_pool_1),sizeof paquete);  /* Bloques de 10 bytes cada uno */

	xTaskCreate(
			myTask,                     // Funcion de la tarea a ejecutar
			(const char *)"myTask",     // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			tskIDLE_PRIORITY+1,         // Prioridad de la tarea
			0                           // Puntero a la tarea creada en el sistema
	);

	vTaskStartScheduler();

	while( TRUE ) {
	}


	return 0;
}


void myTask( void* taskParmPtr )
{
	// ---------- INICIALIZACION ------------------------------
	uartConfig( UART_USB, 115200 );
	uartRxInterruptCallbackSet( UART_USB, onRx );
	uartRxInterruptSet( UART_USB, true );

	char msg[50+1];
	sprintf( msg, "Numero de elementos: %d", QMPool_getMin(&mem_pool_1) );
	debugPrintlnString( msg );

	// ---------- REPETIR POR SIEMPRE --------------------------
	while(TRUE) {
		if(paquete.estado==COMPLETO){
		      realizarOperacion(&paquete,&paqueteAModificar);

		      //toString(&paquete);
		      //toStringModificado(&paqueteAModificar);


		      char * block1 = QMPool_get(&mem_pool_1, 0U);
		      snprintf( block1,sizeof paquete, "%d%d%d%s%d",paqueteAModificar.stx,paqueteAModificar.op,paqueteAModificar.tam,paqueteAModificar.datos,paqueteAModificar.etx );
		      printf("%s\n", block1 );
		      QMPool_put(&mem_pool_1, block1);


		      limpiarPaquete(&paqueteAModificar);
		      limpiarPaquete(&paquete);
		    }
		/*
		 char msg[50+1];
			sprintf( msg, "Numero de elementos: %d", QMPool_getMin(&mem_pool_1) );
			debugPrintlnString( msg );
		//Solicito un bloque de memoria
		char * block1 = QMPool_get(&mem_pool_1, 0U);
		// Intercambia el estado del LEDB
		gpioToggle( LEDB );
		strncpy(block1, "Blink!", 10U);
		debugPrintlnString( block1 );
		//Libero el bloque de memoria
		QMPool_put(&mem_pool_1, block1);
		// Envia la tarea al estado bloqueado durante 500ms
		*/
		gpioToggle( LEDB );

		vTaskDelay( 100 / portTICK_RATE_MS );
	}
}

/*==================[fin del archivo]========================================*/
