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

	#define MIL_STD_1553_HCLK_DIVIDER MIL_STD_1553_HCLKdiv4 //������������ ����� 4
	#define MIL_STD_1553_CLOCK_DIVIDER (CPU_CLOCK_VALUE / (1 << MIL_STD_1553_HCLK_DIVIDER) / 1000000UL)
		
	MIL_STD_1553_BRGInit( MIL_STD_1553_HCLK_DIVIDER ); //initializes the MIL_STD_1553 peripheral Clock according to the specified parameters

	MIL_STD_1553xStructInit( &mil1553_init_struct ); // MIL_STD_15531 configuration

	// Initialize MIL_STD_1553_InitStructure 
	mil1553_init_struct.MIL_STD_1553_Mode = MIL_STD_1553_ModeTerminal; //����� ���������� ����������
	mil1553_init_struct.MIL_STD_1553_RERR = DISABLE; // �������������� ����� ������ ����� ��������� ����� IDLE
	mil1553_init_struct.MIL_STD_1553_DIV = MIL_STD_1553_CLOCK_DIVIDER; //�������� ������� MAN_CLK �� 1 ��� (������� MAN_CLK ����������� ������ ���� �� ����� 24 ���, �� ����� 120 ��� � ������ 8)
	mil1553_init_struct.MIL_STD_1553_RTA = g_MyRTAddress; //����� ���������� ���������� (�����, ������� �������� ����������, ���� ���������� �������� � ������ ���������� ����������)
	mil1553_init_struct.MIL_STD_1553_TRA = DISABLE; //���������� ����������� ��������� ������
	mil1553_init_struct.MIL_STD_1553_TRB = DISABLE; //���������� ����������� ���������� ������
	
	MIL_STD_1553_Init(ptr_MDR_MIL1553b, &mil1553_init_struct); // Configure MIL_STD_15531 parameters

	ptr_MDR_MIL1553b->INTEN = MIL_STD_1553_IT_IDLEIE; //���������� �� �������� ����������� � ���������� ���������
	//ptr_MDR_MIL1553b->INTEN = MIL_STD_1553_IT_RFLAGNIE; //���������� ���������� �� ������ ���������� �����
	
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
	MDR_MIL_STD_15531->StatusWord1 = 0;	//������������� ��������� ����� (��)
	MDR_MIL_STD_15531->ModeData = 0; //������������� ����� ������ (��)
	g_MyDeviceInfo.DeviceType = 0x10; //��������� ���� ������� ��� ��������� ��������� � �� MIL1553
	
	if ((g_MyRTAddress = Get_Module_Address()) == -1) //��������� ������ �����
	{g_MyRTAddress = 0x3;} //���� �����-����� �� �����������
	
	#ifdef __USE_DBG			
		sprintf (DBG_buffer, "adress=%u\r\n", g_MyRTAddress);
		DBG_PutString (DBG_buffer);	
	#endif	
	
	InitRxMIL1553B(MDR_MIL_STD_15531);	// ����-� 1 ������ ��������� mil-std-1553 ��� ������ �� ���
	InitRxMIL1553B(MDR_MIL_STD_15532); // ����-� 2 ������ ��������� mil-std-1553 ��� ������ �� ���

}
	
