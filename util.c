#include "stm32f4xx.h"

#include <string.h>
#include "util.h"

extern uint32_t SystemCoreClock;

extern volatile unsigned long Timing_Delay;

void delay_ms(unsigned int delay){

	int i;
	for(i=0;i<delay;i++) DelayUs(1000);

//  Alternative:
//  Be careful Systic MUST be always on
//	Timing_Delay = delay;
//	while (Timing_Delay != 0);	// Decrement done in Systick IRQ
}


/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

/**
* @brief Inserts a delay time in uS.
* @param delay_us: specifies the delay time in micro second.
* @retval None
*/
void DelayUs(uint32_t delay_us)
{
uint32_t nb_loop;

nb_loop = (((168000000 / 1000000)/4)*delay_us)+1; /* uS (divide by 4 because each loop take about 4 cycles including nop +1 is here to avoid delay of 0 */
__asm volatile(
"1: " "\n\t"
" nop " "\n\t"
" subs.w %0, %0, #1 " "\n\t"
" bne 1b " "\n\t"
: "=r" (nb_loop)
: "0"(nb_loop)
: "r3"
);
}

/*
******************************************************************************

From http://forums.arm.com/index.php?showtopic=13949

volatile unsigned int *DWT_CYCCNT   = (volatile unsigned int *)0xE0001004; //address of the register
volatile unsigned int *DWT_CONTROL  = (volatile unsigned int *)0xE0001000; //address of the register
volatile unsigned int *SCB_DEMCR    = (volatile unsigned int *)0xE000EDFC; //address of the register

******************************************************************************

void EnableTiming(void)
{
    static int enabled = 0;

    if (!enabled)
    {
        *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
        *DWT_CYCCNT = 0; // reset the counter
        *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter

        enabled = 1;
    }
}

******************************************************************************

void TimingDelay(unsigned int tick)
{
    unsigned int start, current;

    start = *DWT_CYCCNT;

    do
    {
        current = *DWT_CYCCNT;
    } while((current - start) < tick);
}

******************************************************************************/


