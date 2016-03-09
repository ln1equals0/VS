#include "usb_conf.h"
#include "usb_regs.h"
#include "usb_defines.h"
#include "usb_core.h"


volatile uint32_t packet_sent;

void stm32_serial_init();
void stm32_serial_print(const char* c);
void samserial_setcallback(void (*c)(unsigned char));
void usb_printf(USB_OTG_CORE_HANDLE *pdev, const char * format, ...);




