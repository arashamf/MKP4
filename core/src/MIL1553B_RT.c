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

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
int16_t g_MyRTAddress = -1;
	
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void InitRxMIL1553B(MDR_MIL_STD_1553_TypeDef *);

//-----------------------------------------------------------------------------//
static void InitRxMIL1553B	(MDR_MIL_STD_1553_TypeDef * ptr_MDR_MIL1553b )
{
  PORT_InitTypeDef port_init_struct;
  MIL_STD_1553_InitTypeDef mil1553_init_struct;

	if(ptr_MDR_MIL1553b == MDR_MIL_STD_15531)
	{
		RST_CLK_PCLKcmd( PCLK_BIT (MDR_MIL_STD_15531_BASE), ENABLE); 	// Enable peripheral clocks
		PORT_StructInit( &port_init_struct );
		port_init_struct.PORT_SPEED = PORT_SPEED_MAXFAST;
		port_init_struct.PORT_MODE = PORT_MODE_DIGITAL;

		port_init_struct.PORT_Pin = MDR_MIL_STD_15531_PRMp_PIN;      	// PRMx+
		port_init_struct.PORT_FUNC = MDR_MIL_STD_15531_PRMp_PORT_FUNC;
		RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15531_PRMp_PORT), ENABLE);
		PORT_Init( MDR_MIL_STD_15531_PRMp_PORT, &port_init_struct );

		port_init_struct.PORT_Pin = MDR_MIL_STD_15531_PRMm_PIN;      	// PRMx-
		port_init_struct.PORT_FUNC = MDR_MIL_STD_15531_PRMm_PORT_FUNC;
		RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15531_PRMm_PORT), ENABLE);
		PORT_Init( MDR_MIL_STD_15531_PRMm_PORT, &port_init_struct );

		port_init_struct.PORT_Pin = MDR_MIL_STD_15531_PRDp_PIN;      	// PRDx+
		port_init_struct.PORT_FUNC = MDR_MIL_STD_15531_PRDp_PORT_FUNC;
		RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15531_PRDp_PORT), ENABLE);
		PORT_Init( MDR_MIL_STD_15531_PRDp_PORT, &port_init_struct );

		port_init_struct.PORT_Pin = MDR_MIL_STD_15531_PRDm_PIN;      	// PRDx-
		port_init_struct.PORT_FUNC = MDR_MIL_STD_15531_PRDm_PORT_FUNC;
		RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15531_PRDm_PORT), ENABLE);
		PORT_Init( MDR_MIL_STD_15531_PRDm_PORT, &port_init_struct );

		port_init_struct.PORT_Pin = MDR_MIL_STD_15531_PRD_PRM_PIN;      	// PRD_PRMx
		port_init_struct.PORT_FUNC = MDR_MIL_STD_15531_PRD_PRM_PORT_FUNC;
		RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15531_PRD_PRM_PORT), ENABLE);
		PORT_Init( MDR_MIL_STD_15531_PRD_PRM_PORT, &port_init_struct );
	}
	else
	{
		if (ptr_MDR_MIL1553b == MDR_MIL_STD_15532)
		{
			RST_CLK_PCLKcmd( PCLK_BIT (MDR_MIL_STD_15532_BASE), ENABLE); 	// Enable peripheral clocks
			
			PORT_StructInit( &port_init_struct );
			port_init_struct.PORT_SPEED = PORT_SPEED_MAXFAST;
			port_init_struct.PORT_MODE = PORT_MODE_DIGITAL;

			port_init_struct.PORT_Pin = MDR_MIL_STD_15532_PRMp_PIN;      	// PRMx+
			port_init_struct.PORT_FUNC = MDR_MIL_STD_15532_PRMp_PORT_FUNC;
			RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15532_PRMp_PORT), ENABLE);
			PORT_Init( MDR_MIL_STD_15532_PRMp_PORT, &port_init_struct );

			port_init_struct.PORT_Pin = MDR_MIL_STD_15532_PRMm_PIN;      	// PRMx-
			port_init_struct.PORT_FUNC = MDR_MIL_STD_15532_PRMm_PORT_FUNC;
			RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15532_PRMm_PORT), ENABLE);
			PORT_Init( MDR_MIL_STD_15532_PRMm_PORT, &port_init_struct );

			port_init_struct.PORT_Pin = MDR_MIL_STD_15532_PRDp_PIN;      	// PRDx+
			port_init_struct.PORT_FUNC = MDR_MIL_STD_15532_PRDp_PORT_FUNC;
			RST_CLK_PCLKcmd	( PCLK_BIT(MDR_MIL_STD_15532_PRDp_PORT), ENABLE);
			PORT_Init( MDR_MIL_STD_15532_PRDp_PORT, &port_init_struct );

			port_init_struct.PORT_Pin = MDR_MIL_STD_15532_PRDm_PIN;      	// PRDx-
			port_init_struct.PORT_FUNC = MDR_MIL_STD_15532_PRDm_PORT_FUNC;
			RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15532_PRDm_PORT), ENABLE);
			PORT_Init( MDR_MIL_STD_15532_PRDm_PORT, &port_init_struct );

			port_init_struct.PORT_Pin = MDR_MIL_STD_15532_PRD_PRM_PIN;      	// PRD_PRMx
			port_init_struct.PORT_FUNC = MDR_MIL_STD_15532_PRD_PRM_PORT_FUNC;
			RST_CLK_PCLKcmd( PCLK_BIT(MDR_MIL_STD_15532_PRD_PRM_PORT), ENABLE);
			PORT_Init( MDR_MIL_STD_15532_PRD_PRM_PORT, &port_init_struct );
		}
		else
			{return;}
	}

	MIL_STD_1553_DeInit( ptr_MDR_MIL1553b ); // Reset all MIL_STD_15531 settings 

	#define MIL_STD_1553_HCLK_DIVIDER MIL_STD_1553_HCLKdiv4 //предделитель равен 4
	#define MIL_STD_1553_CLOCK_DIVIDER (CPU_CLOCK_VALUE / (1 << MIL_STD_1553_HCLK_DIVIDER) / 1000000UL)
		
	MIL_STD_1553_BRGInit( MIL_STD_1553_HCLK_DIVIDER ); //initializes the MIL_STD_1553 peripheral Clock according to the specified parameters

	MIL_STD_1553xStructInit( &mil1553_init_struct ); // MIL_STD_15531 configuration

	// Initialize MIL_STD_1553_InitStructure 
	mil1553_init_struct.MIL_STD_1553_Mode = MIL_STD_1553_ModeTerminal; //режим оконечного устройства
	mil1553_init_struct.MIL_STD_1553_RERR = DISABLE; // Автоматический сброс ошибок после установки флага IDLE
	mil1553_init_struct.MIL_STD_1553_DIV = MIL_STD_1553_CLOCK_DIVIDER; //делитель частоты MAN_CLK до 1 МГц (Частота MAN_CLK обязательно должна быть не менее 24 МГц, не более 120 МГц и кратна 8)
	mil1553_init_struct.MIL_STD_1553_RTA = g_MyRTAddress; //Адрес оконечного устройства (адрес, который присвоен устройству, если контроллер работает в режиме оконечного устройства)
	mil1553_init_struct.MIL_STD_1553_TRA = DISABLE; //Блокировка передатчика основного канала
	mil1553_init_struct.MIL_STD_1553_TRB = DISABLE; //Блокировка передатчика резервного канала
	
	MIL_STD_1553_Init(ptr_MDR_MIL1553b, &mil1553_init_struct); // Configure MIL_STD_15531 parameters

	ptr_MDR_MIL1553b->INTEN = MIL_STD_1553_IT_IDLEIE; //прерывание по переходу контроллера в неактивное состояние
	//ptr_MDR_MIL1553b->INTEN = MIL_STD_1553_IT_RFLAGNIE; //разрешение прерывания по приему командного слова
	
	if(ptr_MDR_MIL1553b == MDR_MIL_STD_15531)
	{
		NVIC_EnableIRQ( MIL_STD_1553B1_IRQn );
		NVIC_SetPriority( MIL_STD_1553B1_IRQn, 2 );
	}
	else
	{
		if (ptr_MDR_MIL1553b == MDR_MIL_STD_15532)
		{
			NVIC_EnableIRQ( MIL_STD_1553B2_IRQn );
			NVIC_SetPriority( MIL_STD_1553B2_IRQn, 2 );
		}
		else
			return;
	}

	MIL_STD_1553_Cmd( ptr_MDR_MIL1553b, ENABLE ); //Enable MIL_STD_15531 peripheral
}

