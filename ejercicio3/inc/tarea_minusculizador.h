#ifndef minusculizador_h
#define minusculizador_h

#include "comunicacionEntreTareas.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define LETRA_A 65															// Valor ASCII de A
#define LETRA_Z 90															// Valor ASCII de Z
#define DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA 32								// Diferencia ASCII entre mayus y minus

void minusculizador_task(void* taskParmPtr);

#endif
