//#include "arm_math.h"
#include "defines.h"



#ifdef USE32BITFFT
//u32 fft_output1[FFT_LENGTH];
//u32 fft_output2[FFT_LENGTH];
u32 fft_output3[FFT_LENGTH];
#else
u16 fft_output1[FFT_LENGTH];
u16 fft_output2[FFT_LENGTH];
u16 fft_output3[FFT_LENGTH];
#endif

extern u32 peak_list[FFT_LENGTH/2][2];
extern uint16_t treshold;
extern uint32_t samplerate;

extern float peakIdcsList[MAXPEAK];
extern uint16_t bladecnt;

extern uint16_t PGAgainIDX[7];
extern uint16_t PGAConfig_Index[7];
extern uint8_t PGA_gain;
extern uint16_t ADC_min_value;

uint32_t rotSpeed(uint16_t harmonics);
uint32_t rotfreq(uint16_t harmonics);

uint32_t rotSpeedFundamental( void );
uint32_t rotfreqFundamental(void);

u32 first_harmonic_freq(u32* sprectrum, u32 sampling_rate);
u32 fundamental_frequency(u32* sprectrum, u32 sampling_rate);
u32* compute_fft(u32* samples, u16 size);
// void PeakSort(u32 a[][2], u32 samplerate, int n);
void FFT_window(u32* dataIn, int samples, int win_type);
void USE_FullRange(u32* ADCBuffer, uint16_t samples, uint16_t adc_lsb);
//uint16_t removeDC(int total);//int16_t* dataIn);
//void remove_DC_Vib(int total);
void remove_DC_Vib(int16_t *data_in, int total);
void remove_DC_Vib1(int32_t *data_in, int total);
void Auto_Gain_AD(float Freq);

#ifdef USE32BITFFT
void Hanning_Window(int16_t* dataIn, int32_t* dataOut, int samples);
void fft_q31(uint32_t * pSrc, uint32_t * pDest, uint32_t fftSize, uint32_t ifftFlag, uint32_t doBitReverse);
uint16_t Spectrum_Peakdetect(u32* samples, u16 size, u32 treshold);
int32_t DetectFreq(u32* samples, u16 size);
#else
void Hanning_Window(int16_t* dataIn, int16_t* dataOut, int samples);
void fft_q15(uint16_t * pSrc, uint16_t * pDest, uint32_t fftSize, uint32_t ifftFlag, uint32_t doBitReverse);
uint16_t Spectrum_Peakdetect(u16* samples, u16 size, u16 treshold);
int32_t DetectFreq(u32* samples, u16 size);
#endif
