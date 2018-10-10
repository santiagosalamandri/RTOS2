#include "pulsadores.h"
#include "leds.h"
#include "tiempoLeds.h"


typedef enum EstadosPulsadorEnum
{
	sPULSADOR_IDLE = 0,
	sPULSADOR_ESPERANDO_PULSACION,
	sPULSADOR_CONFIRMANDO_PULSACION,
	sPULSADOR_ESPERANDO_LIBERACION,
	sPULSADOR_CONFIRMANDO_LIBERACION
} EstadosPulsadorEnum_t;

typedef struct PulsadorStruct
{
	gpioMap_t  boton;
	int estadoFSM;
	int ticksFiltrados;
	int idPulsador;
} Pulsador;

void FSM_Pulsador ( Pulsador * pulsador, Evento_t * evn );

Pulsador vectorPulsadores[MAX_PULSADORES];
static int nPulsadores = 0;


gpioMap_t pulsador1,pulsador2,pulsador3,pulsador4;

static Modulo_t * mod;

static bool_t isActivo(gpioMap_t button);

void PulsadorInit ( Pulsador * p, gpioMap_t  Inicializado )
{
	p->boton 			= Inicializado;
	p->estadoFSM 		= sPULSADOR_ESPERANDO_PULSACION;
	p->ticksFiltrados 	= 0;
	p->idPulsador 		= nPulsadores;
	nPulsadores++;
}

void DriverPulsadoresInit ( Modulo_t * pModulo )
{


	PulsadorInit( &vectorPulsadores[0], TEC1 );

	PulsadorInit( &vectorPulsadores[1], TEC2 );

	PulsadorInit( &vectorPulsadores[2], TEC3 );

	PulsadorInit( &vectorPulsadores[3], TEC4 );

	pModulo->periodo = 10;

}

void DriverPulsadores ( Evento_t *evn )
{
	//printf("DriverPulsadores\n");
	int i;
	switch( evn->signal )
	{
		case SIG_MODULO_INICIAR:
			//mod = (Modulo_t *)evn->valor;
			mod = (Modulo_t *)evn->receptor;
			DriverPulsadoresInit(mod);
			timerArmarRepetitivo(mod, mod->periodo);
			//printf("SIG_MODULO_INICIAR\n");
			break;

		case SIG_TIMEOUT:
			//printf("SIG_TIMEOUT\n");

			for ( i = 0; i < nPulsadores ; i++ )
			{
				FSM_Pulsador( &vectorPulsadores[i], evn );
			}
			break;

		default:	// Ignoro todas las otras seniales
			break;
	}
}

void FSM_Pulsador ( Pulsador * pulsador, Evento_t * evn )
{
	Evento_t _evn;
	if( pulsador->ticksFiltrados > 0 )
	{
		pulsador->ticksFiltrados--;
		return;
	}

	switch ( pulsador->estadoFSM )
	{
		//-----------------------------------------------------------------------------
		case sPULSADOR_ESPERANDO_PULSACION:
			if( isActivo( pulsador->boton ) )
			{
				pulsador->estadoFSM 		= sPULSADOR_CONFIRMANDO_PULSACION;
				pulsador->ticksFiltrados 	= 0;
			}
			else
			{
				pulsador->ticksFiltrados 	= 5;
			}
			break;
			//-----------------------------------------------------------------------------
		case sPULSADOR_CONFIRMANDO_PULSACION:
			if( isActivo( pulsador->boton ) )
			{
				pulsador->estadoFSM 		= sPULSADOR_ESPERANDO_LIBERACION;
				pulsador->ticksFiltrados 	= 5;
				EncolarEvento(ModuloBroadcast, SIG_PULSADOR_APRETADO, pulsador->idPulsador);
				//printf("pulsador %d apretado\n",pulsador->idPulsador);
			}
			else
			{
				pulsador->estadoFSM 		= sPULSADOR_ESPERANDO_PULSACION;
				pulsador->ticksFiltrados 	= 0;
			}
			break;
			//-----------------------------------------------------------------------------
		case sPULSADOR_ESPERANDO_LIBERACION:
			if( !isActivo( pulsador->boton ) )
			{
				pulsador->estadoFSM 		= sPULSADOR_CONFIRMANDO_LIBERACION;
				pulsador->ticksFiltrados 	= 0;
			}
			else
			{
				pulsador->ticksFiltrados 	= 5;
			}
			break;
			//-----------------------------------------------------------------------------
		case sPULSADOR_CONFIRMANDO_LIBERACION:
			if( !isActivo( pulsador->boton ) )
			{
				//Liberacion confirmada, encolo un evento
				pulsador->estadoFSM 		= sPULSADOR_ESPERANDO_PULSACION;
				pulsador->ticksFiltrados 	= 5;
				EncolarEvento(ModuloBroadcast, SIG_PULSADOR_LIBERADO, pulsador->idPulsador);
				//printf("pulsador %d liberado\n",pulsador->idPulsador);

			}
			break;
			//-----------------------------------------------------------------------------
		default:
			// Se me fue de valor la variable de estado!
			// TODO: Reinicializar ESTE pulsador
			break;
	}
}


static bool_t isActivo(gpioMap_t button){
	return !gpioRead( button );
}
