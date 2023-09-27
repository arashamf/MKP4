#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "MDR32FxQI_config.h"
#include "MDR32FxQI_iwdg.h"
#include "MDR32FxQI_mil_std_1553.h"
#include "MDR32FxQI_timer.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_uart.h"
#include "MDR32FxQI_eeprom.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define HSE_ON_ATTEMPTS				15
#define __USE_DBG
//#define __USE_IWDG
 #define USE_MDR1986VE1T  
 
#endif 
