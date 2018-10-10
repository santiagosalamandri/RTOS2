/*
 * broadcast.c
 *
 *  Created on: 29/11/2011
 *      Author: alejandro
 */

#include "FrameworkEventos.h"

typedef enum estadosBroadcastEnum {
	sBROADCAST_IDLE		 = 0,
	sBROADCAST_NORMAL
} EstadosBroadcast_t;

static int estadoBroadcast = sBROADCAST_IDLE;


void ManejadorEventosBroadcast (Evento_t * evn){
	char buf[100+1];
	switch(estadoBroadcast){

//-----------------------------------------------------------------------------
	case sBROADCAST_IDLE:
		switch(evn->signal){
		case SIG_MODULO_INICIAR:
			estadoBroadcast = sBROADCAST_NORMAL;
			//printf("SIG_MODULO_INICIAR\n");
			break;

		default:
			break;
		}
		break;
//-----------------------------------------------------------------------------
		case sBROADCAST_NORMAL:
		switch(evn->signal){

		case SIG_PULSADOR_APRETADO:
			//sprintf			(buf,"%s %d\r\n",pulsadorApretado, evn->valor);
			//printf("pulsador %d apretado\n",evn->valor);
			//gpioWrite(gpioLeds[evn->valor],1);
			//UARTputs		(&consola, buf, UART_PUTS_INTERRUPCIONES);
			ReenviarEvento	(ModuloDriverLeds, evn);
			ReenviarEvento	(ModuloDriverTiempoLeds, evn);
			break;

		case SIG_PULSADOR_LIBERADO:
			//sprintf			(buf,"%s %d\r\n",pulsadorLiberado, evn->valor);
			//printf("pulsador %d liberado\n",evn->valor);
			//gpioWrite(gpioLeds[evn->valor],0);
			//UARTputs		(&consola, buf, UART_PUTS_INTERRUPCIONES);
			ReenviarEvento	(ModuloDriverLeds, evn);
			ReenviarEvento	(ModuloDriverTiempoLeds, evn);

			break;

		default:
			break;
		}
		break;
//-----------------------------------------------------------------------------
	default:
		break;
	}
}
