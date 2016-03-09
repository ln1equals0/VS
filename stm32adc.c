#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_adc.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_exti.h>
#include <misc.h>

#include "defines.h"

#include "stm32adc.h"
extern uint16_t DMA_suspended;

uint32_t Freq_Value[8]={1000,2000,3000,10000,20000,30000,60000};
uint32_t Freq_Timer[8]={42000,21000,14000,4200,2100,1400,700};
int Freq_index;

void TIM2_IRQHandler(void);

//-----------------------------------------------------------------------------
/// Convert a digital value in milivolt
/// \param valueToconvert Value to convert in milivolt
//-----------------------------------------------------------------------------
static unsigned int ConvHex2mV( unsigned int valueToConvert )
{
    unsigned int mask;

    mask = 0xFFF;	// 4096 -> 12Bit ADC

    return( (ADC_VREF * valueToConvert)/mask);
}

unsigned int adc_read(unsigned char channel){
	if((channel>3) || (channel>=4))  return 0;

	return ConvHex2mV(ADCBuffer_avg[channel]);
}

void deinitadc(void){
	DMA_DeInit(DMA2_Stream0);
	DMA_DeInit(DMA2_Stream2);
	TIM_DeInit(TIM2);
	ADC_DeInit();
}

void initadc(int Freq_ind){
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	Freq_index = Freq_ind;
	//SystemInit();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	// APB1 is max. Clock 42MHz
	TIM_TimeBaseStructure.TIM_Period = Freq_Timer[Freq_ind] - 1;//NORMAL_MODE -1;
	TIM_TimeBaseStructure.TIM_Prescaler = 1;	// Timer Tick 42MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);


	/* TIM2 channel2 configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	// Enable ADC-Trigger Timer Interrupt. It's only for debugging
	//TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
	/* TIM1 main Output Enable */
	TIM_CtrlPWMOutputs(TIM2, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);


	/* Enable DMA2 channel0 für I Signal */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_DeInit(DMA2_Stream0);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCBuffer;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x40012308; // CDR_ADDRESS; Packed ADC1, ADC2
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = FFT_LENGTH2; // Count of 16-bit words
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   //I Signal
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);   //Q Signal
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  //| GPIO_Pin_2 | GPIO_Pin_3; // Analog Inputs
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);


    /* ADC1 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    /* ADC2 Init ****************************************************************/
    ADC_Init(ADC2, &ADC_InitStructure);

    /* ADC2 regular channel1 configuration *************************************/

    // Board Revision 2 (with LCD), Board Revision 3 (with BluetoothLE)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);


    ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);


    /* Enable ADC1 DMA */
    //ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);


//     /* Enable ADC2 DMA */
//     ADC_DMACmd(ADC2, ENABLE);
//
//     /* Enable ADC2 */
     ADC_Cmd(ADC2, ENABLE);
	 ADC_SoftwareStartConv(ADC1);


}


// Mittelwertfilter
// http://www.rowetel.com/blog/?p=1245

void TIM2_IRQHandler(void){
	// uint16_t i;
	static uint32_t out32 = 2048;

	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);


	if (pulsecnt >= FFT_LENGTH * OVERSAMPLING * 2) pulsecnt = 0;

	out32 = out32 - (out32>>16) + ADCBuffer[pulsecnt++];
	mean_IIR = (uint16_t) (out32 >> 16);

	mean_IIR = 2047;

	// GPIO_ToggleBits(GPIOC,GPIO_Pin_0);
}

void DMA2_Stream0_IRQHandler(void){
	DMA_InitTypeDef DMA_InitStructure;
	int i;
    // Interrupt service handler for full transfer
    if( DMA_GetITStatus( DMA2_Stream0, DMA_IT_TCIF0 ) != RESET )
    {
    	ADC_Cmd(ADC1, DISABLE);	// Disable DMA otherwise DMA data destroys array during FFT computation
    	ADC_Cmd(ADC2, DISABLE);	// Disable DMA otherwise DMA data destroys array during FFT computation
    	DMA_suspended = 1;
        pulsecnt = 0;

        for(i=0;i<FFT_LENGTH;i++){
        	ADCBuffer1[i] = ADCBuffer[i << 1];
            ADCBuffer2[i] = ADCBuffer[(i << 1) + 1];
            //ADCBuffer1[(i << 1) + 1] = 0;
            //ADCBuffer2[(i << 1) + 1] = 0;
        }
    }

    DMA_ClearFlag( DMA2_Stream0, DMA_IT_TCIF0 );
}