//-------------------------------------------------------------------------------//
void InitMIL1553B_RT( void )
{
	MDR_MIL_STD_15531->StatusWord1 = 0;	//инициализация Ответного Слова (ОС)
	MDR_MIL_STD_15531->ModeData = 0; //инициализация Слова Данных (СД)
	g_MyDeviceInfo.DeviceType = 0x10; //установка типа прибора для ответного сообщения к КШ MIL1553
	
	if ((g_MyRTAddress = Get_Module_Address()) == -1) //получение адреса платы
	{g_MyRTAddress = 0x3;} //если кросс-плата не установлена
	
	#ifdef __USE_DBG			
		sprintf (DBG_buffer, "adress=%u\r\n", g_MyRTAddress);
		DBG_PutString (DBG_buffer);	
	#endif	
	
	InitRxMIL1553B(MDR_MIL_STD_15531);	// иниц-я 1 канала периферии mil-std-1553 для приема по ЛПИ
	InitRxMIL1553B(MDR_MIL_STD_15532); // иниц-я 2 канала периферии mil-std-1553 для приема по ЛПИ

}
	
//-------------------------------------------------------------------------------//
void MIL_STD_1553B1_IRQHandler()
{
  uint32_t status = MDR_MIL_STD_15531->STATUS;

	//если принято командное слово (в режиме КШ получено достоверное ответное слово)
	if((status & MIL_STD_1553_FLAG_RFLAGN) && (MDR_MIL_STD_15531->INTEN & MIL_STD_1553_IT_RFLAGNIE)) 
	{	
		//если принята команда "Передать слово ВСК"
		if( MDR_MIL_STD_15531->CommandWord1 == ((g_MyRTAddress << 11 ) | 0x0413))	
		{	
			MDR_MIL_STD_15531->ModeData = g_MyDeviceInfo.Value;
			//Контроллер интерфейса MIL-STD-1553B, входящий в 1986ВЕ1Т, сам передаст ответ и затем перейдет 
			//в режим IDLE. В этот момент можно будет заблокировать передатчик 
			MDR_MIL_STD_15531->CONTROL |= MIL_STD_1553_TRANSMITTER_MAIN; //Разблокировка передатчик для ответа
			MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_IDLEIE;	//Разрешение прерывания по IDLE (прерывание по переходу в неактивное состояние)
		}
		else 
		{
			//Если принято командное слово сообщения с хронометрической информацией
			if(MDR_MIL_STD_15531->CommandWord1 == 0xFAA6) 
			{
				// Разрешение прерывания по окончанию транзакции (VALMESS) и IDLE
				MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_VALMESSIE | MIL_STD_1553_IT_IDLEIE; 
			}		
			else // Если принято неизвестное командное слово
			{} // Не меняем флаги разрешения прерываний 
		}
	}
	else 
	{
		//успешное завершении транзакции 
		if((status & MIL_STD_1553_FLAG_VALMESS) && (MDR_MIL_STD_15531->INTEN & MIL_STD_1553_IT_VALMESSIE)) 
		{
			//MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //Разрешение прерывания по IDLE
			MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_RFLAGNIE; //Разрешение прерывания по приему командного слова
			
			if(MDR_MIL_STD_15531->CommandWord1 == 0xFAA6) //Если принято сообщение с хронометрической информацией
			{}
		}	
		else 
		{
			//Если перешёл в режим IDLE
			if((status & MIL_STD_1553_FLAG_IDLE) && (MDR_MIL_STD_15531->INTEN & MIL_STD_1553_IT_IDLEIE)) 
			{
				
				#ifdef __USE_DBG		
					DBG_PutString ("get_irq1\r\n");
				#endif
				
				MDR_MIL_STD_15531->CONTROL &= ~MIL_STD_1553_TRANSMITTER_MAIN; // Переход в режим IDLE и блокировка передатчика
				MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_RFLAGNIE; // Разрешение прерывания по приему командного слова
			}
		}
	}
}

