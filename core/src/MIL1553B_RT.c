/*
 Модуль MIL1553B_RT.c предназначен для:
  - приема сообщений с хронометрической информацией от ЦХП по ЛПИ-П;
  - ответа на запросы ЦХП о состоянии прибора (код состояния принимается из МИУ3, МЛК, МФК и проч., см. CAN.c);
  - выдачи метки времени на кросс прибора (в тексте программы - МВ), совпадающей с секундной меткой ЦХП;
  - передачи оцифровки МВ (сообщение типа A1);
  - мониторинга наличия и достоверности хронометрической информации от ЦХП.

*/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "pins.h"
#include "uart_func.h"
#include "HW_Profile.h"
#include "typedef.h"
#include "MIL1553B_RT.h"
#include "XTick.h"
#include "xpt.h"

#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
// #define DEBUG_TIMEMARK_WITHOUT_MIL

#if (MY_RT_MDR_MIL_STD_1553_BASE == MDR_MIL_STD_15531_BASE)
	#define MY_RT_MDR_MIL_STD_1553				MDR_MIL_STD_15531
	#define MY_RT_MIL_STD_1553_IRQn				MIL_STD_1553B1_IRQn	
	#define MY_RT_MIL_STD_1553_IRQ_Handler()	void MIL_STD_1553B1_IRQHandler( void )
#elif (MY_RT_MDR_MIL_STD_1553_BASE == MDR_MIL_STD_15532_BASE)
	#define MY_RT_MDR_MIL_STD_1553				MDR_MIL_STD_15532
	#define MY_RT_MIL_STD_1553_IRQn				MIL_STD_1553B2_IRQn	
	#define MY_RT_MIL_STD_1553_IRQ_Handler()	void MIL_STD_1553B2_IRQHandler( void )
#else
	#error Unknown MY_RT_MDR_MIL_STD_1553_BASE!
#endif

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
int16_t g_MyRTAddress = 0x0;
	
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
	
//-----------------------------------------------------------------------------//
static void InitRxMIL1553B( void )
{
  PORT_InitTypeDef sPort;
  MIL_STD_1553_InitTypeDef sMIL1553;

	RST_CLK_PCLKcmd( PCLK_BIT(MY_RT_MDR_MIL_STD_1553) , ENABLE); 	// Enable peripheral clocks


	// MIL-STD-1553B Ports Init. определения констант см. в HW_Profile.h
	PORT_StructInit( &sPort );
	sPort.PORT_SPEED = PORT_SPEED_MAXFAST;
	sPort.PORT_MODE = PORT_MODE_DIGITAL;

	sPort.PORT_Pin = MY_RT_MIL_STD_1553_PRMp_PIN;      	// PRMx+
	sPort.PORT_FUNC = MY_RT_MIL_STD_1553_PRMp_PORT_FUNC;
	RST_CLK_PCLKcmd( PCLK_BIT(MY_RT_MIL_STD_1553_PRMp_PORT), ENABLE);
	PORT_Init( MY_RT_MIL_STD_1553_PRMp_PORT, &sPort );

	sPort.PORT_Pin = MY_RT_MIL_STD_1553_PRMm_PIN;      	// PRMx-
	sPort.PORT_FUNC = MY_RT_MIL_STD_1553_PRMm_PORT_FUNC;
	RST_CLK_PCLKcmd( PCLK_BIT(MY_RT_MIL_STD_1553_PRMm_PORT), ENABLE);
	PORT_Init( MY_RT_MIL_STD_1553_PRMm_PORT, &sPort );

	sPort.PORT_Pin = MY_RT_MIL_STD_1553_PRDp_PIN;      	// PRDx+
	sPort.PORT_FUNC = MY_RT_MIL_STD_1553_PRDp_PORT_FUNC;
	RST_CLK_PCLKcmd( PCLK_BIT(MY_RT_MIL_STD_1553_PRDp_PORT), ENABLE);
	PORT_Init( MY_RT_MIL_STD_1553_PRDp_PORT, &sPort );

	sPort.PORT_Pin = MY_RT_MIL_STD_1553_PRDm_PIN;      	// PRDx-
	sPort.PORT_FUNC = MY_RT_MIL_STD_1553_PRDm_PORT_FUNC;
	RST_CLK_PCLKcmd( PCLK_BIT(MY_RT_MIL_STD_1553_PRDm_PORT), ENABLE);
	PORT_Init( MY_RT_MIL_STD_1553_PRDm_PORT, &sPort );

	sPort.PORT_Pin = MY_RT_MIL_STD_1553_PRD_PRM_PIN;      	// PRD_PRMx
	sPort.PORT_FUNC = MY_RT_MIL_STD_1553_PRD_PRM_PORT_FUNC;
	RST_CLK_PCLKcmd( PCLK_BIT(MY_RT_MIL_STD_1553_PRD_PRM_PORT), ENABLE);
	PORT_Init( MY_RT_MIL_STD_1553_PRD_PRM_PORT, &sPort );

	if ((g_MyRTAddress = Get_Module_Address()) == -1) //получение адреса платы
		{g_MyRTAddress = 0x3;} //если кросс-плата не установлена
	
	#ifdef __USE_DBG			
		sprintf (DBG_buffer, "adress=%u\r\n", g_MyRTAddress);
		DBG_PutString (DBG_buffer);	
	#endif	

	MIL_STD_1553_DeInit( MY_RT_MDR_MIL_STD_1553 ); // Reset all MIL_STD_1553 settings 

	#define MIL_STD_1553_HCLK_DIVIDER MIL_STD_1553_HCLKdiv4 //предделитель равен 4

	MIL_STD_1553_BRGInit( MIL_STD_1553_HCLK_DIVIDER ); //initializes the MIL_STD_1553 peripheral Clock according to the specified parameters

	#define MIL_STD_1553_CLOCK_DIVIDER (CPU_CLOCK_VALUE / (1 << MIL_STD_1553_HCLK_DIVIDER) / 1000000UL)

	MIL_STD_1553xStructInit( &sMIL1553 ); // MIL_STD_15531 configuration

	// Initialize MIL_STD_1553_InitStructure 
	sMIL1553.MIL_STD_1553_Mode = MIL_STD_1553_ModeTerminal; //режим оконечного устройства
	sMIL1553.MIL_STD_1553_RERR = DISABLE; // Автоматический сброс ошибок после установки флага IDLE
	sMIL1553.MIL_STD_1553_DIV = MIL_STD_1553_CLOCK_DIVIDER; //делитель частоты MAN_CLK до 1 МГц (Частота MAN_CLK обязательно должна быть не менее 24 МГц, не более 120 МГц и кратна 8)
	sMIL1553.MIL_STD_1553_RTA = g_MyRTAddress; //Адрес оконечного устройства (адрес, который присвоен устройству, если контроллер работает в режиме оконечного устройства)
	sMIL1553.MIL_STD_1553_TRA = DISABLE; //Блокировка передатчика основного канала
	sMIL1553.MIL_STD_1553_TRB = DISABLE; //Блокировка передатчика резервного канала
	
	MIL_STD_1553_Init(MY_RT_MDR_MIL_STD_1553, &sMIL1553); // Configure MIL_STD_15532 parameters

	MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //прерывание по переходу контроллера в неактивное состояние

	NVIC_SetPriority( MY_RT_MIL_STD_1553_IRQn, 2 );
	NVIC_EnableIRQ( MY_RT_MIL_STD_1553_IRQn );

 	MY_RT_MDR_MIL_STD_1553->StatusWord1 = 0;	//инициализация Ответного Слова (ОС)
	MY_RT_MDR_MIL_STD_1553->ModeData = 0; //инициализация Слова Данных (СД)

	MIL_STD_1553_Cmd( MY_RT_MDR_MIL_STD_1553, ENABLE ); //Enable MIL_STD_15531 peripheral
}

