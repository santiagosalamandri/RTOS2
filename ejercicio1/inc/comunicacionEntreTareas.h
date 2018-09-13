#pragma once

#define QUEQUE_NO_WAIT	0


typedef struct
{
	uint32_t length;
	uint8_t* dataPtr;
} ascii_message_t;


extern QueueHandle_t queMayusculizar;
extern QueueHandle_t queMayusculizados;

extern QueueHandle_t queMinusculizar;
extern QueueHandle_t queMinusculizados;


extern QueueHandle_t queEnvioUART;
