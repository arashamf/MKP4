#ifndef _HW_Profile_H_
#define _HW_Profile_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define CPU_CLOCK_VALUE	(96000000UL)	/* Частота контроллера */
//#define TICKS_PER_SECOND			1000 

//-----------------------------------------------------

#define LED_GREEN_PIN							PORT_Pin_1
#define LED_GREEN_PORT						MDR_PORTB

#define LED_PIN1								PORT_Pin_0
#define LED_LED_PORT						MDR_PORTB

#define LED_PIN2								PORT_Pin_2
#define LED_LED_PORT						MDR_PORTB
//UART для отладки--------------------------------------

#define DBG_UART								MDR_UART1
#define DBG_UART_CLOCK 					RST_CLK_PCLK_UART1
	
#define DBG_BAUD_RATE						115200
	
#define DBG_CLOCK_Pin_TX 				RST_CLK_PCLK_PORTC
	
#define DBG_PORT_TX							MDR_PORTC
#define DBG_PinTX								PORT_Pin_3
#define DBG_PORT_FuncTX  				PORT_FUNC_MAIN 	

#define DBG_PORT_RX							MDR_PORTC
#define DBG_PinRX								PORT_Pin_4
#define DBG_PORT_FuncRX  				PORT_FUNC_MAIN 	

// Адрес модуля в кроссе----------------------------------
#define BACKPLANE_PIN_CLOCK 		RST_CLK_PCLK_PORTB

#define BACKPLANE_ADDR0_PIN			PORT_Pin_11
#define BACKPLANE_ADDR0_PORT		MDR_PORTB
                              
#define BACKPLANE_ADDR1_PIN			PORT_Pin_12       
#define BACKPLANE_ADDR1_PORT		MDR_PORTB             
                              
#define BACKPLANE_ADDR2_PIN			PORT_Pin_13        
#define BACKPLANE_ADDR2_PORT		MDR_PORTB             
                             
#define BACKPLANE_ADDR3_PIN			PORT_Pin_14        
#define BACKPLANE_ADDR3_PORT		MDR_PORTB             
                                        
#define BACKPLANE_ADDR4_PIN			PORT_Pin_15        
#define BACKPLANE_ADDR4_PORT		MDR_PORTB   
          
//---------------------------ЛПИ 1
#define MDR_MIL_STD_15531_PRMp_PIN						PORT_Pin_13				// PRMA+
#define MDR_MIL_STD_15531_PRMp_PORT						MDR_PORTC
#define MDR_MIL_STD_15531_PRMp_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15531_PRMm_PIN						PORT_Pin_14				// PRMA-
#define MDR_MIL_STD_15531_PRMm_PORT						MDR_PORTC
#define MDR_MIL_STD_15531_PRMm_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15531_PRDp_PIN						PORT_Pin_1				// PRDA+
#define MDR_MIL_STD_15531_PRDp_PORT						MDR_PORTD
#define MDR_MIL_STD_15531_PRDp_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15531_PRDm_PIN						PORT_Pin_2				// PRDA-
#define MDR_MIL_STD_15531_PRDm_PORT						MDR_PORTD
#define MDR_MIL_STD_15531_PRDm_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15531_PRD_PRM_PIN					PORT_Pin_5				// PRD_PRMA
#define MDR_MIL_STD_15531_PRD_PRM_PORT				MDR_PORTD
#define MDR_MIL_STD_15531_PRD_PRM_PORT_FUNC		PORT_FUNC_MAIN

//---------------------------ЛПИ 2
#define MDR_MIL_STD_15532_PRMp_PIN						PORT_Pin_3				// PRMС+
#define MDR_MIL_STD_15532_PRMp_PORT						MDR_PORTF
#define MDR_MIL_STD_15532_PRMp_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15532_PRMm_PIN						PORT_Pin_4				// PRMС-
#define MDR_MIL_STD_15532_PRMm_PORT						MDR_PORTF
#define MDR_MIL_STD_15532_PRMm_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15532_PRDp_PIN						PORT_Pin_7				// PRDС+
#define MDR_MIL_STD_15532_PRDp_PORT						MDR_PORTF
#define MDR_MIL_STD_15532_PRDp_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15532_PRDm_PIN						PORT_Pin_8				// PRDС-
#define MDR_MIL_STD_15532_PRDm_PORT						MDR_PORTF
#define MDR_MIL_STD_15532_PRDm_PORT_FUNC			PORT_FUNC_MAIN

#define MDR_MIL_STD_15532_PRD_PRM_PIN					PORT_Pin_11				// PRD_PRMС
#define MDR_MIL_STD_15532_PRD_PRM_PORT				MDR_PORTF
#define MDR_MIL_STD_15532_PRD_PRM_PORT_FUNC		PORT_FUNC_MAIN

//--------------------------------------
#define CHECK_VOLTAGE_PIN_CLOCK 			RST_CLK_PCLK_PORTE

#define PIN_CHANNEL1_27V 			PORT_Pin_10
#define CHANNEL1_27V_PORT			MDR_PORTE

#define PIN_CHANNEL2_27V 			PORT_Pin_9
#define CHANNEL2_27V_PORT			MDR_PORTE

#define PIN_CHANNEL3_27V 			PORT_Pin_8
#define CHANNEL3_27V_PORT			MDR_PORTE

#define PIN_CHANNEL4_27V 			PORT_Pin_5
#define CHANNEL4_27V_PORT			MDR_PORTE

#define PIN_CHANNEL1_220V 		PORT_Pin_7
#define CHANNEL1_220V_PORT		MDR_PORTE

#define PIN_CHANNEL2_220V 		PORT_Pin_6
#define CHANNEL2_220V_PORT		MDR_PORTE
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//-------------------------------------------------------------------------------------------------
	
#endif
