#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"



#include "dac.h"
#include "defines.h"

void initDAC_1(void){
	  /* Preconfiguration before using DAC----------------------------------------*/
	  GPIO_InitTypeDef GPIO_InitStructure;
	  DAC_InitTypeDef  DAC_InitStructure;

	  /* DMA1 clock and GPIOA clock enable (to be used with DAC) */
	  //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOA, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	  /* DAC Periph clock enable */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	  /* DAC channel 1 & 2 (DAC_OUT1 = PA.4)(DAC_OUT2 = PA.5) configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  /* DAC channel1 Configuration */
	  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None ;
	  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	  /* DAC channel2 Configuration */
	  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None ;
	  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	  DAC_Init(DAC_Channel_2, &DAC_InitStructure);


	  /* Enable DAC Channel1 */
	  DAC_Cmd(DAC_Channel_1, ENABLE);
	  DAC_Cmd(DAC_Channel_2, ENABLE);

	  /* Set Value to zero */
	  DAC_SetChannel1Data(DAC_Align_12b_R, 2048);
	  DAC_SetChannel2Data(DAC_Align_12b_R, 2048);
}




void rpm2DAC(uint32_t rotation_speed){
	uint32_t value;
	static uint32_t rotbuf = 0;

	if ((rotation_speed != rotbuf) && (no_of_peaks > 0)){
		if(samplerate == SAMPLERATE_NORM){
			value = (uint32_t) ((MAXBITS * rotation_speed) / MAX_SPEED);
		}
		else {
			value = (uint32_t) ((MAXBITS * rotation_speed) / MAX_SPEED * OVERSAMPLING);
		}

		DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t)value);
		debug = value;
	}

	rotbuf = rotation_speed;
}