//-------------------------------------------------------------------------------//
void MIL_STD_1553B2_IRQHandler()
{
	uint32_t status = MDR_MIL_STD_15532->STATUS;

	//если принято командное слово (в режиме КШ получено достоверное ответное слово)
	if((status & MIL_STD_1553_FLAG_RFLAGN) && (MDR_MIL_STD_15532->INTEN & MIL_STD_1553_IT_RFLAGNIE)) 
	{	
		//если принята команда "Передать слово ВСК"
		if( MDR_MIL_STD_15532->CommandWord1 == ((g_MyRTAddress << 11 ) | 0x0413))	
		{	
			MDR_MIL_STD_15532->ModeData = g_MyDeviceInfo.Value;
			//Контроллер интерфейса MIL-STD-1553B, входящий в 1986ВЕ1Т, сам передаст ответ и затем перейдет 
			//в режим IDLE. В этот момент можно будет заблокировать передатчик 
			MDR_MIL_STD_15532->CONTROL |= MIL_STD_1553_TRANSMITTER_MAIN; //Разблокировка передатчик для ответа
			MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_IDLEIE;	//Разрешение прерывания по IDLE (прерывание по переходу в неактивное состояние)
		}
		else 
		{
			//Если принято командное слово сообщения с хронометрической информацией
			if(MDR_MIL_STD_15532->CommandWord1 == 0xFAA6) 
			{
				// Разрешение прерывания по окончанию транзакции (VALMESS) и IDLE
				MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_VALMESSIE | MIL_STD_1553_IT_IDLEIE; 
			}		
			else // Если принято неизвестное командное слово
			{} // Не меняем флаги разрешения прерываний 
		}
	}
	else 
	{
		//успешное завершении транзакции 
		if((status & MIL_STD_1553_FLAG_VALMESS) && (MDR_MIL_STD_15532->INTEN & MIL_STD_1553_IT_VALMESSIE)) 
		{
			//MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //Разрешение прерывания по IDLE
			MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_RFLAGNIE; //Разрешение прерывания по приему командного слова
			
			if(MDR_MIL_STD_15531->CommandWord1 == 0xFAA6) //Если принято сообщение с хронометрической информацией
			{}
		}	
		else 
		{
			//Если перешёл в режим IDLE
			if((status & MIL_STD_1553_FLAG_IDLE) && (MDR_MIL_STD_15532->INTEN & MIL_STD_1553_IT_IDLEIE)) 
			{
				#ifdef __USE_DBG		
					DBG_PutString ("get_irq2\r\n");
				#endif
				
				MDR_MIL_STD_15532->CONTROL &= ~MIL_STD_1553_TRANSMITTER_MAIN; // Переход в режим IDLE и блокировка передатчика
				MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_RFLAGNIE; // Разрешение прерывания по приему командного слова
			}
		}
	}
}






