#ifndef USER_IF_H
#define USER_IF_H

#include "ff.h"
#include "defines.h"

#if BOARD_REV == 1 || BOARD_REV == 2
// BLE RESET on PB15
#define BacklightON()  GPIO_SetBits(GPIOB, GPIO_Pin_15); LCDBackTimer = LCDBackTimeout;
#define BacklightOFF() GPIO_ResetBits(GPIOB, GPIO_Pin_15); LCDBackTimer = 0;

#endif

#if BOARD_REV == 3
#define BacklightON()  LCDBackTimer = LCDBackTimeout;
#define BacklightOFF() LCDBackTimer = 0;
#endif

extern uint16_t LCDBackTimer;

void init_Clickwheel(void);
void Clickwheel_IRQ (void);
int8_t clickwheel_read4(void);


// Clickwheel Counter
volatile int8_t enc_delta;          // -128 ... 127
static int8_t last;
int8_t ClickState;

volatile uint8_t key_state;                                // debounced and inverted key state:
                                                  // bit = 1: key pressed
volatile uint8_t key_press;                                // key press detect

volatile uint8_t key_rpt;                                  // key long press and repeat

int8_t ClickWheelState(void);

void ENC_Button_IRQ(void);

uint8_t get_ENCBTN_press(uint8_t key_mask);
uint8_t get_ENCBTN_rpt(uint8_t key_mask);
uint8_t get_ENCBTN_state( uint8_t key_mask );
uint8_t get_ENCBTN_short(uint8_t key_mask);
uint8_t get_ENCBTN_long(uint8_t key_mask);

int32_t wheel_val; // Clickwheel position

char NumBuf[20]; // for itoa etc. function

extern uint32_t pk_treshold;
extern uint16_t  bladecnt;
extern uint8_t PGA_gain;
extern uint16_t autogain;
extern uint16_t rpm_updaterate;
extern uint16_t logging;

// SD-Card logging
FRESULT fr;
FATFS fs;
FIL fil;

extern uint32_t samplerate;
extern float peakIdcsList[];

void Callback1(unsigned char);

void manageMenu(void);

void FFTDisplayMenu(void);
void StatusMenu(void);
void InfoMenu(void);
void DisplayZoomMenu(void);
void DetectionTrshMenu(void);
void NoOfBladesMenu(void);
void EnbDisBacklightMenu(void);
void BacklightTimeoutMenu(void);
void PGAGainMenu(void);
void AGCOnOffMenu(void);
void LoggingMenu(void);

#define MENU_ENTRIES	21	// Anzahl Menuzeilen OHNE Root!

#endif // USER_IF_H
