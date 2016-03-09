#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_exti.h>
#include "stm32F4xx_fft.h"
#include "stm32f4xx_usart.h"
#include <misc.h>

#include "util.h"
#include "BLE.h"
#include "user_if.h"

#include "defines.h"
#include "stm32adc.h"

extern float Phasearray[FFT_LENGTH];
float send[20];
int Peak_send[20][2];

void BLE_Init(void){
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
	  NVIC_InitTypeDef NVIC_InitStructure;
	  /* Enable GPIO clock */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);


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


	  /* Enable UART clock */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	  /* Connect PXx to USARTx_Tx*/
	  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);

	  /* Connect PXx to USARTx_Rx*/
	  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

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
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  /* Configure USART CTS as alternate function  */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

//	  /* Configure USART RTS as alternate function  */
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);


	  USART_InitStructure.USART_BaudRate = 38400;
	  /*BLE has pretty limited throughput, and even under the best circumstances,
	   * a 115200 UART stream will far outpace the BLE link. 38400 baud on the UART
	   * link is likely to be as fast as you can really use even with flow control
	   * enabled and perfect data integrity, simply because the BLE connection itself
	   * cannot transmit data faster than that when used in typical connection environments.
	   * If you have a 20ms connection interval and you can only send 20 bytes of payload
	   * data at a time, then try not to send more than that to the module over UART.*/
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
	  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // only send data

	  /* USART configuration */
	  USART_Init(USART3, &USART_InitStructure);

	  /* Enable USART */
	  USART_Cmd(USART3, ENABLE);

	  /* Enable USART */
	  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	  USART_ClearFlag(USART3, USART_IT_RXNE);
}
extern int Fir_Filter_Toggle;
uint8_t USART3_Re;
uint8_t Freq_index_send=1;
void USART3_IRQHandler(void)
{
  uint8_t temp;
  /* USART in Receiver mode */
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
     USART3_Re = USART_ReceiveData(USART3);
     if((USART3_Re!=0x40)&&(USART3_Re!=0x80)){
    	 if(USART3_Re==0xA0) {
    		 initadc(0);
    		 Freq_index_send = 0;
    		 Fir_Filter_Toggle = 0;
    	 }
    	 if(USART3_Re==0xA1) {
    		 initadc(2);
    		 Freq_index_send = 1;
    		 Fir_Filter_Toggle = 0;
    	 }
    	 if(USART3_Re==0xA2) {
    		 initadc(3);
    		 Freq_index_send = 2;
    	 }
    	 if(USART3_Re==0xA3) {
    		 initadc(5);
    		 Freq_index_send = 3;
    	 }
    	 if(USART3_Re==0xA4) {
    		 initadc(6);
    		 Freq_index_send = 4;
    	 }
    	 if(USART3_Re==0xB1) {
			 if(Freq_index_send >=2) Fir_Filter_Toggle = 1;
			 else Fir_Filter_Toggle = 0;
		 }
    	 if(USART3_Re==0xB0) {
			 Fir_Filter_Toggle = 0;
		 }
     }
  }
  USART_ClearFlag(USART3, USART_IT_RXNE);
}


uint8_t send1[20];// = {0xFF, 0xA2, 0x02, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xDD, 0xEE,
				  //   0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44};
