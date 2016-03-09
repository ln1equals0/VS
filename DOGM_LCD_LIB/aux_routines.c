#include "defines.h"
#include "aux_routines.h"
#include "DOGM_LCD_LIB/driver_dogm128.h"
#include "DOGM_LCD_LIB/graphics.h"


// Messdaten an das LCD Display ausgeben
void DrawData (uint8_t gain)
{
	uint16_t i,j,k;
	uint16_t fftbin, trshbin;

// todo: Unabhängig von FFT_LENGTH machen
	g_draw_line(0, 43, 102, 0); // hline

	j=0;
	for (i=0;i<FFT_LENGTH/2;i=i+4){

		fftbin = (uint16_t)(((fft_output1[i] >> 2) + (fft_output1[i+1] >> 2)+(fft_output1[i+2] >> 2) + (fft_output1[i+3] >> 2)) >> gain) ;
		if (fftbin > 32) fftbin = 32;
		trshbin = (uint16_t)(((temp_TresholdVek[i] >> 2) + (temp_TresholdVek[i+1] >> 2)+(temp_TresholdVek[i+2] >> 2) + (temp_TresholdVek[i+3] >> 2)) >> gain) ;
		if (trshbin > 32) trshbin = 32;

		g_draw_pixel(j, 42 - trshbin);

		for (k=0;k<fftbin;k++){
		g_draw_pixel(j, 42 - k);
		}
		j++;
	
	}
}

/*
unsigned int Calc_mV (unsigned int ADLevel)
{
	unsigned long retval;
	retval = (unsigned long)ADLevel;
	retval = (retval * (REFVOLTAGE/ADFULL*1000*100))/100;	// in mV
	return (unsigned int) retval;
}

unsigned int Calc_us(unsigned int ADCCLK_Cycles)
{
	unsigned long retval;
	
	retval = (unsigned long) ADCCLK_Cycles * 1/(24e6/8)*1e6; // in �seconds
	
	return (unsigned int) retval;
}

unsigned int Calc_ms(unsigned char timesteps, unsigned char samplerate) // Samplerate in kSamples/s
{
	unsigned int var1, var2;
	var1=(unsigned int)timesteps;
	var2=(unsigned int)samplerate;
		
	return (var1 * 100 / var2);
}
*/

void set_comma(char *buf)
{
		char bufspace[7]={' ',' ',' ',' ',' ',' ',0}, *bufspace_ptr;
		unsigned char length=0, i;
		
		bufspace_ptr = bufspace;
		
		while (*buf)	// Count number of chars in buf
			{
				*bufspace_ptr = *buf; // Copy contents of buf-array to bufspace
				buf++;
				bufspace_ptr++;
				length++;
			}
			
		buf = buf - length; 					// reset pointers
		bufspace_ptr = bufspace_ptr - length;	// reset pointers
		
		if (length == 0)
			{
				*buf = '0';
				buf++;
				*buf = '.';
				buf++;
				*buf = '0';
				buf++;
				*buf = '0';
				buf++;
				*buf = 0x00;
			}
			else if (length < 3)
					{
						*buf = '0';
						buf++;
						*buf = '.';
						buf++;
						*buf = *bufspace_ptr;
						buf++;
						bufspace_ptr++;
						*buf = *bufspace_ptr;
						buf++;
						*buf = 0x00;						
					}
					else
						{
							for (i=0;i<length-2;i++)
								{
									*buf = 	*bufspace_ptr;
									buf++;
									bufspace_ptr++;
								}
								*buf = '.';
								buf++;
								*buf = *bufspace_ptr;
								buf++;
								bufspace_ptr++;
								*buf = *bufspace_ptr;
								buf++;
								*buf = 0x00;
						}
}

/*
void BacklightON(void)
{
	if (LCDBackLight)
		{
			LCD_BEL_PORT |= (1<<LCDBEL);
			LCDBacklightTimer = LCDBackTimeout * 40;
		}
}
*/

/**
 * Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C":
 */
/*
void strreverse(char* begin, char* end) {
	
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}


void itoa(int value, char* str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }
	
	// Take care of sign
	if ((sign=value) < 0) value = -value;
	
	// Conversion. Number is reversed.
	do *wstr++ = num[value%base]; while(value/=base);
	
	if(sign<0) *wstr++='-';
	
	*wstr='\0';

	// Reverse string
	strreverse(str,wstr-1);
}
*/
