/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the GenericHID demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

 
 /**
 18.02.2015
 Отключил клавиатуру и реле
 
 */
#include "GenericHID.h"
#include "oddebug.h"
#include "one_wire.h"
#include "key_matr.h"
#include "iodefs.h"
#include "command.h"

#include <util/delay.h>


#define DDR_IO		DDRD 
#define PORT_IO		PORTD
#define PIN_IO		PIND


#define MASK_IO		(1 << PD7 | 1 << PD6 | 1 << PD5 | 1 << PD4)

#define COMMAND		0
#define DATA		1

void SCAN_MKeyboard(void);
static uint8_t counter = 0;
static uint8_t temp_ow_read_bit = 0;
static uint8_t flag = 0; 
/******************************/
/* bit 0 - antidrebezg */
/* bit 1 - for one wire */
/* bit 2 - for one wire SEARH ROM start -? */
static uint8_t temp_mask = -1;

#define FDR 	0
#define FOW 	1
#define FOWS	2

ISR(TIMER0_OVF_vect)
{
	if(counter > 10)
	{
		TCCR0B = STOP_TIMER;
		counter = 0;
		//flag &= ~(1<<FDR);
		BIT_CLR(flag, FDR);
	}
	else
	{
		counter++;
	}
}

static uint8_t LastCommand = 0x00;

extern uint8_t onewire_enum[8]; // найденный восьмибайтовый адрес 
extern uint8_t onewire_enum_fork_bit; // последний нулевой бит, где была неоднозначность (нумеруя с единицы) 



/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevHIDReportBuffer[GENERIC_REPORT_SIZE];


/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = 0,
				.ReportINEndpoint             =
					{
						.Address              = GENERIC_IN_EPADDR,
						.Size                 = GENERIC_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevHIDReportBuffer),
			},
	};


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
  
 /*
 int main(void)
 {

	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	clock_prescale_set(clock_div_1);
	
	ONEWIRE_PORT &= ~_BV(ONEWIRE_PIN_NUM);
	onewire_high();
	
	odDebugInit();
	DBG1(0x00, 0, 0);
while(1)
{	
	if (onewire_skip()) 
	{
		DBG1(0x01, 0, 0);
		onewire_send(0x44);
		_delay_ms(900);
		onewire_enum_init();
		for(;;)
		{
		uint8_t * p = onewire_enum_next();
			if(!p)
			{
				DBG1(0x50, 0, 0);
				break;
			}

		DBG1(0x02, onewire_enum, 8);

			onewire_send(0xBE); 
			uint8_t scratchpad[8];
			for (uint8_t i = 0; i < 8; i++) 
			{
			   uint8_t b = onewire_read();
			   scratchpad[i] = b;
			}
			
			//int16_t t = (scratchpad[1] << 8) | scratchpad[0];
			
			DBG1(0x03, scratchpad, 8);
			
			//uart_num(t);
			
			uartPutc('\r');
			uartPutc('\n');
			
			_delay_ms(1000);
		}

	}
	else
		DBG1(0x04, 0, 0);
		

}		
		
	while(1)
	{
		asm("NOP");
	}
 }
*/
 

