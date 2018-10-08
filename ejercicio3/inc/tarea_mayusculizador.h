#ifndef mayusculizador_h
#define mayusculizador_h

#include "comunicacionEntreTareas.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define LETRA_a 97															// Valor ASCII de a
#define LETRA_z 122															// Valor ASCII de z
#define DIFERENCIA_ASCII_MINUSCULA_MAYUSCULA 32								// Diferencia ASCII entre mayus y minus

void mayusculizador_task(void* taskParmPtr);
static void mayusculizar(mensaje_entre_tareas_t* mensajeEntreTareas);

#endif
