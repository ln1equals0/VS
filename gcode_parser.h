/*
 G-Code Interpreter
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef GCODE_PARSER_H_4G4XUC96
#define GCODE_PARSER_H_4G4XUC96

#include <inttypes.h>
#include "defines.h"


typedef void (*ReplyFunction)(const char* format,...);

void gcode_init(ReplyFunction replyFunc);
void gcode_update();

int32_t get_int(char chr);
uint32_t get_uint(char chr);
float get_float(char chr);
const char* get_str(char chr);
int has_code(char chr);

extern uint16_t wintype, bladecnt, amplchg;
extern uint16_t sample_interval;
extern uint32_t samplerate;

extern u32 temp_TresholdVek[FFT_LENGTH2];


extern int32_t DeBuf1[FFT_LENGTH2], DeBuf2[FFT_LENGTH2];
extern int32_t DeHannBuf1[FFT_LENGTH2], DeHannBuf2[FFT_LENGTH2];
extern uint16_t dma_enabled;
extern uint16_t flag;
extern uint16_t update_rate;
extern float peakIdcsList[MAXPEAK];
extern int16_t no_of_peaks;
extern uint16_t ADC_max_value;
extern uint16_t DMA_suspended;
extern uint16_t autogain;
extern uint16_t rpm_updaterate;

#ifdef USE32BITFFT
extern uint32_t pk_treshold;
#else
extern uint16_t pk_treshold;
#endif

#ifdef USE32BITFFT
extern u32 fft_output1[FFT_LENGTH];
extern u32 fft_output2[FFT_LENGTH];
#else
extern u16 fft_output1[FFT_LENGTH];
extern u16 fft_output2[FFT_LENGTH];
#endif

#ifdef USE32BITFFT
int32_t HannBuf[FFT_LENGTH * 2];
#else
int16_t HannBuf[FFT_LENGTH * 2];
#endif

#endif /* end of include guard: GCODE_PARSER_H_4G4XUC96 */
