#ifndef PGA_CTRL_H
#define PGA_CTRL_H

//void SPI_Config(void);
void SoftSPI_Config(void);
void spi_out_pedge( unsigned char val );
void spi_out_nedge( unsigned char val );
void ConfPGA(uint8_t conf_byte);

extern uint8_t PGA_gain;

#endif // PGA_CTRL_H
