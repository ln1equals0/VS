/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stm32f4xx_tim.h>
#include <stm32f4xx_exti.h>
#include "stm32f4xx_dac.h"
#include <misc.h>

#include "util.h"
#include "BGT24_Ctrl.h"
#include "PGA_Ctrl.h"
#include "defines.h"




void initBGT24(void){

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

		TIM_DeInit(TIM3);

		GPIO_InitTypeDef GPIO_InitStructure;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // BGT24 Divider output
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOB, &GPIO_InitStructure);


		/* Connect TIM pin to AF3 */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; // Testing only
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStructure);


		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

		TIM_TimeBaseStructure.TIM_Period = 0xFFFF;	//
		TIM_TimeBaseStructure.TIM_Prescaler = 0;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		TIM_ICInitTypeDef TIM_ICInitStructure;
		TIM_UpdateRequestConfig(TIM3,TIM_UpdateSource_Regular);

		TIM_ICInitStructure.TIM_Channel     = TIM_Channel_3;
		TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Falling;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
		TIM_ICInitStructure.TIM_ICFilter    = 0x0;
		TIM_ICInit(TIM3, &TIM_ICInitStructure);

		NVIC_InitTypeDef NVIC_InitStructure;

		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_Init(&NVIC_InitStructure);

		// Enable Timer Capture Compare Interrupt.
		TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);

		/* TIM3 enable counter */
		TIM_Cmd(TIM3, ENABLE);

}

void TIM3_IRQHandler(void){

	if(TIM_GetITStatus(TIM3, TIM_IT_CC3) == SET)
	  {
	    /* Clear TIM1 Capture compare interrupt pending bit */
	    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
	    if(CaptureNumber == 0)
	    {
	      /* Get the Input Capture value */
	      IC3ReadValue1 = TIM_GetCapture3(TIM3);
	      CaptureNumber = 1;
	    }
	    else if(CaptureNumber == 1)
	    {
	      /* Get the Input Capture value */
	      IC3ReadValue2 = TIM_GetCapture3(TIM3);

	      /* Capture computation */
	      if (IC3ReadValue2 > IC3ReadValue1)
	      {
	        Capture = (IC3ReadValue2 - IC3ReadValue1);
	      }
	      else if (IC3ReadValue2 < IC3ReadValue1)
	      {
	        Capture = ((0xFFFF - IC3ReadValue1) + IC3ReadValue2);
	      }
	      else
	      {
	        Capture = 0;
	      }
	      /* Frequency computation */
	      TIM3Freq = (uint32_t) SystemCoreClock / 2 / Capture * 8;  // /2 -> TIM3 clock = SystemCoreClock/2
	      	  	  	  	  	  	  	  	  	  	  	  	  	  	  	// *8 -> TIM_ICPSC_DIV8
	      CaptureNumber = 0;
	    }

	    // BGT24 soft PLL
	    if(TIM3Freq > DIVRADARFREQ) VCO_tune(-1);
	    else VCO_tune(1);



	  }


}


// Configuration of Infineon 24GHz ASIC BGT24
// Configuration word
// Bit 0..2 			TX power
// Bit 3,4				LO buffer power
// Bit 7,8,11			AMUX
// Bit 12				Disable PA
// Bit 5,6,9,10,13..15	program to zero
void ConfBGT24(uint8_t txpwr, uint8_t LOpwr, uint8_t aMUX, uint8_t enbPA){
	uint16_t conf_word;

	conf_word = 0;

	conf_word |=   (txpwr & 0x07) | (LOpwr & 0x03) << 3 | (aMUX & 0x03) << 7 | (aMUX & 0x04) << 11
				 | (enbPA & 0x01) << 12;

	GPIO_ResetBits(GPIOB, GPIO_Pin_4);		// CS  - low
	DelayUs(1);

	spi_out_nedge((uint8_t)conf_word);
	conf_word = conf_word >> 8;
	spi_out_nedge((uint8_t)conf_word);

	GPIO_SetBits(GPIOB, GPIO_Pin_4);		// CS  - high
	DelayUs(1);

}

void VCO_tune(int8_t updwn){
	static uint32_t dacvalue = 0x007FF7FF;
	uint16_t coarse, fine;

	if(updwn == 1 && dacvalue < 0x00FFFFFF) dacvalue++;
	else if(dacvalue > 0) dacvalue--;


	fine = dacvalue & 0x0FFF;
	coarse = (dacvalue >> 12) & 0xFFF;

	DAC_SetChannel1Data(DAC_Align_12b_R, coarse);
	DAC_SetChannel2Data(DAC_Align_12b_R, fine);

}



