/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "HW_Profile.h"
#include "typedef.h"
#include "pins.h"
#include "XTick.h"
#include "uart_func.h"
#include "MIL1553B_RT.h"
#include "MDR1986VE1T_IT.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PORT_InitTypeDef port_init_struct;
TMyDeviceInfo g_MyDeviceInfo = { 0, 0 }; // информация о приборе для передачи в ЦХП 

/* Private function prototypes -----------------------------------------------*/
static bool ClockConfigure (void);
static void InitWatchDog(void);
//static void Task_Control_LEDs( void );


/* Private functions ---------------------------------------------------------*/
#ifdef __USE_DBG
	#define ITM_STIM_U32 (*(volatile unsigned int*)0xE0000000)    // Stimulus Port Register word acces
	#define ITM_STIM_U8  (*(volatile         char*)0xE0000000)    // Stimulus Port Register byte acces
	#define ITM_ENA      (*(volatile unsigned int*)0xE0000E00)    // Trace Enable Ports Register
	#define ITM_TCR      (*(volatile unsigned int*)0xE0000E80)    // Trace control register

int fputc( int c, FILE *f ) 
{
	if( (ITM_TCR & 1) && (ITM_ENA & 1) ) 	// Check if ITM_TCR.ITMENA is set, Check if stimulus port is enabled
	{ 		
		while ((ITM_STIM_U8 & 1) == 0) {}; //Wait until STIMx is ready,
		ITM_STIM_U8 = (char)c; // then send data
	}
	return( c );
}
#endif

//-----------------------------------------------------------------------------
int main( void )
//-----------------------------------------------------------------------------
{
	SystemInit();
	ClockConfigure();
	InitXTick( );
	InitBiLED(&m_Led );	
	LED_GREEN(ON);
	
	DBG_LoLevel_Init(DBG_UART, DBG_UART_CLOCK , DBG_BAUD_RATE);
	
	//Get_Module_Address();
	Pins_CheckVoltage_Init();
	
	InitMIL1553B_RT();
	#ifdef __USE_IWDG
		InitWatchDog(); //инициализация сторожевого таймера
	#endif
	
while(1)
	{
	
		TOOGLE_LED_GREEN();
		Check_Voltage();
		#ifdef __USE_DBG
			sprintf (DBG_buffer, "status=%x\r\n",g_MyDeviceInfo.DeviceState);
			DBG_PutString (DBG_buffer);	
			DelayMs(500);
		#endif	
		
		#ifdef __USE_IWDG	
			IWDG_ReloadCounter(); //перезагрузка сторожевого таймера
		#endif

	}
}

//-----------------------------------------------------------------------------
bool ClockConfigure ( void )
{
  uint32_t cntr = 0;

	cntr = 0;
	RST_CLK_HSEconfig(RST_CLK_HSE_ON); //switch on HSE clock generator
  while(RST_CLK_HSEstatus() != SUCCESS && cntr++ < 0x40000) {};//ожидание готовности генератора HSE

  if(RST_CLK_HSEstatus() != SUCCESS) //получение статуса генератора HSE
		return false;
	
	#define PLL_MULL_VALUE (CPU_CLOCK_VALUE / HSE_Value - 1) //96МГц/8=12МГц
	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSEdiv1, PLL_MULL_VALUE ); //Select HSE clock as CPU_PLL input clock source & set PLL multiplier

	RST_CLK_CPU_PLLcmd(ENABLE); //enable CPU_PLL
	
	while(RST_CLK_CPU_PLLstatus() != SUCCESS && cntr++ < 0x40000) {};//ожидание готовности CPU_PLL 

  if(RST_CLK_CPU_PLLstatus() != SUCCESS) //получение статуса CPU_PLL 
		return false;

	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE); //enable the RST_CLK_PCLK_EEPROM 

	// Set the code latency value
	#if CPU_CLOCK_VALUE < 25000000UL					// Freqency < 25MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_0
	#elif CPU_CLOCK_VALUE < 50000000UL					// 25MHz <= Freqency < 50MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_1				
	#elif CPU_CLOCK_VALUE < 75000000UL					// 50MHz <= Freqency < 75MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_2				
	#elif CPU_CLOCK_VALUE < 100000000UL					// 75MHz <= Freqency < 100MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_3				
	#elif CPU_CLOCK_VALUE < 125000000UL					// 100MHz <= Freqency < 125MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_4				
	#elif CPU_CLOCK_VALUE < 150000000UL					// 125MHz <= Freqency < 150MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_5				
	#else												// 150MHz <= Freqency
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_7
	#endif
	
	EEPROM_SetLatency(EEPROM_LATENCY_VALUE); //sets the code latency value

	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1); //set CPU_C3_prescaler to 1

	RST_CLK_CPU_PLLuse(ENABLE); 	//set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock
	
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3); //select CPU_C3 clock on the CPU clock MUX 

	SystemCoreClockUpdate(); //get core clock frequency  
	
	return true;
}

//-----------------------------------------------------------------------------------------------------//
#ifdef __USE_IWDG
void InitWatchDog( void )
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_IWDG,ENABLE);
	IWDG_WriteAccessEnable();
	IWDG_SetPrescaler(IWDG_Prescaler_64);	// 40000/64=625 Гц
	while( IWDG_GetFlagStatus( IWDG_FLAG_PVU ) != 1 ){}
	IWDG_SetReload (2500);	// 2500 / 652 = 4 сек
	IWDG_Enable();
	IWDG_ReloadCounter();
}
#endif

//------------------------------------------------------------------------------------------------//
/*void Task_Control_LEDs( void )
{

}*/

