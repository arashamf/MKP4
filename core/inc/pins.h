
#ifndef __PINS_H__
#define __PINS_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ------------------------------------------------------------------------//
#include "main.h"
#include "HW_Profile.h"
#include "uart_func.h"

// Private defines------------------------------------------------------------------//


#define ON 1
#define OFF 0

//Exported types -----------------------------------------------------------------//
typedef enum { LED_BLACK = 0, LED_RED, LED_GREEN, LED_YELLOW }TBiLEDColor;

//----------------------------------------------------------------------------
typedef struct 
{
	MDR_PORT_TypeDef *PORTx;
	uint16_t PORT_Pin;		

}TPortPin;

//----------------------------------------------------------------------------
typedef struct 
{		
	TPortPin 	Green;
}TBiLED;

static TBiLED m_Led = {{LED_GREEN_PORT, LED_GREEN_PIN}}; 

//Prototypes------------------------------------------------------------------------//
int8_t Get_Module_Address( void );
void Pins_CheckVoltage_Init(void);
uint8_t Check_Voltage(void);
void InitBiLED( const TBiLED *pBiLed );

//Macro---------------------------------------------------------------------------//
#define LED_GREEN(x) ((x) ? (PORT_SetBits (LED_GREEN_PORT, LED_GREEN_PIN)) : (PORT_ResetBits(LED_GREEN_PORT, LED_GREEN_PIN)))
#define TOOGLE_LED_GREEN() (PORT_ReadInputDataBit (LED_GREEN_PORT, LED_GREEN_PIN) ? (LED_GREEN(OFF)) : (LED_GREEN(ON)))
#define LED1(x) ((x)? (PORT_SetBits (LED_RED_PORT, LED_PIN1)) : (PORT_ResetBits(LED_RED_PORT, LED_PIN1)))
#define LED2(x) ((x)? (PORT_SetBits (LED_RED_PORT, LED_PIN2)) : (PORT_ResetBits(LED_RED_PORT, LED_PIN2)))
#ifdef __cplusplus
}
#endif
#endif /*__ PINS_H__ */

