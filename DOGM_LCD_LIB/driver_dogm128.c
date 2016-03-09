
// Display driver for DOGM128
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"
#include <stdint.h> 
#include "util.h"
#include "defines.h"

#include "DOGM_LCD_LIB/driver_dogm128.h"
#include "DOGM_LCD_LIB/graphics.h"

SPI_InitTypeDef  SPI_InitStructure;

uint8_t disp_ram[DISP_WIDTH * (DISP_HEIGHT/8)];

void Init_LCD(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//GPIO_DeInit(GPIOC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // LCD_RST, A0_LCD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// LCD reset
	GPIO_ResetBits(GPIOC, GPIO_Pin_0); // LCD Reset low
	delay_ms(1);
	GPIO_SetBits(GPIOC, GPIO_Pin_0); // LCD Reset high

	GPIO_ResetBits(GPIOC, GPIO_Pin_1);	// A0 default state

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

#if BOARD_REV == 3

	// PB15 is on BLE RESET for BLE Module
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_13; // LCD_SS, LCD backlight, SD-Card ENB
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9 | GPIO_Pin_13); // Backlight on, CS-high, SD-Card CS high
#endif

#if BOARD_REV == 2 || BOARD_REV == 1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_15 | GPIO_Pin_13; // LCD_SS, LCD backlight, SD-Card ENB
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_15 | GPIO_Pin_9 | GPIO_Pin_13); // Backlight on, CS-high, SD-Card CS high

	// In BLE Board used as Sync
	/* SD Card Detect-Pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; // SD Card Detect-Pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;

	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif


	SPI_Config(SPI_BaudRatePrescaler_8);
	disp_init();

}

void SPI_Config(unsigned int prescaler)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the SPI clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2); // MISO
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2); // MOSI
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2); // SCK

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SPI_I2S_DeInit(SPI2);
  // SPI_I2S_DeInit(SPI1);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // SPI_Direction_1Line_Tx;  //
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  /* Initializes the SPI communication */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}


// hardware functions (only used by "disp_..."-functions)

void SPI_MasterTransmit(char cData)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_9); // Display SPI Enable low

	/* Enable the SPI peripheral */

	SPI_Cmd(SPI2, ENABLE);

	while(DMA_suspended == 0);	// No SPI transfers during ADC conversion cycle

	/* Send Transaction data */
	SPI_I2S_SendData(SPI2, cData);

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);

	GPIO_SetBits(GPIOB, GPIO_Pin_9); // Display SPI Enable high

}


void dogm_send(unsigned char spi_data, unsigned char a0)
{
	//a0 = 0: Command
	//a0 = 1: Display data

	//set / clear A0-Bit
	if (a0)
		GPIO_SetBits(GPIOC, GPIO_Pin_1);
	else
		GPIO_ResetBits(GPIOC, GPIO_Pin_1);

	//Byte senden
	SPI_MasterTransmit(spi_data);
}



void dogm_reset( void )
{
	//Reset display (50ms)
	GPIO_ResetBits(GPIOC, GPIO_Pin_0); // RES\ = LOW (Reset)
	delay_ms(10);  // wait 50ms
	GPIO_SetBits(GPIOC, GPIO_Pin_0); // RES\ = HIGH (kein Reset)
}



// basic implementations (used by graphics functions)

void disp_init()
{

	//RESET
	dogm_reset();


	//Display start line
	dogm_send_command(0x40); // Display start line 0

	//Bottom view
	dogm_send_command(0xA1); // ADC reverse
	dogm_send_command(0xC0); // Normal COM0~COM63

	// Disable -> Set All Pixel to ON
	dogm_send_command(0xA4);

	//Normal / Inverted
	dogm_send_command(0xA6); // Display normal

	//Hardware options
	dogm_send_command(0xA2); // Set bias 1/9 (Duty 1/65)
	dogm_send_command(0x2F); // Booster, Regulator and Follower on

	//Contrast options
	dogm_send_command(0x27); // Contrast set
	dogm_send_command(0x81);
	dogm_send_command(0x08);

	// Temperature compensation
	dogm_send_command(0xFA);
	dogm_send_command(0x90);

	//(Init done)
	dogm_send_command(0xAF); // Display on

	disp_clear();
}





void disp_send_frame()
{
	uint8_t page;
	for (page = 0; page < (DISP_HEIGHT/8); page++)
	{
		dogm_send_command(0xB0 + page); //Set page address to <page>
		dogm_send_command(0x10 + 0); //Set column address to 0 (4 MSBs)
		dogm_send_command(0x00 + 0); //Set column address to 0 (4 LSBs)
	
		uint8_t column;
		for (column = 0; column < DISP_WIDTH; column++)
//			dogm_send_display_data(disp_ram[page + (column * 8)]);
			dogm_send_display_data(disp_ram[(page*DISP_WIDTH)+column]);
	}
}

void disp_clear()
{
	//clears the local RAM and send this cleared frame
	uint16_t d;
	for (d = 0; d < (DISP_WIDTH * (DISP_HEIGHT/8)); d++)
		disp_ram[d] = 0x00;
	disp_send_frame();
}

void disp_clear_dont_refresh()
{
	//clears the local RAM but don't send it
	uint16_t d;
	for (d = 0; d < (DISP_WIDTH * (DISP_HEIGHT/8)); d++)
		disp_ram[d] = 0x00;
}


/*
void disp_frame_begin()
{
	//not relevant for this display
}
*/
//ignore "disp_frame_begin()" in source code
#define disp_frame_begin()  


void disp_frame_end()
{
	//send the display data
	disp_send_frame();
}


void disp_set_pixel(uint8_t x, uint8_t y, uint8_t pixel_status)
{
	if (x < DISP_WIDTH && y < DISP_HEIGHT)
		{
		if (pixel_status != 0) disp_ram[(y/8)*DISP_WIDTH + x ] |=  (1 << (y & 0x07));
		    else disp_ram[(y/8)*DISP_WIDTH + x ] &= ~(1 << (y & 0x07));
		}
}


/************************/
/* SD-Card SPI routines */
/************************/


void SPI_send_single(SPI_TypeDef* SPIx, unsigned char data)
{
        unsigned char tmp;
        SPIx->DR = data; // write data to be transmitted to the SPI data register
        while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
        while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
        while( SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
        tmp = SPIx->DR; // return received data from SPI data register
}

unsigned char SPI_receive_single(SPI_TypeDef* SPIx)
{
        SPI2->DR = 0xFF; // write data to be transmitted to the SPI data register
        while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
        while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
        while( SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
        return SPIx->DR; // return received data from SPI data register
}

void SPI_send(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length)
{
        while (length--)
        {
                SPI_send_single(SPIx, *data);
                data++;
        }

}

void SPI_receive(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length)
{
        while (length--)
        {
                *data = SPI_receive_single(SPIx);
                data++;
        }
}

void SPI_transmit(SPI_TypeDef* SPIx, unsigned char* txbuf, unsigned char* rxbuf, unsigned int len)
{
        while (len--)
        {
                SPIx->DR = *txbuf; // write data to be transmitted to the SPI data register
                while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
                while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
                while( SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
                *rxbuf = SPIx->DR; // return received data from SPI data register
                txbuf++;
                rxbuf++;
        }
}
