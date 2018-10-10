#ifndef TIEMPOLEDS_H_
#define TIEMPOLEDS_H_

#include "pulsadores.h"

typedef struct{
	uint32_t tiemposInicialPulsadores[MAX_PULSADORES];
	uint32_t tiemposFinalPulsadores[MAX_PULSADORES];
	bool_t pulsado[MAX_PULSADORES];
	}tiemposPulsadores_t;


void ManejadorEventosTiempoLeds  (Evento_t * evn);

void inicializarTiemposPulsadores();

tiemposPulsadores_t* getTiemposPulsadores();

#endif /* BROADCAST_H_ */
