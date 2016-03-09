#ifndef DAC_H
#define DAC_H

void initDAC_1(void);
void rpm2DAC(uint32_t rotation_speed);

extern uint16_t debug;
extern uint32_t samplerate;
extern int16_t no_of_peaks;


#endif // DAC_H
