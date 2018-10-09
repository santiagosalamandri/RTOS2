#include "FrameworkEventos.h"
#include "leds.h"


const gpioMap_t gpioLeds[]={LEDB,LED1,LED2,LED3};

void ManejadorEventosLeds (Evento_t * evn){

		switch(evn->signal){

		case SIG_PULSADOR_APRETADO:
			gpioWrite(gpioLeds[evn->valor],ENCENDER);
			break;

		case SIG_PULSADOR_LIBERADO:
			gpioWrite(gpioLeds[evn->valor],APAGAR);
			break;

		default:
			break;
		}

}