int main(void)
{
	SetupHardware();

	GlobalInterruptEnable();
	
	odDebugInit();
	DBG1(0x00, 0, 0);
	
	for (;;)
	{
		HID_Device_USBTask(&Generic_HID_Interface);
		USB_USBTask();
		//SCAN_MKeyboard(); //Клавиатура
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* One Wire */
	
	OneWireInit();
	
	
	/* IO Init for Relay */
	DDR_IO = MASK_IO;
	PORT_IO |= MASK_IO;
	
	/* Hardware Initialization */
	USB_Init();
	
	/* Key Matr */

	DDR_MATR = SET_DDR;
	
	TIMSK0 = (1<<TOIE0);
	sei(); // прерывания он
	
}

void SCAN_MKeyboard(void)
{

	for(int i = 0; i < MATR_COLUMN; i++)
	{
		DDR_MATR |= (1<<Matr.Column[i]); // Установить низкий уровень на столбце
		
		asm("nop");
		
		for(int j = 0; j < MATR_ROW; j++)
		{
			if(temp_mask == Matr_mask[i][j] && bit_is_set(PIN_MATR, Matr.Row[j]) && bit_is_clear(flag, FDR)) // Если кнопку отпустили
				temp_mask = -1;
			
			if(temp_mask == Matr_mask[i][j]) // Если кнопка нажата
				continue; 				
								
			if(bit_is_clear(PIN_MATR, Matr.Row[j]) && bit_is_clear(flag, FDR)) // Нажатие клавиши
			{
				
				DBG1(0x80, &Matr_mask[i][j], sizeof(uint8_t));
				if(Matr_mask[i][j] == 0xF0)
					PORT_IO |= Matr_mask[i][j];
				
				if(PORT_IO & (~Matr_mask[i][j]))	// Включить погасить
					PORT_IO &= (Matr_mask[i][j] | (~MASK_IO)); 
				else
					PORT_IO |= ((~Matr_mask[i][j]) & (MASK_IO));
				
				
				/*
				if(PORT_IO & Matr_mask[i][j]) // Включить погасить
					PORT_IO &= ~Matr_mask[i][j];
				else
					PORT_IO |= Matr_mask[i][j];
				*/
				
				//LastCommand = PRESS_KEY;
				temp_mask = Matr_mask[i][j]; // Что за кнопка была нажата				
				TCCR0B = START_TIMER;
				//flag |= (1<<FDR); // Анти дребезг
				BIT_SET(flag, FDR);
				//bit_is_set(flag, FDR);
				asm("nop");
			}

		}
		
		DDR_MATR &= ~(1<<Matr.Column[i]); // Вернуть все на место
		asm("nop");
	
	}
	PORTB = 0x00;
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	DBG1(0x01, 0, 0);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	DBG1(0x02, 0, 0);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);

	USB_Device_EnableSOFEvents();

}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Generic_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */

// от устройства 
 
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	
	
	uint8_t* Data        = (uint8_t*)ReportData;
	*ReportSize = GENERIC_REPORT_SIZE;
	
	if(LastCommand == 0)
		return false;
	
	Data[COMMAND] = LastCommand;
	Data[DATA] = 0x00;
	switch(LastCommand)
	{
		
		/* без реле
		case RELE_ON:
		case RELE_OFF:
		case PRESS_KEY:
			Data[DATA] = (~PORT_IO & MASK_IO);
		break;
		*/
		
		case SKIP_ROM:
		case MATCH_ROM:
		case OW_RESET:
			if(bit_is_set(flag, FOW))
				Data[DATA] = 0xFF;
		break;
		
		case SEARCH_ROM:
			if(onewire_enum[0])
			{
				for(uint8_t i = 0; i < 8; i++)
					Data[DATA + i] =  onewire_enum[i];
			}
		break;

		case OW_READ:
			Data[DATA] = temp_ow_read_bit;
			temp_ow_read_bit = 0;
		break;
		
	}
	
	uint8_t* temp = Data;
	DBG1(0x53, temp, sizeof(uint8_t)*2);
	
	LastCommand = 0;
	
	return true;
	
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	// к устройства
	uint8_t* Data       = (uint8_t*)ReportData;
	
	//uint8_t temp = (uint8_t) LastCommand;
	uint8_t temp = 0;
	
	
		DBG1(0x41, &Data[0], sizeof(uint8_t) * 9);
		temp = Data[DATA];
		LastCommand = Data[COMMAND];
		switch(Data[COMMAND])
		{
			/* Без реле
			case RELE_ON:
				//PORT_IO &= ~(Data[DATA] & MASK_IO);
			//break;
			case RELE_OFF:
				PORT_IO ^= (Data[DATA] & MASK_IO);
			break;
			*/
			case OW_READ:
				temp_ow_read_bit = onewire_read();
				temp = temp_ow_read_bit;
				DBG1(0x42, &temp, sizeof(uint8_t));
			break;
			case SKIP_ROM:
				if(onewire_skip())
				{
					BIT_SET(flag, FOW);
				}
				else
				{
					BIT_CLR(flag, FOW);
				}
			break;
			case MATCH_ROM:
			
				if(onewire_match(&Data[DATA]))
				{
					BIT_SET(flag, FOW);
				}
				else
				{
					BIT_CLR(flag, FOW);
				}
				
			break;
			case SEARCH_ROM:
				if(!onewire_enum_next())
					onewire_enum_init();
			break;
			case OW_SEND:
				onewire_send(Data[DATA]);
			break;
			case OW_RESET:
			  if (onewire_reset()) 
				{
					BIT_SET(flag, FOW);
				}
				else
				{
					BIT_CLR(flag, FOW);
				}
			break;
			
		}
	
}

