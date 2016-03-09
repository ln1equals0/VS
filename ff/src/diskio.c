/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "driver_dogm128.h"
#include "diskio.h"		/* FatFs lower layer API */
#include "util.h"

unsigned char sdcCommand[6];

void sdc_assert(void)
{
	//GPIOC->ODR &= ~GPIO_Pin_4;
	GPIO_ResetBits(GPIOB, GPIO_Pin_13); // SD-Card CS
}

void sdc_deassert(void)
{
	//GPIOC->ODR |= GPIO_Pin_4;
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
	SPI_send_single(SPI2, 0xFF); // send 8 clocks for SDC to set SDO tristate
}

uint8_t sdc_isConn(void)
{
	//if (GPIOC->IDR & GPIO_Pin_5)
	//if (GPIO_ReadInputData(GPIOB) & GPIO_Pin_14)
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void sdc_sendCommand(uint8_t command, uint8_t par1, uint8_t par2, uint8_t par3, uint8_t par4)
{
	sdcCommand[0] = 0x40 | command;
	sdcCommand[1] = par1;
	sdcCommand[2] = par2;
	sdcCommand[3] = par3;
	sdcCommand[4] = par4;
	
	if (command == SDC_GO_IDLE_STATE)
	{
		sdcCommand[5] = 0x95; // precalculated CRC
	}
	else if (command == SEND_IF_COND){
		sdcCommand[5] = 0x87; // precalculated CRC
	}
	else 
	{
		sdcCommand[5] = 0xFF;
	}
	SPI_send(SPI2, sdcCommand, 6);
}

uint8_t sdc_getResponse(uint8_t response)
{
	for(uint8_t n = 0; n < 8; n++)
	{
		if (SPI_receive_single(SPI2) == response)
		{
			return 0;
		}
	}
	return 1;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{	
	uint8_t sd_standard = 0, response;
	uint8_t rec_data[10] = {0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE};
	// initialize chip select line
	// chip select is PB13
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	//GPIOC->ODR |= GPIO_Pin_4;

	// initialize card detect line
	// card detect is PB14
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//SPI_init(SPI2, SPI_BaudRatePrescaler_256);
	SPI_Config(SPI_BaudRatePrescaler_128); // Max clk speed during init. is 400kHz

	// check wether SDC is inserted into socket
	if (!sdc_isConn())
	{
		return STA_NODISK;
	}
	
	// send 10 dummy bytes to wake up SDC
	for (uint8_t i = 0; i < 10; i++)
	{
		SPI_send_single(SPI2, 0xFF);
	}

	// assert SDC
	sdc_assert();
	// software reset the SDC
	sdc_sendCommand(SDC_GO_IDLE_STATE, 0, 0, 0, 0);

	if (sdc_getResponse(0x01))
	{
		return STA_NOINIT;
	}
	
	// Check for SDHC
	sdc_sendCommand(SEND_IF_COND, 0, 0, 1, 0xAA);
	if (sdc_getResponse(0x01) == 0){
		SPI_receive(SPI2, rec_data, 4);
		if (rec_data[2]==0x01 && rec_data[3]==0xAA){

			uint8_t resp = 1;
			while(resp)
			{
				sdc_sendCommand(SDC_APP_CMD, 0, 0, 0, 0); // SD specific
				sdc_getResponse(0x00); // read answer but do not evaluate
				sdc_sendCommand(SDC_APP_SEND_OP_COND, 64, 0, 0, 0);
				if ( sdc_getResponse(0x00) == 0){
					resp = 0;
					SPI_receive(SPI2, rec_data, 3); // read answer but do not evaluate

					do{
						sdc_sendCommand(SDC_SET_BLOCKLEN, 0, 0, 2, 0); // Blocksize set to 512
					} while (sdc_getResponse(0x00) != 0);
				}
			}
		}
	} else { // SD Ver. 1
			sdc_sendCommand(SDC_APP_CMD, 0, 0, 0, 0); // SD specific
			sdc_getResponse(0x00); // read answer but do not evaluate
			sdc_sendCommand(SDC_APP_SEND_OP_COND, 0, 0, 0, 0);
			sdc_getResponse(0x00); // read answer but do not evaluate

			do{
				sdc_sendCommand(SDC_SET_BLOCKLEN, 0, 0, 2, 0); // Blocksize set to 512
			} while (sdc_getResponse(0x00) != 0);
		}

	sdc_deassert();

	SPI_Config(SPI_BaudRatePrescaler_8);
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	unsigned char result[2];

	if (!sdc_isConn())
	{
		return STA_NODISK;
	}
	/*
	sdc_assert();
	sdc_sendCommand(SDC_SEND_STATUS, 0, 0, 0, 0);
	SPI_receive(SPI2, result, 2);
	sdc_deassert();
	
	if (result[0] & 0x01)
	{
		return STA_NOINIT; 
	}
	else if (result[1] & 0x01)
	{
		return STA_PROTECT;
	}
	*/
	
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	unsigned char result;
	unsigned char buf[2];
	unsigned char resp;
	
	// check if parameters are in valid range
	if( count > 128 )
	{
		return RES_PARERR;
	}
	
	sector *= 512; // convert LBA to physical address
	sdc_assert(); // assert SDC
	
	// if multiple sectors are to be read
	if(count > 1)
	{
		// start multiple sector read
		sdc_sendCommand(SDC_READ_MULTIPLE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for command acknowledgement
		
		while(count)
		{
			while(sdc_getResponse(0xFE)); // wait for data token 0xFE
			SPI_receive(SPI2, buff, 512); // read 512 bytes
			SPI_receive(SPI2, buf, 2); // receive two byte CRC
			count--;
			buff += 512;
		}
		// stop multiple sector read
		sdc_sendCommand(SDC_STOP_TRANSMISSION, 0, 0, 0, 0);
		while(sdc_getResponse(0x00)); // wait for R1 response
	}
	else // if single sector is to be read
	{
		sdc_sendCommand(SDC_READ_SINGLE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for command acknowledgement
		while(sdc_getResponse(0xFE)); // wait for data token 0xFE
		SPI_receive(SPI2, buff, 512); // receive data
		SPI_receive(SPI2, buf, 2); // receive two byte CRC
	}
	
	while(!SPI_receive_single(SPI2)); // wait until card is not busy anymore
	
	sdc_deassert(); // deassert SDC 
	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE* buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	unsigned char result;
	unsigned char buf[2];
	buf[0] = 0xFF;
	buf[1] = 0xFF;
	
	// check if parameters are in valid range
	if( count > 128 )
	{
		return RES_PARERR;
	}
	
	sector *= 512; // convert LBA to physical address
	sdc_assert(); // assert SDC
	
	// if multiple sectors are to be written
	if(count > 1)
	{
		// start multiple sector write
		sdc_sendCommand(SDC_WRITE_MULTIPLE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for R1 response
		SPI_send_single(SPI2, 0xFF);  // send one byte gap
		
		while(count--)
		{
			SPI_send_single(SPI2, 0xFC); // send multi byte data token 0xFC
			SPI_send(SPI2, (unsigned char*)buff, 512); // send 512 bytes
			SPI_send(SPI2, buf, 2); // send two byte CRC
			
			// check if card has accepted data
			result = SPI_receive_single(SPI2);
			if( (result & 0x1F) != 0x05)
			{
				return RES_ERROR;
			}
			count--;
			buff += 512;
			while(!SPI_receive_single(SPI2)); // wait until SD card is ready
		}
		
		SPI_send_single(SPI2, 0xFD); // send stop transmission data token 0xFD
		SPI_send_single(SPI2, 0xFF);  // send one byte gap
	}
	else // if single sector is to be written
	{
		sdc_sendCommand(SDC_WRITE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for R1 response
		SPI_send_single(SPI2, 0xFF);  // send one byte gap
		SPI_send_single(SPI2, 0xFE); // send data token 0xFE
		SPI_send(SPI2, (unsigned char*)buff, 512); // send data
		SPI_send(SPI2, buf, 2); // send two byte CRC
		// check if card has accepted data
		result = SPI_receive_single(SPI2);
		if( (result & 0x1F) != 0x05)
		{
			return RES_ERROR;
		}
	}
	
	while(!SPI_receive_single(SPI2)); // wait until card is not busy anymore
	
	sdc_deassert(); // deassert SDC 
	
	return RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;
	
	return RES_OK;
}
#endif

DWORD get_fattime (void)
{
	return 0;
}
