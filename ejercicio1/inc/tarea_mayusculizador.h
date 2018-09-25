#ifndef mayusculizador_h
#define mayusculizador_h

#include "comunicacionEntreTareas.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA	32

void mayusculizador_task(void* taskParmPtr);

#endif
