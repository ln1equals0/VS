#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#include "usb_core.h"
#include "usb_conf.h"
#include "usb_dcd.h"
#include "usbd_conf.h"
#include "usbd_cdc_vcp.h"
#include "usbd_cdc_core.h"

#include "stm32serial.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
extern uint8_t APP_Rx_Buffer[];
extern uint32_t APP_Rx_ptr_in, APP_Rx_ptr_out;
extern volatile unsigned long Timing_Delay;
volatile uint32_t packet_sent = 1;
char printBuffer[256];

void stm32_serial_print(const char* c)
{
	usb_printf(&USB_OTG_dev,c);
}

//------------------------------------------------------------------------------
/// Initializes drivers and start the USB <-> Serial bridge.
//------------------------------------------------------------------------------
void stm32_serial_init()
{

	// Init code

}

void usb_printf(USB_OTG_CORE_HANDLE *pdev, const char * format, ...)
{
	unsigned int str_len = 0, i;
	// uint32_t ptrbuffer;
	u32 delay;

	if (pdev->dev.device_status != USB_OTG_CONFIGURED) return;

	va_list args;
	va_start (args, format);
	str_len = vsprintf (printBuffer,format, args);
	va_end (args);


	for(i=0;i<str_len;i++){
		APP_Rx_Buffer[APP_Rx_ptr_in] = printBuffer[i];
		APP_Rx_ptr_in++;

		// To avoid buffer overflow
		if(APP_Rx_ptr_in == APP_RX_DATA_SIZE - 1)
		  {
			if(APP_Rx_ptr_out == APP_RX_DATA_SIZE) while(APP_Rx_ptr_out != APP_Rx_ptr_in ); // send data already in tx buffer

		  }

		if(APP_Rx_ptr_in == APP_RX_DATA_SIZE){
			delay = 6720000; // Systic countdown Variable
			while((APP_Rx_ptr_out != APP_Rx_ptr_in) && delay-- );
			APP_Rx_ptr_in = 0;

		}


	}

}

