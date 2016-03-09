#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_exti.h>
#include "stm32F4xx_fft.h"
#include "stm32f4xx_usart.h"
#include <misc.h>

#include "util.h"
#include "rpm_sig_gen.h"
#include "user_if.h"

#include "defines.h"


void setupRPMdivider(void){

		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
		TIM_TimeBaseStructure.TIM_Period = 100 -1;
		TIM_TimeBaseStructure.TIM_Prescaler = 4 - 1;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

		NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_Init(&NVIC_InitStructure);

		/* TIM2 enable counter */
		TIM_Cmd(TIM5, ENABLE);

		// Enable ADC-Trigger Timer Interrupt.
		TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
}

void setupUART_RPM_out(void){
	 /* USARTx configured as follow:
	        - BaudRate = 9600 baud
	        - Word Length = 8 Bits
	        - One Stop Bit
	        - No parity
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Receive and transmit enabled
	  */
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;

	  /* Enable GPIO clock */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

#if BOARD_REV == 3
	  /* Sync Signal / Reset for UART on BLE 113 Module */
	  /* Configure PC5 in output pushpull mode */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  // Sync Signal low
	  GPIO_ResetBits(GPIOB, GPIO_Pin_14);

	  // Reset Signal high
	  GPIO_SetBits(GPIOB, GPIO_Pin_15);
#endif

	  /* Enable UART clock */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	  /* Connect PXx to USARTx_Tx*/
	  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);

	  /* Connect PXx to USARTx_Rx*/
	  // GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	  /* Connect PXx to USARTx_CTS*/
	  // GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_USART3);

	  /* Connect PXx to USARTx_RTS*/
	  // GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_USART3);

	  /* Configure USART Tx as alternate function  */
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  /* Configure USART Rx as alternate function  */
	  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	  //GPIO_Init(GPIOC, &GPIO_InitStructure);

	  /* Configure USART CTS as alternate function  */
	  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	  //GPIO_Init(GPIOB, &GPIO_InitStructure);

	  /* Configure USART RTS as alternate function  */
	  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	  //GPIO_Init(GPIOB, &GPIO_InitStructure);

	  USART_InitStructure.USART_BaudRate = 115200;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Tx; // only send data

	  /* USART configuration */
	  USART_Init(USART3, &USART_InitStructure);

	  /* Enable USART */
	  USART_Cmd(USART3, ENABLE);

}

// Generate RPM proportional rectangular signal
void TIM5_IRQHandler(void){

	uint32_t tim_reload;
	static uint32_t cnt = 0;
	uint8_t debug_lo, debug_hi;
	uint16_t debug_wf;



	if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
	{
		// AHB-clk/wheelfreq/TIM_Prescaler/2 Teiler durch 2 da bit toggeln einem teilen durch 2 entspricht
		tim_reload = (uint32_t)((84000000/wheelfreq/4))/2;

		if (tim_reload >= 30 && tim_reload < 7000 ) {
			TIM_SetAutoreload(TIM5,  (tim_reload * 10) - 1);
			out_of_range = 0;
		}
			else{
					TIM_SetAutoreload(TIM5,  210000 - 1);	// Otherwise ultra slow/fast -> underspeed
															// Min. frequency  0.30kHz -> 12 Blades   2.500rpm
															// Max. frequency 65.00kHz -> 12 Blades 325.000rpm
															// In under/overspeed a 50Hz rectangular signal is produced
					out_of_range = 1;
				}
		GPIO_ToggleBits(GPIOC,GPIO_Pin_6);	// RPM rectangular output

		if(logging){		// Logfile to SD-Card enabled
		/* Append a line */
		f_printf(&fil, "%u, %u\n", cnt++, wheelfreq);
		}

#if BOARD_REV == 3
		if ( !(cnt++ % 1000) ){	// Only every cnt modulo 100
			// Send RPM speed via UART to BLE
			// Highbyte RPM divided by 10

			// Synchronize UART on BLE
			GPIO_SetBits(GPIOB, GPIO_Pin_14);

			debug_wf = wheelfreq/10;
			USART_SendData(USART3, (uint8_t) ((wheelfreq/10) >> 8) );
			debug_hi = (uint8_t) ((wheelfreq/10) >> 8);
			/* Loop until the end of transmission */
			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){

				}
			// Lowbyte RPM divided by 10
			USART_SendData(USART3, (uint8_t) ((wheelfreq/10)));
			debug_lo = (uint8_t) (wheelfreq/10);

			// GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
		}
#endif



		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}
