#ifndef BGT24_CTRL_H
#define BGT24_CTRL_H


uint16_t IC3ReadValue1, IC3ReadValue2;
uint16_t CaptureNumber;
uint32_t Capture;
uint32_t TIM3Freq;


void initBGT24(void);
void ConfBGT24(uint8_t txpwr, uint8_t LOpwr, uint8_t aMUX, uint8_t enbPA);
void VCO_tune(int8_t updwn);




#endif // BGT24_CTRL_H
