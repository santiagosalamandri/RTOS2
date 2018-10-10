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
#include "FrameworkEventos.h"
#include "pulsadores.h"
#include "leds.h"
#include "tiempoLeds.h"

/*==================[definiciones y macros]==================================*/
//DEBUG_PRINT_ENABLE
#define CANTIDAD_PAQUETES 4
#define OVERHEAD CANTIDAD_PAQUETES*2
/*==================[definiciones de datos internos]=========================*/

uint8_t memoriaPoolChico[BLOQUE_POOL_CHICO * CANTIDAD_POOL_CHICO];				// Memoria a asignar para un pool chico
uint8_t memoriaPoolMediano[BLOQUE_POOL_MEDIANO * CANTIDAD_POOL_MEDIANO];		// Memoria a asignar para un pool mediano
uint8_t memoriaPoolGrande[BLOQUE_POOL_GRANDE * CANTIDAD_POOL_GRANDE];			// Memoria a asignar para un pool grande

/*==================[definiciones de datos externos]=========================*/

QMPool qmPoolChico, qmPoolMediano, qmPoolGrande;								// Pool de memoria para chico/mediano/grande

// Definicion de colas para enviar/recibir los datos
QueueHandle_t queMayusculizar;
QueueHandle_t queMinusculizar;
QueueHandle_t queTransmision;
QueueHandle_t queMedirPerformance;

header_t headerEnProceso;

Modulo_t * ModuloDriverPulsadores;
Modulo_t * ModuloBroadcast;
Modulo_t * ModuloDriverLeds;
Modulo_t * ModuloDriverTiempoLeds;

/*==================[declaraciones de funciones internas]====================*/

static void header_init(void);
static void pools_init(void);													// Inicializacion de pools
static void pool_free(uint8_t* buffer, QMPool* pool);							// Liberacion de memoria de pools
extern void heapReport(void);													// Reporte de memoria disponible en heap
static void queues_init(void);													// Inicializacion de colas
static void modulos_init(void);													// Inicializacion de modulos

static void uart_task_create(void);												// Creacion de tarea de utilizacion de UART
static void mayusculizador_task_create(void);									// Creacion de tarea mayusculizadora
static void minusculizador_task_create(void);									// Creacion de tarea minusculizadora
static void performance_task_create(void);
static void despachador_eventos_task_create(void);
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
	// Inicializaciones

	boardConfig(); 																// Inicializar y configurar la plataforma

	header_init();
	queues_init();																// Inicializacion de colas
	pools_init();																// Inicializacion de pools
	procesador_paquetes_init(); 												// Inicializacion de procesamiento de paquetes

	queEventosBaja= xQueueCreate(15, sizeof(Evento_t));

	// Creacion de tareas
	uart_task_create();															// Creacion de tarea de utilizacion de UART
	mayusculizador_task_create();												// Creacion de tarea mayusculizadora
	minusculizador_task_create();												// Creacion de tarea minusculizadora
	performance_task_create();
	despachador_eventos_task_create();
	heapReport();																// Reporte de memoria disponible en heap
	modulos_init();



	vTaskStartScheduler();														// Iniciar scheduler de tareas

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

static void header_init(void)
{
	headerEnProceso.stx = 0;
	headerEnProceso.op = 0;
	headerEnProceso.tam = 0;
	headerEnProceso.id_de_paquete = 0;
	headerEnProceso.tiempo_de_llegada = 0;
	headerEnProceso.tiempo_de_recepcion = 0;
	headerEnProceso.tiempo_de_inicio = 0;
	headerEnProceso.tiempo_de_fin = 0;
	headerEnProceso.tiempo_de_salida = 0;
	headerEnProceso.tiempo_de_transmision = 0;
	headerEnProceso.largo_del_paquete = 0;
	headerEnProceso.memoria_alojada = 0;
}

