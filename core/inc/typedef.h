#ifndef __TYPEDEF_H
#define __TYPEDEF_H

// Includes ------------------------------------------------------------------------//
#include "main.h"

// Exported types -----------------------------------------------------------------//
#pragma anon_unions(1)
#pragma pack(1)

typedef union 							// информация о приборе для передачи в ЦХП 
{
	struct
	{
		uint8_t	DeviceState;		// Код состояния прибора.
		uint8_t	DeviceType;			// Код типа прибора. У МКП4 равен ???????.
	};

	uint16_t Value;

}TMyDeviceInfo;

extern TMyDeviceInfo g_MyDeviceInfo;	

typedef enum  
{
	NO_ERROR = 0x0,
  CH1_27V_ERROR 	= 0x001,
	CH2_27V_ERROR 	= 0x002,
	CH3_27V_ERROR 	= 0x004,
  CH4_27V_ERROR 	= 0x008,
	CH1_220V_ERROR 	= 0x010,
	CH2_220V_ERROR	= 0x020,
} channel_status;

typedef union
{
	struct
	{
		uint8_t	CH1_27V_flag 		:1;
		uint8_t	CH2_27V_flag 		:1;
		uint8_t	CH3_27V_flag 		:1;
		uint8_t	CH4_27V_flag 		:1;
		uint8_t	CH1_220V_flag 	:1;
		uint8_t	CH2_220V_flag 	:1;
		uint8_t dummy						:2;
	};

	uint8_t flags;
	
} Status_Device;

#pragma pack()
#pragma anon_unions()

//Private defines ------------------------------------------------------------------//
/* DEBUG information */
#ifdef __USE_DBG
#define DBG(...)  printf(__VA_ARGS__)
#else
#define DBG(...)
#endif

//Constants ----------------------------------------------------------------------//


#endif
