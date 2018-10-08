#ifndef comunicacion_tareas_h
#define comunicacion_tareas_h

#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "qmpool.h"
#include "procesador_paquetes.h"

#define QUEQUE_NO_WAIT	0

#define BLOQUE_POOL_CHICO				(64 + HEADER_TAIL_LENGTH)
#define BLOQUE_POOL_MEDIANO				(128 + HEADER_TAIL_LENGTH)
#define BLOQUE_POOL_GRANDE				(256 + HEADER_TAIL_LENGTH)

#define CANTIDAD_POOL_CHICO				64
#define CANTIDAD_POOL_MEDIANO			32
#define CANTIDAD_POOL_GRANDE			16


extern QueueHandle_t queProcesadorPaquetes;

extern QueueHandle_t queMayusculizar;
extern QueueHandle_t queMinusculizar;
extern QueueHandle_t queTransmision;
extern QueueHandle_t queMedirPerformance;

typedef struct {
  uint8_t* buffer;
  uint32_t length;
  QMPool* pool;
} mensaje_entre_tareas_t;


extern QMPool qmPoolChico, qmPoolMediano, qmPoolGrande;

#endif
