#include "FrameworkEventos.h"
#include "tiempoLeds.h"

#include "task.h"

tiemposPulsadores_t tiempoPulsadores;

static void tiemposToString(int pulsador);

tiemposPulsadores_t* getTiemposPulsadores(){
return &tiempoPulsadores;
}

void ManejadorEventosTiempoLeds (Evento_t * evn){

		switch(evn->signal){

		case SIG_MODULO_INICIAR:
			inicializarTiemposPulsadores();
			break;

		case SIG_PULSADOR_APRETADO:
			tiempoPulsadores.pulsado[evn->valor]=true;
			tiempoPulsadores.tiemposInicialPulsadores[evn->valor]=xTaskGetTickCount()/ portTICK_RATE_MS;
			break;

		case SIG_PULSADOR_LIBERADO:
			tiempoPulsadores.pulsado[evn->valor]=false;
			tiempoPulsadores.tiemposFinalPulsadores[evn->valor]=xTaskGetTickCount()/ portTICK_RATE_MS;
			//tiemposToString(evn->valor);
			break;

		default:
			break;
		}

}


void inicializarTiemposPulsadores(){
	int i=0;
	for (i = 0; i < MAX_PULSADORES; ++i) {
		tiempoPulsadores.tiemposInicialPulsadores[i]=0;
		tiempoPulsadores.tiemposFinalPulsadores[i]=0;
		tiempoPulsadores.pulsado[i]=false;
	}


}

static void tiemposToString(int pulsador){
	printf("Tiempo de pulsacion la tecla %d: %ld mS.\n ",pulsador+1,tiempoPulsadores.tiemposFinalPulsadores[pulsador]-tiempoPulsadores.tiemposInicialPulsadores[pulsador]);
}