//uint8_t send2[20] = {0xFF, 0xA2, 0x02, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xDD, 0xEE,
//					 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44};
//uint8_t send3[20] = {0xFF, 0xA3, 0x03, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xDD, 0xEE,
//					 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44};
//uint8_t send4[20] = {0xFF, 0xA4, 0x04, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xDD, 0xEE,
//					 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44};
//uint8_t send5[20] = {0xFF, 0xA5, 0x05, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xDD, 0xEE,
//					 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44};
int counnnt = 0;
int counnnt1 = 0;
void BLE_Send(int16_t Freq, u32 Maxpeak, int16_t Ampli)
{
	uint8_t i,j;
	uint8_t temp;

	int start;

//	counnnt = 0;
//	for(i=1;i<=20;i++){
//		send1[0] = 0xFF;
//		send1[1] = 0xA1;
//		send1[2] = (uint8_t)i;
//		start = (i-1)*16;
//		for(j=0;j<15;j++)
//			send1[j+3] = (uint8_t)((float)(fft_output1[start+j])/Maxpeak*254);
//		temp = 0;
//		USART3_Re = 0;
//		counnnt1 = 0;
//		while((USART3_Re != 0x80)&&(counnnt1<3)){
//			temp = 0;
//			counnnt1++;
//			USART3_Re = 0;
//			for(j=0;j<=17;j++){
//				temp = (uint8_t)((temp + send1[j]) & 0xFF);
//				while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//				USART_SendData(USART3, (uint8_t)(send1[j]));
//			}
//			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//			USART_SendData(USART3, (uint8_t)(temp));
//			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//			USART_SendData(USART3, (uint8_t)(send1[19]));
//			counnnt = 0;
//			while(USART3_Re==0){
//				counnnt++;
//				delay_ms(2);
//				if(counnnt>=50) break;
//			}
//		}
//	}
//	for(i=0;i<20;i++) send1[i]=0;
//	send1[0]=0xFF;
//	send1[1]=0xA2;
//	send1[2]=0x01;
//	send1[3]=(uint8_t)(Freq>>8);
//	send1[4]=(uint8_t)(Freq&0xFF);
//	send1[5]=(uint8_t)(Ampli>>8);
//	send1[6]=(uint8_t)(Ampli&0xFF);
//	temp = 0;
//	USART3_Re = 0;
//	counnnt1 = 0;
//	while((USART3_Re != 0x80)&&(counnnt1<3)){
//		temp = 0;
//		counnnt1++;
//		USART3_Re = 0;
//		for(j=0;j<=17;j++){
//			temp = (uint8_t)((temp + send1[j]) & 0xFF);
//			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//			USART_SendData(USART3, (uint8_t)(send1[j]));
//		}
//		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//		USART_SendData(USART3, (uint8_t)(temp));
//		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//		USART_SendData(USART3, (uint8_t)(send1[19]));
//		counnnt = 0;
//		while(USART3_Re==0){
//			counnnt++;
//			delay_ms(2);
//			if(counnnt>=50) break;
//		}
//	}

	GPIO_SetBits(GPIOD, GPIO_Pin_2);
	for(i=0;i<20;i++) send1[i]=0;
	send1[0]=0xFF;
	send1[1]=(uint8_t)(Freq>>8);
	send1[2]=(uint8_t)(Freq&0xFF);
	send1[3]=(uint8_t)(Ampli>>8);
	send1[4]=(uint8_t)(Ampli&0xFF);
	for(i=0;i<6;i++){
		send1[5+i*2]=(uint8_t)(Peak_send[i][0]);
		send1[5+i*2+1]=(uint8_t)Peak_send[i][1];
	}
	send1[17]=(Freq_index_send & 0x0F);
	if(Fir_Filter_Toggle==1) send1[17] = send1[17] + 0x10;
	temp = 0;
	USART3_Re = 0;
	counnnt1 = 0;
	while((USART3_Re != 0x80)&&(counnnt1<3)){
		temp = 0;
		counnnt1++;
		USART3_Re = 0;
		for(j=0;j<=17;j++){
			temp = (uint8_t)((temp + send1[j]) & 0xFF);
			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
			USART_SendData(USART3, (uint8_t)(send1[j]));
		}
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3, (uint8_t)(temp));
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3, (uint8_t)(send1[19]));
		counnnt = 0;
		while(USART3_Re==0){
			counnnt++;
			delay_ms(2);
			if(counnnt>=50) break;
		}
	}
	GPIO_ResetBits(GPIOD, GPIO_Pin_2);
}