static void pools_init(void)
{
	// Inicializacion de pools de memoria segun la capacidad requerida
	QMPool_init(&qmPoolChico, memoriaPoolChico, sizeof(memoriaPoolChico), BLOQUE_POOL_CHICO);
	QMPool_init(&qmPoolMediano, memoriaPoolMediano, sizeof(memoriaPoolMediano), BLOQUE_POOL_MEDIANO);
	QMPool_init(&qmPoolGrande, memoriaPoolGrande, sizeof(memoriaPoolGrande), BLOQUE_POOL_GRANDE);
}

static void queues_init(void)
{
	// Inicializacion de colas a utilizar
	queMayusculizar = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
	queMinusculizar = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
	queTransmision = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
	queMedirPerformance = xQueueCreate(16, sizeof(mensaje_entre_tareas_t));
}

static void pool_free(uint8_t* buffer, QMPool* pool)
{
	// Liberar la memoria del pool indicado
	QMPool_put(pool, buffer);
}

static void uart_task_create(void)
{
	xTaskCreate(uart_task,                    								// Funcion de la tarea a ejecutar
			(const char *) "tarea_uart", 									// Nombre de la tarea como String amigable para el usuario
			UART_TASK_STACK_SIZE, 											// Cantidad de stack de la tarea
			pool_free,                       								// Parametros de tarea
			UART_TASK_PRIORITY,         									// Prioridad de la tarea
			0                       										// Puntero a la tarea creada en el sistema
	);
}

static void mayusculizador_task_create(void)
{
	xTaskCreate(mayusculizador_task,                    					// Funcion de la tarea a ejecutar
			(const char *) "tarea_mayusculizador", 							// Nombre de la tarea como String amigable para el usuario
			MAYUSCULIZADOR_TASK_STACK_SIZE, 								// Cantidad de stack de la tarea
			0,                          									// Parametros de tarea
			MAYUSCULIZADOR_TASK_PRIORITY,        							// Prioridad de la tarea
			0                         										// Puntero a la tarea creada en el sistema
	);
}

static void minusculizador_task_create(void)
{
	xTaskCreate(minusculizador_task,                     					// Funcion de la tarea a ejecutar
			(const char *) "tarea_minusculizador", 							// Nombre de la tarea como String amigable para el usuario
			MINUSCULIZADOR_TASK_STACK_SIZE, 								// Cantidad de stack de la tarea
			0,                          									// Parametros de tarea
			MINUSCULIZADOR_TASK_PRIORITY,         							// Prioridad de la tarea
			0                         										// Puntero a la tarea creada en el sistema
	);
}

static void performance_task_create(void)
{
	xTaskCreate(performance_task,                    					// Funcion de la tarea a ejecutar
			(const char *) "tarea_performance", 							// Nombre de la tarea como String amigable para el usuario
			PERFORMANCE_TASK_STACK_SIZE, 								// Cantidad de stack de la tarea
			0,                          									// Parametros de tarea
			PERFORMANCE_TASK_PRIORITY,        							// Prioridad de la tarea
			0                         										// Puntero a la tarea creada en el sistema
	);
}

static void despachador_eventos_task_create(void){

	xTaskCreate(
				taskDespacharEventos,
					"Control",
					5 * configMINIMAL_STACK_SIZE,
					(void*) queEventosBaja,
					PERFORMANCE_TASK_PRIORITY,
					NULL );

}


static void modulos_init(void){

	ModuloBroadcast			= RegistrarModulo(ManejadorEventosBroadcast, 			PRIORIDAD_BAJA);
	ModuloDriverPulsadores	= RegistrarModulo(DriverPulsadores, 					PRIORIDAD_BAJA);
	ModuloDriverLeds	= RegistrarModulo(ManejadorEventosLeds, 					PRIORIDAD_BAJA);
	ModuloDriverTiempoLeds	= RegistrarModulo(ManejadorEventosTiempoLeds,			PRIORIDAD_BAJA);

	IniciarTodosLosModulos();
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
