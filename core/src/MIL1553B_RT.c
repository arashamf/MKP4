/*
 ������ MIL1553B_RT.c ������������ ���:
  - ������ ��������� � ���������������� ����������� �� ��� �� ���-�;
  - ������ �� ������� ��� � ��������� ������� (��� ��������� ����������� �� ���3, ���, ��� � ����., ��. CAN.c);
  - ������ ����� ������� �� ����� ������� (� ������ ��������� - ��), ����������� � ��������� ������ ���;
  - �������� ��������� �� (��������� ���� A1);
  - ����������� ������� � ������������� ���������������� ���������� �� ���.

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


	// MIL-STD-1553B Ports Init. ����������� �������� ��. � HW_Profile.h
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

	if ((g_MyRTAddress = Get_Module_Address()) == -1) //��������� ������ �����
		{g_MyRTAddress = 0x3;} //���� �����-����� �� �����������
	
	#ifdef __USE_DBG			
		sprintf (DBG_buffer, "adress=%u\r\n", g_MyRTAddress);
		DBG_PutString (DBG_buffer);	
	#endif	

	MIL_STD_1553_DeInit( MY_RT_MDR_MIL_STD_1553 ); // Reset all MIL_STD_1553 settings 

	#define MIL_STD_1553_HCLK_DIVIDER MIL_STD_1553_HCLKdiv4 //������������ ����� 4

	MIL_STD_1553_BRGInit( MIL_STD_1553_HCLK_DIVIDER ); //initializes the MIL_STD_1553 peripheral Clock according to the specified parameters

	#define MIL_STD_1553_CLOCK_DIVIDER (CPU_CLOCK_VALUE / (1 << MIL_STD_1553_HCLK_DIVIDER) / 1000000UL)

	MIL_STD_1553xStructInit( &sMIL1553 ); // MIL_STD_15531 configuration

	// Initialize MIL_STD_1553_InitStructure 
	sMIL1553.MIL_STD_1553_Mode = MIL_STD_1553_ModeTerminal; //����� ���������� ����������
	sMIL1553.MIL_STD_1553_RERR = DISABLE; // �������������� ����� ������ ����� ��������� ����� IDLE
	sMIL1553.MIL_STD_1553_DIV = MIL_STD_1553_CLOCK_DIVIDER; //�������� ������� MAN_CLK �� 1 ��� (������� MAN_CLK ����������� ������ ���� �� ����� 24 ���, �� ����� 120 ��� � ������ 8)
	sMIL1553.MIL_STD_1553_RTA = g_MyRTAddress; //����� ���������� ���������� (�����, ������� �������� ����������, ���� ���������� �������� � ������ ���������� ����������)
	sMIL1553.MIL_STD_1553_TRA = DISABLE; //���������� ����������� ��������� ������
	sMIL1553.MIL_STD_1553_TRB = DISABLE; //���������� ����������� ���������� ������
	
	MIL_STD_1553_Init(MY_RT_MDR_MIL_STD_1553, &sMIL1553); // Configure MIL_STD_15532 parameters

	MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //���������� �� �������� ����������� � ���������� ���������

	NVIC_SetPriority( MY_RT_MIL_STD_1553_IRQn, 2 );
	NVIC_EnableIRQ( MY_RT_MIL_STD_1553_IRQn );

 	MY_RT_MDR_MIL_STD_1553->StatusWord1 = 0;	//������������� ��������� ����� (��)
	MY_RT_MDR_MIL_STD_1553->ModeData = 0; //������������� ����� ������ (��)

	MIL_STD_1553_Cmd( MY_RT_MDR_MIL_STD_1553, ENABLE ); //Enable MIL_STD_15531 peripheral
}

//-------------------------------------------------------------------------------//
void InitMIL1553B_RT( void )
{
  #if !defined( DEBUG_TIMEMARK_WITHOUT_MIL )
		InitRxMIL1553B();			// ����-� ��������� mil-std-1553b ��� ������ �� ���
  #else
		#warning �������! defined( DEBUG_TIMEMARK_WITHOUT_MIL )
  #endif
}
	
//-------------------------------------------------------------------------------//
MY_RT_MIL_STD_1553_IRQ_Handler()
{
  uint32_t status = MY_RT_MDR_MIL_STD_1553->STATUS;

	//���� ������� ��������� ����� (� ������ �� �������� ����������� �������� �����)
	if((status & MIL_STD_1553_FLAG_RFLAGN) && (MY_RT_MDR_MIL_STD_1553->INTEN & MIL_STD_1553_IT_RFLAGNIE)) 
	{	
		//���� ������� ������� "�������� ����� ���"
		if( MY_RT_MDR_MIL_STD_1553->CommandWord1 == ((g_MyRTAddress << 11 ) | 0x0413))	
		{	
			MY_RT_MDR_MIL_STD_1553->ModeData = g_MyDeviceInfo.Value;
			//���������� ���������� MIL-STD-1553B, �������� � 1986��1�, ��� �������� ����� � ����� �������� 
			//� ����� IDLE. � ���� ������ ����� ����� ������������� ���������� 
			MY_RT_MDR_MIL_STD_1553->CONTROL |= MIL_STD_1553_TRANSMITTER_MAIN; //������������� ���������� ��� ������
			MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE;	//���������� ���������� �� IDLE (���������� �� �������� � ���������� ���������)
		}
		else 
		{
			//���� ������� ��������� ����� ��������� � ���������������� �����������
			if(MY_RT_MDR_MIL_STD_1553->CommandWord1 == 0xFAA6) 
			{
				// ���������� ���������� �� ��������� ���������� (VALMESS) � IDLE
				MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_VALMESSIE | MIL_STD_1553_IT_IDLEIE; 
			}		
			else // ���� ������� ����������� ��������� �����
			{} // �� ������ ����� ���������� ���������� 
		}
	}
	else 
	{
		//�������� ���������� ���������� 
		if((status & MIL_STD_1553_FLAG_VALMESS) && (MY_RT_MDR_MIL_STD_1553->INTEN & MIL_STD_1553_IT_VALMESSIE)) 
		{
			//MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //���������� ���������� �� IDLE
			MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_RFLAGNIE; //���������� ���������� �� ������ ���������� �����
			
			if(MY_RT_MDR_MIL_STD_1553->CommandWord1 == 0xFAA6) //���� ������� ��������� � ���������������� �����������
			{}
		}	
		else 
		{
			//���� ������� � ����� IDLE
			if((status & MIL_STD_1553_FLAG_IDLE) && (MY_RT_MDR_MIL_STD_1553->INTEN & MIL_STD_1553_IT_IDLEIE)) 
			{
				sprintf (DBG_buffer, "get_irq\r\n");
				DBG_PutString (DBG_buffer);
				MY_RT_MDR_MIL_STD_1553->CONTROL &= ~MIL_STD_1553_TRANSMITTER_MAIN; // ������� � ����� IDLE � ���������� �����������
				MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_RFLAGNIE; // ���������� ���������� �� ������ ���������� �����
			}
		}
	}
}

//-----------------------------------------------------------------------------
#undef MY_RT_MIL_STD_1553_IRQn
#undef MY_RT_MDR_MIL_STD_1553

