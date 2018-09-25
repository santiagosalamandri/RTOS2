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
//DEBUG_PRINT_ENABLE
#define CANTIDAD_PAQUETES 4
#define OVERHEAD CANTIDAD_PAQUETES*2

/*==================[definiciones de datos internos]=========================*/


static uint8_t memoriaPoolChico[BLOQUE_POOL_CHICO * CANTIDAD_POOL_CHICO];
static uint8_t memoriaPoolMediano[BLOQUE_POOL_MEDIANO * CANTIDAD_POOL_MEDIANO];
static uint8_t memoriaPoolGrande[BLOQUE_POOL_GRANDE * CANTIDAD_POOL_GRANDE];

/*==================[definiciones de datos externos]=========================*/

QMPool qmPoolChico, qmPoolMediano, qmPoolGrande;

QueueHandle_t queMayusculizar;
QueueHandle_t queMayusculizados;
QueueHandle_t queMinusculizar;
QueueHandle_t queMinusculizados;
QueueHandle_t queEnvioUART;

/*==================[declaraciones de funciones internas]====================*/

static void pools_init(void);
static void queues_init(void);
static void uart_task_create(void);
static void mayusculizador_task_create(void);
static void minusculizador_task_create(void);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
	// ---------- CONFIGURACIONES ------------------------------

	boardConfig(); 	// Inicializar y configurar la plataforma

	queues_init();

	pools_init();

	proceso_init();

	gpioWrite(LEDG, ON); 	// Led indicador programa corriendo	

	uart_task_create();		// Crear tarea UART

	mayusculizador_task_create();

	minusculizador_task_create();

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

static void pools_init(void)
{
	QMPool_init(&qmPoolChico, memoriaPoolChico, sizeof(memoriaPoolChico), BLOQUE_POOL_CHICO);
	QMPool_init(&qmPoolMediano, memoriaPoolMediano, sizeof(memoriaPoolMediano), BLOQUE_POOL_MEDIANO);
	QMPool_init(&qmPoolGrande, memoriaPoolGrande, sizeof(memoriaPoolGrande), BLOQUE_POOL_GRANDE);
}

static void queues_init(void)
{
	queMayusculizar = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
	queMayusculizados = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
	queMinusculizar = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
	queMinusculizados = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
	queEnvioUART = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
}

static void uart_task_create(void)
{
	xTaskCreate(uart_task,                     // Funcion de la tarea a ejecutar
			(const char *) "tarea_uart", // Nombre de la tarea como String amigable para el usuario
			UART_TASK_STACK_SIZE, // Cantidad de stack de la tarea
			procesarByteRecibido,                          // Parametros de tarea
			UART_TASK_PRIORITY,         // Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);
}

static void mayusculizador_task_create(void)
{
	xTaskCreate(mayusculizador_task,                     // Funcion de la tarea a ejecutar
			(const char *) "tarea_mayusculizador", // Nombre de la tarea como String amigable para el usuario
			MAYUSCULIZADOR_TASK_STACK_SIZE, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			MAYUSCULIZADOR_TASK_PRIORITY,         // Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);
}

static void minusculizador_task_create(void)
{
	xTaskCreate(minusculizador_task,                     // Funcion de la tarea a ejecutar
			(const char *) "tarea_minusculizador", // Nombre de la tarea como String amigable para el usuario
			MINUSCULIZADOR_TASK_STACK_SIZE, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			MINUSCULIZADOR_TASK_PRIORITY,         // Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
