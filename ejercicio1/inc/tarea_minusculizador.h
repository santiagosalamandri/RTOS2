#ifndef minusculizador_h
#define minusculizador_h

#include "comunicacionEntreTareas.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA	32

void minusculizador_task(void* taskParmPtr);

#endif
