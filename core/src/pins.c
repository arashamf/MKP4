
// Includes---------------------------------------------------------------------------------------//
#include "main.h"
#include "pins.h"
#include "HW_Profile.h"
#include "XTick.h"
#include "typedef.h"

static void Pins_Address_Init(const TPortPin *, uint8_t );
static void InitLED_Pin( const TPortPin *PortPin );

	
const TPortPin check_pins[] = 
{
//	{CHANNEL1_27V_PORT, 	PIN_CHANNEL1_27V },
//	{CHANNEL2_27V_PORT, 	PIN_CHANNEL2_27V },
	{CHANNEL3_27V_PORT, 	PIN_CHANNEL3_27V },
	{CHANNEL4_27V_PORT,	 	PIN_CHANNEL4_27V },
	{CHANNEL1_220V_PORT, 	PIN_CHANNEL1_220V},
//	{CHANNEL2_220V_PORT, 	PIN_CHANNEL2_220V}
};



uint8_t number_check_pins = sizeof(check_pins)/sizeof (check_pins[0]); //���������� ����������� �������

Status_Device MKP4_channel_status; //���������� ��������� � ������� ����������� �������

//--------------------������������� ����� ��������� ������ ������ � ����������--------------------//
static void Pins_Address_Init(const TPortPin * pins, uint8_t number_pins)
{
	uint8_t count;	
	
	PORT_InitTypeDef sPort;
	RST_CLK_PCLKcmd(BACKPLANE_PIN_CLOCK, ENABLE);
	PORT_StructInit( &sPort );
	
	sPort.PORT_OE    = PORT_OE_IN; //����� - ����
	sPort.PORT_FUNC  = PORT_FUNC_PORT;  //������� ������
	sPort.PORT_MODE  = PORT_MODE_DIGITAL;  //�������� ����� ����
	sPort.PORT_SPEED = PORT_SPEED_SLOW;
	
	for(count = 0 ; count < number_pins; count++ )
	{		
		//RST_CLK_PCLKcmd( PCLK_BIT( pins[count].PORTx ), ENABLE ); // Enable peripheral clock for Port
		sPort.PORT_Pin   = pins[count].PORT_Pin; // Configure Pin
		PORT_Init( pins[count].PORTx, &sPort );
	}
}

//------------------------------��������� ������ ������ � ����������------------------------------//
int8_t Get_Module_Address(void)
{
	uint8_t count = 0;
	int8_t addr = 0;
	uint8_t number_pins = 0;
	
	const TPortPin adr_pins[] = 
	{
		{ BACKPLANE_ADDR0_PORT, BACKPLANE_ADDR0_PIN },
		{ BACKPLANE_ADDR1_PORT, BACKPLANE_ADDR1_PIN },
		{ BACKPLANE_ADDR2_PORT, BACKPLANE_ADDR2_PIN },
		{ BACKPLANE_ADDR3_PORT, BACKPLANE_ADDR3_PIN },
		{ BACKPLANE_ADDR4_PORT, BACKPLANE_ADDR4_PIN }
	};
	
	number_pins = sizeof(adr_pins)/sizeof (adr_pins[0]); //���������� ����� ��������� ������
	Pins_Address_Init(adr_pins, number_pins);
	
	DelayMs(500);

	for(count = 0 ; count < number_pins; count++ )
	{
		addr |= ( (!PORT_ReadInputDataBit(adr_pins[count].PORTx, adr_pins[count].PORT_Pin )) << count); 
	}
	#ifdef __USE_DBG
		sprintf (DBG_buffer, "my_adress=%d\r\n", addr);
		DBG_PutString (DBG_buffer);
	#endif		 	 
	
	if ( addr != 0x00 ) 
		{return addr;} //���� ������� �����
	else 
		{return -1;} //���� �����-����� �� ���������
}

//---------------------------������������� ����� �������� ����������---------------------------//
void Pins_CheckVoltage_Init(void)
{
	uint8_t count = 0;
	
	PORT_InitTypeDef sPort;
	RST_CLK_PCLKcmd(CHECK_PIN_CLOCK, ENABLE);
	PORT_StructInit( &sPort );
	
	sPort.PORT_OE    = PORT_OE_IN; //����� - ����
	sPort.PORT_FUNC  = PORT_FUNC_PORT;  //������� ������
	sPort.PORT_MODE  = PORT_MODE_DIGITAL;  //�������� ����� ����
	sPort.PORT_SPEED = PORT_SPEED_SLOW;
	
	for(count = 0 ; count < number_check_pins; count++ )
	{		
		sPort.PORT_Pin   = check_pins[count].PORT_Pin; // Configure Pin
		PORT_Init( check_pins[count].PORTx, &sPort );
	}
	
	MKP4_channel_status.flags = 0; //������������� ��������� � ������� ����������� �������
}

//---------------------------�������� ������� ������� ���---------------------------//
uint8_t Check_Voltage(void)
{
	auto uint8_t count = 0;	
	auto uint8_t buf_error = 0;	
	MKP4_channel_status.flags = NO_ERROR; //����� ������ ����������� �������
	
	for(count = 0 ; count < number_check_pins; count++ )//�������� ���� �������
	{
		if ((PORT_ReadInputDataBit(check_pins[count].PORTx, check_pins[count].PORT_Pin )) == SET) //���� �� ����������� ������ ����������� ����������
		{
			MKP4_channel_status.flags |= (1 << count); //���������� ������ ������������ ������ � ����� �����
		}		
	}
	
	if (MKP4_channel_status.flags != NO_ERROR) //���� ���������� ����������� ������ �������
	{
		for (count = 0 ; count < number_check_pins; count++) //����� ������ ������� ������������ ������
		{
			if ((MKP4_channel_status.flags & (0x1 << count)) == (0x1 << count)) //���� �� ����������� ������ ����������� ����������
			{
				buf_error = (count+1);  //���������� ������ ������� ���������� ������������ ������ 
				break; 									//� ����� �� �����
			}
		}
	}	
	
	g_MyDeviceInfo.DeviceState = buf_error;
	
	return MKP4_channel_status.flags;
}

//---------------------------������������� ���� ���������� �����������---------------------------//
static void InitLED_Pin( const TPortPin *PortPin )  
{
  PORT_InitTypeDef port_init_struct;
	RST_CLK_PCLKcmd( PCLK_BIT(PortPin->PORTx), ENABLE ); // Enable peripheral clocks for PORT
	PORT_StructInit( &port_init_struct ); // Configure pin

	port_init_struct.PORT_Pin   = PortPin->PORT_Pin;
	port_init_struct.PORT_OE    = PORT_OE_OUT; //��� �� �����
	port_init_struct.PORT_FUNC  = PORT_FUNC_PORT; //������� ������
	port_init_struct.PORT_MODE  = PORT_MODE_DIGITAL; //�������� ����� ����
	
	port_init_struct.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( PortPin->PORTx, &port_init_struct );
}

//---------------------------������������� ����� ���������� �����������---------------------------//
void InitBiLED( const TBiLED *pBiLed )  
{
	InitLED_Pin( &pBiLed->Red );
}
//-----------------------------------------------------------------------------------------------//
