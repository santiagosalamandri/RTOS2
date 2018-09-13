/* Copyright 2017-2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[inlcusiones]============================================*/


#include "main.h"

/*==================[definiciones y macros]==================================*/

#define CANTIDAD_PAQUETES 4
#define OVERHEAD CANTIDAD_PAQUETES*2

Paquete paquete;
Paquete paqueteAModificar;

QMPool mem_pool_1; /* Estructura de control del Pool */
static uint8_t memoria_para_pool_1[(sizeof paquete)*CANTIDAD_PAQUETES+OVERHEAD]; /* Espacio de almacenamiento para el Pool */

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

QueueHandle_t queMayusculizar;
QueueHandle_t queMayusculizados;
QueueHandle_t queMinusculizar;
QueueHandle_t queMinusculizados;
QueueHandle_t queEnvioUART;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

static printFln_queue_message_t printFln_queue_message;
static void queues_init(void);
static void uart_task_create(void);

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
	// ---------- CONFIGURACIONES ------------------------------
	
	boardConfig(); 	// Inicializar y configurar la plataforma
	queues_init();
	gpioWrite(LEDG, ON); 	// Led indicador programa corriendo	
	uart_task_create();		// Crear tarea UART
	Manejador_paquete_create(); // Crear tarea para manejar paquetes
	
	limpiarPaquete(&paquete);
	limpiarPaquete(&paqueteAModificar);
	QMPool_init(&mem_pool_1,
	memoria_para_pool_1,
	sizeof(memoria_para_pool_1),sizeof paquete);  /* Bloques de 10 bytes cada uno */

	
	
	vTaskStartScheduler();	// Iniciar scheduler

	while ( TRUE)		// ---------- REPETIR POR SIEMPRE --------------------------
	{
		// Si cae en este while 1 significa que no pudo iniciar el scheduler
	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}

/*==================[definiciones de funciones internas]=====================*/

static void queues_init(void)
{
	queMayusculizar = xQueueCreate(16, sizeof(ascii_message_t));
	queMayusculizados = xQueueCreate(16, sizeof(ascii_message_t));
	queMinusculizar = xQueueCreate(16, sizeof(ascii_message_t));
	queMinusculizados = xQueueCreate(16, sizeof(ascii_message_t));
	queEnvioUART = xQueueCreate(16, sizeof(ascii_message_t));
}

static void uart_task_create(void)
{
	xTaskCreate(uart_task,                     // Funcion de la tarea a ejecutar
			(const char *) "tarea_uart", // Nombre de la tarea como String amigable para el usuario
			UART_TASK_STACK_SIZE, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			UART_TASK_PRIORITY,         // Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);
}

static void Manejador_paquete_create(void)
{
	xTaskCreate(
			Manejador_Paquete,                     // Funcion de la tarea a ejecutar
			(const char *)"Manejador_Paquete",     // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			tskIDLE_PRIORITY+1,         // Prioridad de la tarea
			0                           // Puntero a la tarea creada en el sistema
	);
}

void onRx( void *noUsado ) // Callback 1
{
	char c = uartRxRead( UART_USB );
    agregarCaracter(&paquete,&c);

   //printf( "Recibimos <<%c>> por UART\r\n", c );
}

void Manejador_Paquete( void* taskParmPtr ) // Callback 2
{
	// ---------- Movido a uart_task ------------------------------
	//uartConfig( UART_USB, 115200 );
	//uartRxInterruptCallbackSet( UART_USB, onRx );
	//uartRxInterruptSet( UART_USB, true );

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

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
