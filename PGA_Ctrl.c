/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "util.h"
#include "PGA_Ctrl.h"





void SoftSPI_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_4);	// CS_BGT24 - high
	GPIO_SetBits(GPIOB, GPIO_Pin_5);	// CS_PGA   - high
	GPIO_SetBits(GPIOB, GPIO_Pin_6);	// SCK      - high
	GPIO_SetBits(GPIOB, GPIO_Pin_7);	// MOSI     - high

}

void spi_out_pedge( unsigned char val )
{

	unsigned char i;

	GPIO_SetBits(GPIOB, GPIO_Pin_6);			// SCK  - high
	DelayUs(1);

  for( i = 8; i; i-- ){
	  GPIO_ResetBits(GPIOB, GPIO_Pin_6);		// SCK  - low
	  if( val & 0x80 )
		  GPIO_SetBits(GPIOB, GPIO_Pin_7);		// MOSI - high DOUT = 1
	  else GPIO_ResetBits(GPIOB, GPIO_Pin_7);	// MOSI - low  DOUT = 0
	  val <<= 1;
	  DelayUs(1);
	  GPIO_SetBits(GPIOB, GPIO_Pin_6);			// SCK  - high
	  DelayUs(1);
  }
}

void spi_out_nedge( unsigned char val )
{

	unsigned char i;

	GPIO_SetBits(GPIOB, GPIO_Pin_6);		// SCK  - High

  for( i = 8; i; i-- ){
	  if( val & 0x80 )
		  GPIO_SetBits(GPIOB, GPIO_Pin_7);		// MOSI - high DOUT = 1
	  else GPIO_ResetBits(GPIOB, GPIO_Pin_7);	// MOSI - low  DOUT = 0
	  DelayUs(1);
	  GPIO_ResetBits(GPIOB, GPIO_Pin_6);		// SCK  - low
	  val <<= 1;
	  DelayUs(1);
	  GPIO_SetBits(GPIOB, GPIO_Pin_6);		// SCK  - High
	  DelayUs(1);
  }
}


void ConfPGA(uint8_t conf_byte){
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);		// CS  - low
	DelayUs(1);

	spi_out_pedge((uint8_t)conf_byte);
	DelayUs(1);
	spi_out_pedge((uint8_t)conf_byte);

	GPIO_SetBits(GPIOB, GPIO_Pin_5);		// CS  - high
	DelayUs(1);
}