//-------------------------------------------------------------------------------//
void InitMIL1553B_RT( void )
{
  #if !defined( DEBUG_TIMEMARK_WITHOUT_MIL )
		InitRxMIL1553B();			// иниц-я периферии mil-std-1553b для приема по ЛПИ
  #else
		#warning Отладка! defined( DEBUG_TIMEMARK_WITHOUT_MIL )
  #endif
}
	
//-------------------------------------------------------------------------------//
MY_RT_MIL_STD_1553_IRQ_Handler()
{
  uint32_t status = MY_RT_MDR_MIL_STD_1553->STATUS;

	//если принято командное слово (в режиме КШ получено достоверное ответное слово)
	if((status & MIL_STD_1553_FLAG_RFLAGN) && (MY_RT_MDR_MIL_STD_1553->INTEN & MIL_STD_1553_IT_RFLAGNIE)) 
	{	
		//если принята команда "Передать слово ВСК"
		if( MY_RT_MDR_MIL_STD_1553->CommandWord1 == ((g_MyRTAddress << 11 ) | 0x0413))	
		{	
			MY_RT_MDR_MIL_STD_1553->ModeData = g_MyDeviceInfo.Value;
			//Контроллер интерфейса MIL-STD-1553B, входящий в 1986ВЕ1Т, сам передаст ответ и затем перейдет 
			//в режим IDLE. В этот момент можно будет заблокировать передатчик 
			MY_RT_MDR_MIL_STD_1553->CONTROL |= MIL_STD_1553_TRANSMITTER_MAIN; //Разблокировка передатчик для ответа
			MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE;	//Разрешение прерывания по IDLE (прерывание по переходу в неактивное состояние)
		}
		else 
		{
			//Если принято командное слово сообщения с хронометрической информацией
			if(MY_RT_MDR_MIL_STD_1553->CommandWord1 == 0xFAA6) 
			{
				// Разрешение прерывания по окончанию транзакции (VALMESS) и IDLE
				MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_VALMESSIE | MIL_STD_1553_IT_IDLEIE; 
			}		
			else // Если принято неизвестное командное слово
			{} // Не меняем флаги разрешения прерываний 
		}
	}
	else 
	{
		//успешное завершении транзакции 
		if((status & MIL_STD_1553_FLAG_VALMESS) && (MY_RT_MDR_MIL_STD_1553->INTEN & MIL_STD_1553_IT_VALMESSIE)) 
		{
			//MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //Разрешение прерывания по IDLE
			MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_RFLAGNIE; //Разрешение прерывания по приему командного слова
			
			if(MY_RT_MDR_MIL_STD_1553->CommandWord1 == 0xFAA6) //Если принято сообщение с хронометрической информацией
			{}
		}	
		else 
		{
			//Если перешёл в режим IDLE
			if((status & MIL_STD_1553_FLAG_IDLE) && (MY_RT_MDR_MIL_STD_1553->INTEN & MIL_STD_1553_IT_IDLEIE)) 
			{
				sprintf (DBG_buffer, "get_irq\r\n");
				DBG_PutString (DBG_buffer);
				MY_RT_MDR_MIL_STD_1553->CONTROL &= ~MIL_STD_1553_TRANSMITTER_MAIN; // Переход в режим IDLE и блокировка передатчика
				MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_RFLAGNIE; // Разрешение прерывания по приему командного слова
			}
		}
	}
}

//-----------------------------------------------------------------------------
#undef MY_RT_MIL_STD_1553_IRQn
#undef MY_RT_MDR_MIL_STD_1553

