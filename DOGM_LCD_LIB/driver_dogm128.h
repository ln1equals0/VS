#ifndef DRIVER_DOGM_H
#define DRIVER_DOGM_H

#include "stm32f4xx_spi.h"

// Display driver for DOGM128

// Display properties (for graphics functions)
#define DISP_WIDTH   102L
#define DISP_HEIGHT  64L
#define DISP_DEPTH   1   //monochrome

#define dogm_send_command(command)      dogm_send(command, 0)
#define dogm_send_display_data(data)    dogm_send(data, 1)

extern uint16_t DMA_suspended;


/************************/
/* SD-Card SPI routines */
/************************/

void SPI_send_single(SPI_TypeDef* SPIx, unsigned char data);
unsigned char SPI_receive_single(SPI_TypeDef* SPIx);
void SPI_send(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length);
void SPI_receive(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length);
void SPI_transmit(SPI_TypeDef* SPIx, unsigned char* txbuf, unsigned char* rxbuf, unsigned int len);

/************************/

//unsigned char disp_ram[DISP_WIDTH * (DISP_HEIGHT/8)];

void Init_LCD(void);

void SPI_Config(unsigned int prescaler);

void SPI_MasterTransmit(char cData);

void disp_clear( void );

// hardware functions (only used by "disp_..."-functions)

void dogm_send(unsigned char spi_data, unsigned char a0);

void dogm_reset( void );

// basic implementations (used by graphics functions)

void disp_init( void);

void disp_send_frame( void );

void disp_clear( void );

void disp_clear_dont_refresh( void );


/*
void disp_frame_begin()
{
	//not relevant for this display
}
*/
//ignore "disp_frame_begin()" in source code
#define disp_frame_begin()  


void disp_frame_end( void );


void disp_set_pixel(unsigned char x, unsigned char y, unsigned char pixel_status);

#endif
