#include "FrameworkEventos.h"
#include "tiempoLeds.h"
#include "pulsadores.h"

#include "task.h"

struct{
uint32_t tiemposInicialPulsadores[MAX_PULSADORES];
uint32_t tiemposFinalPulsadores[MAX_PULSADORES];
bool_t pulsado[MAX_PULSADORES];
}tiemposPulsadores_t;

static void inicializarTiemposPulsadores();
static void tiemposToString(int pulsador);

void ManejadorEventosTiempoLeds (Evento_t * evn){

		switch(evn->signal){

		case SIG_MODULO_INICIAR:
			inicializarTiemposPulsadores();
			break;

		case SIG_PULSADOR_APRETADO:
			tiemposPulsadores_t.pulsado[evn->valor]=true;
			tiemposPulsadores_t.tiemposInicialPulsadores[evn->valor]=xTaskGetTickCount();
			break;

		case SIG_PULSADOR_LIBERADO:
			tiemposPulsadores_t.pulsado[evn->valor]=false;
			tiemposPulsadores_t.tiemposFinalPulsadores[evn->valor]=xTaskGetTickCount();
			tiemposToString(evn->valor);
			break;

		default:
			break;
		}

}


static void inicializarTiemposPulsadores(){
	int i=0;
	for (i = 0; i < MAX_PULSADORES; ++i) {
		tiemposPulsadores_t.tiemposInicialPulsadores[i]=0;
		tiemposPulsadores_t.tiemposFinalPulsadores[i]=0;
		tiemposPulsadores_t.pulsado[i]=false;
	}


}

static void tiemposToString(int pulsador){
	printf("Tiempo de pulsacion la tecla %d: %ld mS.\n ",pulsador+1,tiemposPulsadores_t.tiemposFinalPulsadores[pulsador]-tiemposPulsadores_t.tiemposInicialPulsadores[pulsador]);

}