//-------------------------------------------------------------------------------//
void MIL_STD_1553B1_IRQHandler()
{
  uint32_t status = MDR_MIL_STD_15531->STATUS;

	//���� ������� ��������� ����� (� ������ �� �������� ����������� �������� �����)
	if((status & MIL_STD_1553_FLAG_RFLAGN) && (MDR_MIL_STD_15531->INTEN & MIL_STD_1553_IT_RFLAGNIE)) 
	{	
		//���� ������� ������� "�������� ����� ���"
		if( MDR_MIL_STD_15531->CommandWord1 == ((g_MyRTAddress << 11 ) | 0x0413))	
		{	
			MDR_MIL_STD_15531->ModeData = g_MyDeviceInfo.Value;
			//���������� ���������� MIL-STD-1553B, �������� � 1986��1�, ��� �������� ����� � ����� �������� 
			//� ����� IDLE. � ���� ������ ����� ����� ������������� ���������� 
			MDR_MIL_STD_15531->CONTROL |= MIL_STD_1553_TRANSMITTER_MAIN; //������������� ���������� ��� ������
			MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_IDLEIE;	//���������� ���������� �� IDLE (���������� �� �������� � ���������� ���������)
		}
		else 
		{
			//���� ������� ��������� ����� ��������� � ���������������� �����������
			if(MDR_MIL_STD_15531->CommandWord1 == 0xFAA6) 
			{
				// ���������� ���������� �� ��������� ���������� (VALMESS) � IDLE
				MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_VALMESSIE | MIL_STD_1553_IT_IDLEIE; 
			}		
			else // ���� ������� ����������� ��������� �����
			{} // �� ������ ����� ���������� ���������� 
		}
	}
	else 
	{
		//�������� ���������� ���������� 
		if((status & MIL_STD_1553_FLAG_VALMESS) && (MDR_MIL_STD_15531->INTEN & MIL_STD_1553_IT_VALMESSIE)) 
		{
			//MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //���������� ���������� �� IDLE
			MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_RFLAGNIE; //���������� ���������� �� ������ ���������� �����
			
			if(MDR_MIL_STD_15531->CommandWord1 == 0xFAA6) //���� ������� ��������� � ���������������� �����������
			{}
		}	
		else 
		{
			//���� ������� � ����� IDLE
			if((status & MIL_STD_1553_FLAG_IDLE) && (MDR_MIL_STD_15531->INTEN & MIL_STD_1553_IT_IDLEIE)) 
			{
				
				#ifdef __USE_DBG		
					DBG_PutString ("get_irq1\r\n");
				#endif
				
				MDR_MIL_STD_15531->CONTROL &= ~MIL_STD_1553_TRANSMITTER_MAIN; // ������� � ����� IDLE � ���������� �����������
				MDR_MIL_STD_15531->INTEN = MIL_STD_1553_IT_RFLAGNIE; // ���������� ���������� �� ������ ���������� �����
			}
		}
	}
}

//-------------------------------------------------------------------------------//
void MIL_STD_1553B2_IRQHandler()
{
	uint32_t status = MDR_MIL_STD_15532->STATUS;

	//���� ������� ��������� ����� (� ������ �� �������� ����������� �������� �����)
	if((status & MIL_STD_1553_FLAG_RFLAGN) && (MDR_MIL_STD_15532->INTEN & MIL_STD_1553_IT_RFLAGNIE)) 
	{	
		//���� ������� ������� "�������� ����� ���"
		if( MDR_MIL_STD_15532->CommandWord1 == ((g_MyRTAddress << 11 ) | 0x0413))	
		{	
			MDR_MIL_STD_15532->ModeData = g_MyDeviceInfo.Value;
			//���������� ���������� MIL-STD-1553B, �������� � 1986��1�, ��� �������� ����� � ����� �������� 
			//� ����� IDLE. � ���� ������ ����� ����� ������������� ���������� 
			MDR_MIL_STD_15532->CONTROL |= MIL_STD_1553_TRANSMITTER_MAIN; //������������� ���������� ��� ������
			MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_IDLEIE;	//���������� ���������� �� IDLE (���������� �� �������� � ���������� ���������)
		}
		else 
		{
			//���� ������� ��������� ����� ��������� � ���������������� �����������
			if(MDR_MIL_STD_15532->CommandWord1 == 0xFAA6) 
			{
				// ���������� ���������� �� ��������� ���������� (VALMESS) � IDLE
				MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_VALMESSIE | MIL_STD_1553_IT_IDLEIE; 
			}		
			else // ���� ������� ����������� ��������� �����
			{} // �� ������ ����� ���������� ���������� 
		}
	}
	else 
	{
		//�������� ���������� ���������� 
		if((status & MIL_STD_1553_FLAG_VALMESS) && (MDR_MIL_STD_15532->INTEN & MIL_STD_1553_IT_VALMESSIE)) 
		{
			//MY_RT_MDR_MIL_STD_1553->INTEN = MIL_STD_1553_IT_IDLEIE; //���������� ���������� �� IDLE
			MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_RFLAGNIE; //���������� ���������� �� ������ ���������� �����
			
			if(MDR_MIL_STD_15531->CommandWord1 == 0xFAA6) //���� ������� ��������� � ���������������� �����������
			{}
		}	
		else 
		{
			//���� ������� � ����� IDLE
			if((status & MIL_STD_1553_FLAG_IDLE) && (MDR_MIL_STD_15532->INTEN & MIL_STD_1553_IT_IDLEIE)) 
			{
				#ifdef __USE_DBG		
					DBG_PutString ("get_irq2\r\n");
				#endif
				
				MDR_MIL_STD_15532->CONTROL &= ~MIL_STD_1553_TRANSMITTER_MAIN; // ������� � ����� IDLE � ���������� �����������
				MDR_MIL_STD_15532->INTEN = MIL_STD_1553_IT_RFLAGNIE; // ���������� ���������� �� ������ ���������� �����
			}
		}
	}
}






