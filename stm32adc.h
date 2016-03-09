#include "defines.h"

extern uint16_t ADCBuffer_avg[];

extern uint16_t pulsecnt;
//extern uint16_t sample_interval;
extern uint32_t samplerate;
extern int16_t ADCBuffer[FFT_LENGTH2];
extern int16_t ADCBuffer1[FFT_LENGTH];
extern int16_t ADCBuffer2[FFT_LENGTH];

uint16_t mean_IIR;


unsigned int adc_read(unsigned char channel);
void initadc(int Freq_ind);
void deinitadc(void);
