#ifndef UART_FUNC_H
#define UART_FUNC_H

#include "main.h"
//#include "typedef.h"
//#include "ring_buffer.h"

//Private prototypes--------------------------------------------------------------//
void DBG_LoLevel_Init(MDR_UART_TypeDef* , uint32_t , uint32_t );
void DBG_PutString (char *);
uint8_t UART_CharReception_Callback (void);

extern char DBG_buffer[];
#endif
