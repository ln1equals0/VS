#ifndef AUX_ROUTINES_H_
#define AUX_ROUTINES_H_

#include <inttypes.h>
#include "defines.h"

typedef uint32_t  u32;

#ifdef USE32BITFFT
extern u32 fft_output1[FFT_LENGTH];
extern u32 fft_output2[FFT_LENGTH];
#else
extern u16 fft_output1[FFT_LENGTH];
extern u16 fft_output2[FFT_LENGTH];
#endif

extern u32 temp_TresholdVek[FFT_LENGTH2];

extern unsigned char DrawFlag2, LCDGain, mode, oldmode, LCDBackLight, LCDBackTimeout;
extern volatile unsigned char TxENB;

void DrawData (uint8_t gain);
unsigned int Calc_mV (unsigned int ADLevel);
unsigned int Calc_ms(unsigned char timesteps, unsigned char samplerate);
unsigned int Calc_us(unsigned int ADCCLK_Cycles);
void set_comma(char *buf);

void strreverse(char* begin, char* end);
//void itoa(int value, char* str, int base);
void BacklightON(void);

#endif /*AUX_ROUTINES_H_*/
