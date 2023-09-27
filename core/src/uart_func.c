// Includes --------------------------------------------------------------------//
#include "main.h"
#include "HW_Profile.h"
#include "uart_func.h"
//#include "pins.h"

// Exported types ---------------------------------------------------------------//
typedef enum {UART_RX_MODE = 0, UART_TX_MODE = 1} UartMode; //����� �����

// Private define ---------------------------------------------------------------//
// Private consts --------------------------------------------------------------//
// Private variables -----------------------------------------------------------//
char DBG_buffer[64];

// Private functions prototypes -----------------------------------------------//
static void UART_InitIRQ(IRQn_Type IRQn, uint32_t priority);
static void UARTSetBaud(MDR_UART_TypeDef* , uint32_t , uint32_t );

//--------------------------������������� ������ UART ��� ������--------------------------//
void DBG_LoLevel_Init(MDR_UART_TypeDef* UARTx, uint32_t UARTx_CLOCK, uint32_t uartBaudRate)
{
  PORT_InitTypeDef GPIOInitStruct; // ��������� ��� ������������� ����� �����-������

  UART_InitTypeDef UARTInitStruct;  // ��������� ��� ������������� ������ UART

  RST_CLK_PCLKcmd(UARTx_CLOCK, ENABLE); // ���������� ������������ �����  � ������ UART

  // ����� ������������ ����� �����-������
  PORT_StructInit (&GPIOInitStruct);
  GPIOInitStruct.PORT_SPEED = PORT_SPEED_MAXFAST;
  GPIOInitStruct.PORT_MODE  = PORT_MODE_DIGITAL;

			// ������������ � ������������� ����� ��� �������� ������ 
	RST_CLK_PCLKcmd(DBG_CLOCK_Pin_TX , ENABLE);
	GPIOInitStruct.PORT_FUNC  = DBG_PORT_FuncTX ;	
	GPIOInitStruct.PORT_OE    = PORT_OE_OUT;
	GPIOInitStruct.PORT_Pin   = DBG_PinTX;
	PORT_Init(DBG_PORT_TX, &GPIOInitStruct);	

  // ������������ ������ UART
  UARTInitStruct.UART_BaudRate            = uartBaudRate;                  // �������� �������� ������
  UARTInitStruct.UART_WordLength          = UART_WordLength8b;             // ���������� ����� ������ � ���������
  UARTInitStruct.UART_StopBits            = UART_StopBits1;                // ���������� STOP-�����
  UARTInitStruct.UART_Parity              = UART_Parity_No;                // �������� ��������
  UARTInitStruct.UART_FIFOMode            = UART_FIFO_OFF;                 // ���������/���������� ������
  UARTInitStruct.UART_HardwareFlowControl = UART_HardwareFlowControl_TXE;   // ���������� �������� �� ��������� � ������� ������
                                          

  UART_Init(UARTx, &UARTInitStruct);  // ������������� ������ UART

  UART_BRGInit(UARTx, UART_HCLKdiv1);  // ����� ������������ �������� ������� ������ UART
  UART_Cmd(UARTx, ENABLE); // ���������� ������ ������ UART
}

//----------------------------------------------------------------------------------------------//
static void UART_InitIRQ(IRQn_Type IRQn, uint32_t priority)
{
  NVIC_SetPriority(IRQn, priority);  // ���������� ���������� ����������� ���������� �� UART

  NVIC_EnableIRQ(IRQn); // ���������� ���������� ���������� �� UART
}	

//----------------------------------------------------------------------------------------------//
static void UARTSetBaud(MDR_UART_TypeDef* UARTx, uint32_t baudRate, uint32_t freqCPU)
{
	uint32_t divider = freqCPU / (baudRate >> 2);
	uint32_t CR_tmp = UARTx->CR;
	uint32_t LCR_tmp = UARTx->LCR_H;
	
	while ( (UARTx->FR & UART_FLAG_BUSY) ) 
		{__NOP();}		

  UARTx->CR = 0;
  UARTx->IBRD = divider >> 6;
  UARTx->FBRD = divider & 0x003F;
  UARTx->LCR_H = LCR_tmp;
  UARTx->CR = CR_tmp;
}

//-------------------------------------����� ��������� ��� ������-------------------------------------//
void DBG_PutString (char * str)
{
	auto char smb;
	
	while  ((smb = *str++) != 0)
	{
		while(UART_GetFlagStatus(DBG_UART, UART_FLAG_BUSY) == SET) {}
		UART_SendData(DBG_UART, smb);
	}
}

//------------------------------------��������� ������� �� UART1------------------------------------//
uint8_t UART_CharReception_Callback (void)
{
	auto uint8_t smb;
	if ( UART_GetITStatusMasked(DBG_UART, UART_IT_RX) == SET ) 
	{
    UART_ClearITPendingBit(DBG_UART, UART_IT_RX);
		smb = UART_ReceiveData(DBG_UART); // ���� ����� ������ �� GPS ���������  
 	}
	return smb;
}

