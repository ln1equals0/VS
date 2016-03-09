// Clickwheel Routines based on http://www.mikrocontroller.net/articles/Drehgeber

#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_exti.h>
#include <misc.h>
#include <util.h>

#include "ff.h"

#include "PGA_Ctrl.h"
#include "DOGM_LCD_LIB/graphics.h"
#include "DOGM_LCD_LIB/driver_dogm128.h"
#include "DOGM_LCD_LIB/aux_routines.h"

#include "rpm_sig_gen.h"

#include "defines.h"

#include "user_if.h"
#include "menu.h"


#define PHASE_A  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)	// ENC1
#define PHASE_B  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)	// ENC2
#define KEY_PIN  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)	// BTN

#define REPEAT_MASK 	0x01
#define REPEAT_START    100
#define REPEAT_NEXT     20

// Defines für Menü & Displayanzeige
#define CH_PER_LINE				16
#define NO_OF_LINES				6
#define NO_OF_PARENTS			1

// MENU ***********************************************************


// User Menus
void HelpMenu(void);
void BacklightON_OFFMenu(void);
void BacklightTimeoutMenu(void);
void TriggerLevelMenu(void);
void TrigHoldOffMenu(void);
void TrigMaskLengthMenu(void);
void SamplingRateMenu(void);
void OutputMenu(void);
void DisplayZoomMenu(void);
void AnalogDisplayMenu(void);
void SH_TimeMenu(void);
void Info_Menu(void);
//Menu tree

//Typical menu with callback params enabled:

// Callback, Callback ID, Menu Text, No of Subitems
const menudata mMenuData[] =   {
                                {menuCnull, 0, "Main Menu", 7},   /*0 - root element*/

                                {Callback1, 1, "FFT Display", 0},   	/*1 - child of Main Menu*/
                                {menuCnull, 2, "System Setup", 3},   	/*2 - child of Main Menu*/
                                {menuCnull, 3, "Sensor Settings", 3},  /*3 - child of Main Menu*/
                                {menuCnull, 4, "ATL Parameters", 2},   /*17 - child of Main Menu*/
                                {Callback1,16, "Logging", 0},   		/*4 - child of Main Menu*/
                                {Callback1, 5, "Status", 0},   			/*4 - child of Main Menu*/
								{Callback1, 6, "Info", 0},   			/*4 - child of Main Menu*/

                                {menuCnull, 7, "Backlight", 3}, 		/*4 - child of System Setup*/
                                {Callback1, 8, "Display Zoom", 0}, 	/*5 - child of System Setup*/
                                {Callback1, 99, "---- back ----", 0},	/* menu up */

                                {menuCnull, 9, "PGA Gain", 3}, 			/*6 - child of Sensor Settings*/
                                {Callback1, 10,"Detection tresh.", 0}, 	/*7 - child of Sensor Settings*/
                                {Callback1, 99,"---- back ----", 0},	/* menu up */

                                {Callback1, 11,"No. of Blades", 0}, 	/*7 - child of ATL Parameters*/
                                {Callback1, 99, "---- back ----", 0},	/* menu up */

                                {Callback1, 12, "Enable / Disable", 0},	/*4 - child of System Setup-Backlight */
                                {Callback1, 13, "Timeout", 0}, 			/*5 - child of System Setup-Backlight */
                                {Callback1, 99, "---- back ----", 0},	/* menu up */

                                {Callback1, 14, "Gain Value", 0}, 		/*8 - child of Sensor Settings-PGA Gain */
                                {Callback1, 15, "AGC on/off", 0},		/*9 - child of Sensor Settings-PGA Gain */
                                {Callback1, 99, "---- back ----", 0}, /* menu up */
                               };

//#define MENU_ENTRIES	20	// Anzahl Menuzeilen OHNE Root!

	uint16_t choose = 0;//mKeyUp;
    uint16_t chars = CH_PER_LINE, lines = NO_OF_LINES, parents = NO_OF_PARENTS;
    uint16_t i = 0;

	char cLns[NO_OF_LINES * (CH_PER_LINE+1)];
	uint8_t visible_lines;

	uint8_t UserMenu = 0, Flag = 1;


void init_Clickwheel(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	int8_t new;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12; // Button, Encoder sw 1, Encoder sw 2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 100 -1;	// 0.1ms
	TIM_TimeBaseStructure.TIM_Prescaler = 71 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 15;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM2 enable counter */
	TIM_Cmd(TIM4, ENABLE);

	// Enable ADC-Trigger Timer Interrupt.
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

}

// Reads the clickwheel, must be placed in an timer Interrupt routine
// 1ms is sufficient for manual turning
void Clickwheel_IRQ (void){

	const int8_t table[16] = {0,0,-1,0,0,0,0,1,1,0,0,0,0,-1,0,0};

	static int8_t last=0;           // alten Wert speichern

    last = (last << 2)  & 0x0F;
    if (PHASE_A) last |=2;
    if (PHASE_B) last |=1;
    enc_delta += table[last];
}

void ENC_Button_IRQ(void){

	  static uint8_t ct0, ct1, rpt;
	  uint8_t i;

	  i = key_state ^ ~KEY_PIN;                       // key changed ?
	  ct0 = ~( ct0 & i );                             // reset or count ct0
	  ct1 = ct0 ^ (ct1 & i);                          // reset or count ct1
	  i &= ct0 & ct1;                                 // count until roll over ?
	  key_state ^= i;                                 // then toggle debounced state
	  key_press |= key_state & i;                     // 0->1: key press detect

	  if( (key_state & REPEAT_MASK) == 0 )            // check repeat function
	     rpt = REPEAT_START;                          // start delay
	  if( --rpt == 0 ){
	    rpt = REPEAT_NEXT;                            // repeat delay
	    key_rpt |= key_state & REPEAT_MASK;
	  }


}


int8_t clickwheel_read4( void )         // read 4 step encoders
{
	  int8_t val;

	  // atomarer Variablenzugriff
	  val = enc_delta;
	  enc_delta = 0;

	  return val;

}

uint8_t get_ENCBTN_press(uint8_t key_mask){
									// read and clear atomic !
	  key_mask &= key_press;        // read key(s)
	  key_press ^= key_mask;        // clear key(s)

	  return key_mask;
}

uint8_t get_ENCBTN_rpt( uint8_t key_mask )
{
							// read and clear atomic !
	  key_mask &= key_rpt;  // read key(s)
	  key_rpt ^= key_mask; // clear key(s)

	  return key_mask;
}

uint8_t get_ENCBTN_state( uint8_t key_mask )
{
  key_mask &= key_state;
  return key_mask;
}

uint8_t get_ENCBTN_short(uint8_t key_mask){
	return get_ENCBTN_press( ~key_state & key_mask );
}

uint8_t get_ENCBTN_long(uint8_t key_mask)
{
	  return get_ENCBTN_press( get_ENCBTN_rpt( key_mask ));
}

void TIM4_IRQHandler(void){

	static uint8_t count=0;

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET){

		if (count++ % 100 == 0) ENC_Button_IRQ();	// 10.0 ms
		Clickwheel_IRQ();							//  0.1 ms
	}

	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}


int8_t ClickWheelState(void){
	// wheelval is a global var updated in systic IRQ

	static int32_t wheelstate_old = 0;
	int8_t wheeldir = 0;

	if (wheel_val > (wheelstate_old + CLICKWHEELSTEPS)) {
		wheeldir =  1;
	}
	if (wheel_val < (wheelstate_old - CLICKWHEELSTEPS)) {
		wheeldir = -1;
	}

	wheelstate_old = wheel_val;

	return wheeldir;
}

void manageMenu(void){

//int8_t ClickState = 0;

ClickState = ClickWheelState();

if (ClickState == 1)
	{
		BacklightON();
		choose = mKeyUp;
		//ClickState = 0;
		//UserMenu = 0;
	}

if (ClickState == -1)
    {
		BacklightON();
		choose = mKeyDown;
		//ClickState = 0;
		//UserMenu = 0;
	}

if( get_ENCBTN_short(0x01) )
	{
		BacklightON();
		Flag = 1;		// Draw User Menu first time
		choose = mKeyOK;
	}

if( get_ENCBTN_long(0x01))
	{
		BacklightON();
		choose = mKeyDown;
		UserMenu = 0;
	}

if (UserMenu) Callback1(UserMenu);	// System is inside of a UserMenu, recall UserMenu routines

//Call the statemachine, it navigates through the tree.
if (choose != 0)
	{
		mStatemachine(choose);

		if (! UserMenu)
		{
			//Get menu text. It is generated using internal pointers
			visible_lines = mGetText(cLns, lines, chars, parents);

			//Print the menu output:
			g_clear();	// LCD löschen
			g_draw_line(0, 9, 102, 0); // hline
			for( i = 0 ; i < visible_lines ; i++)
				{
	 	 	 	if (cLns[i * (chars +1)] != 0x3E) g_draw_string(3, (i*10)+1, &(cLns[i * (chars +1)]), 0); // ">" = 0x3E
	 	 			else
	 	 				{
	 	 		 	 	 g_fill_rect(0, i*10-1, 101, 11);
	 	 		 	 	 g_draw_string(3, (i*10+1), &(cLns[(i * (chars +1)) + 1]), 2); // ">" überspringen
	 	 				}
				}
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		choose = 0;
		}
	}
}


void Callback1(unsigned char param){

    UserMenu = param;

    switch (param)
    {

    	case 1		: 	FFTDisplayMenu();
						break;

    	case 5		: 	StatusMenu();
						break;

		case 6		:   InfoMenu();
						break;

		case 8		:   DisplayZoomMenu();
						break;

		case 10		:   DetectionTrshMenu();
						break;

		case 11		:   NoOfBladesMenu();
						break;

		case 12		:	EnbDisBacklightMenu();
						break;

		case 13		:	BacklightTimeoutMenu();
						break;

		case 14		:	PGAGainMenu();
						break;

		case 15		:	AGCOnOffMenu();
						break;

		case 16		:	LoggingMenu();
						break;

		case 99		:	mStatemachine(mKeyBack);
						UserMenu = 0;
						break;

		default		:	break;
    }

}


void StatusMenu(void)
{
	if (Flag)	// Write only once
	{
		g_clear();	// LCD löschen
    	g_draw_string(3, 1,  "Status Menu", 0); //set_comma(NumBuf);
    	g_draw_line(0, 9, 102, 0); // hline
    	g_draw_string(3, 11,  "Blades:", 0);
    	itoa(bladecnt, NumBuf, 10);
    	g_draw_string(60, 11, NumBuf,0);
    	g_draw_string(3, 21,  "AGC:", 0);
    	if(autogain) g_draw_string(60, 21,  "On", 0);
    	else g_draw_string(60, 21,  "Off", 0);
		g_draw_string(3, 31, "PGA Gain:", 0);
    	itoa(PGA_gain, NumBuf, 10);
    	g_draw_string(60, 31, NumBuf,0);
		g_draw_string(3, 41, "Update:",0);
    	itoa(rpm_updaterate, NumBuf, 10);
    	g_draw_string(60, 41, NumBuf,0);
    	g_draw_string(75, 41, "ms",0);
		//g_draw_string(3, 55, "^ or _ returns",0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 1;
	}
}

void InfoMenu(void)
{
	if (Flag)	// Write only once
	{
		g_clear();	// LCD löschen
    	g_draw_string(2, 1,  "Rob. Bosch GmbH", 0);
    	g_draw_line(0, 9, 102, 0); // hline
    	g_draw_string(2, 11,  "Turbocharger", 0);
    	g_draw_string(2, 21,  "Speed Sensor", 0);
    	g_draw_string(2, 35,  "CR/ARE1 D. Eichel", 0);
		g_draw_string(2, 55,  "FW Version:", 0);
		itoa(FWVERSION, NumBuf, 10);
		set_comma(NumBuf);
		g_draw_string(68, 55, NumBuf,0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}
}

void DisplayZoomMenu(void)
{
	if (Flag)
	{
		g_clear();	// LCD löschen
		g_draw_string(3, 1,  "Display Zoom", 0);
		g_draw_line(0, 9, 128, 0); // hline
		itoa(LCDGain, NumBuf, 10);
		g_draw_string(3, 16,  NumBuf, 0);
		//g_draw_string(3, 32, "wheel changes",0);
		//g_draw_string(3, 40, "long returns",0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState == -1)
	{
		if (LCDGain > 1) LCDGain--;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
		if( ClickState == 1 )
	{
		if (LCDGain < 50) LCDGain++;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
}

void DetectionTrshMenu(void)
{
	if (Flag)
	{
		g_clear();	// LCD löschen
		g_draw_string(3, 1,  "Treshold Level", 0);
		g_draw_line(0, 9, 128, 0); // hline
		itoa(pk_treshold, NumBuf, 10);
		g_draw_string(3, 16,  NumBuf, 0);
		//g_draw_string(27, 16, "mV", 0);
		//g_draw_string(3, 32, "press < > to change",0);
		//g_draw_string(3, 40, "^ or _ returns",0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState == -1)
	{
		if (pk_treshold > PKTRSHSTEP) pk_treshold = pk_treshold - PKTRSHSTEP;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
		if( ClickState == 1 )
	{
		if (pk_treshold < (2^31)-PKTRSHSTEP) pk_treshold = pk_treshold - PKTRSHSTEP;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
}

void NoOfBladesMenu(void)
{
	if (Flag)
	{
		g_clear();	// LCD löschen
		g_draw_string(3, 1,  "No. of Blades", 0);
		g_draw_line(0, 9, 128, 0); // hline
		itoa(bladecnt, NumBuf, 10);
		//set_comma(NumBuf);
		g_draw_string(3, 16,  NumBuf, 0);
		//g_draw_string(33, 16, "ms", 0);
		//g_draw_string(3, 32, "press < > to change",0);
		//g_draw_string(3, 40, "^ or _ returns",0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState == -1)
	{
		if (bladecnt > 9) bladecnt--;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
		if( ClickState == 1 )
	{
		if (bladecnt < 14) bladecnt++;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
}

void EnbDisBacklightMenu(void)
{
	if (Flag)
	{
		g_clear();	// LCD löschen
		g_draw_string(3, 1,  "LCD Backlight Enable", 0);
		g_draw_line(0, 9, 128, 0); // hline
		if (LCDBackLight) g_draw_string(3, 16,  "Enabled", 0);
			else g_draw_string(3, 16,  "Disabled", 0);
		//g_draw_string(3, 32, "press < > to change",0);
		//g_draw_string(3, 40, "^ or _ returns",0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState )
	{
		LCDBackLight ^= 0x01;
		if (LCDBackLight) {
			BacklightON();
		}
		 else {
			   BacklightOFF();
		 }
		Flag = 1;	// Update LCD contents
	}
}

void BacklightTimeoutMenu(void)
{
	if (Flag)
	{
		g_clear();	// LCD löschen
		g_draw_string(2, 1,  "LCD Backlight Timeout", 0);
		g_draw_line(0, 9, 128, 0); // hline
		itoa(LCDBackTimeout, NumBuf, 10);
		g_draw_string(3, 16,  NumBuf, 0);
		g_draw_string(27, 16, "seconds", 0);
		//g_draw_string(3, 32, "press < > to change",0);
		//g_draw_string(3, 40, "^ or _ returns",0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState == -1)
	{
		if (LCDBackTimeout > 10) LCDBackTimeout = LCDBackTimeout - 10;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
		if( ClickState == 1 )
	{
		if (LCDBackTimeout < 240) LCDBackTimeout = LCDBackTimeout + 10;
		BacklightON();
		Flag = 1;	// Update LCD contents
	}
}



void PGAGainMenu(void)
{
	//1,2,5,10,20,50
	if (Flag)
	{
		g_clear();	// LCD löschen
		g_draw_string(3, 1,  "PGA Gain", 0);
		g_draw_line(0, 9, 128, 0); // hline
		itoa(PGA_gain, NumBuf, 10);
		g_draw_string(3, 16,  NumBuf, 0);
		//g_draw_string(27, 16, "ms", 0);
		//g_draw_string(3, 32, "press < > to change",0);
		//g_draw_string(3, 40, "^ or _ returns",0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState == -1)
	{
		switch (PGA_gain){

			case 2:  PGA_gain = 1;
					 ConfPGA(0x11);
					 break;
			case 5:  PGA_gain = 2;
			         ConfPGA(0x22);
					 break;
			case 10:  PGA_gain = 5;
			         ConfPGA(0x33);
					 break;
			case 20: PGA_gain = 10;
			         ConfPGA(0x44);
					 break;
			case 50: PGA_gain = 20;
			         ConfPGA(0x55);
			         break;

			default: break;
		}
		BacklightON();
		Flag = 1;	// Update LCD contents
	}

		if( ClickState == 1 )
		{
			switch (PGA_gain){

				case 1:  PGA_gain = 2;
				         ConfPGA(0x22);
				         break;
				case 2:  PGA_gain = 5;
				         ConfPGA(0x33);
				         break;
				case 5:  PGA_gain = 10;
				         ConfPGA(0x44);
				         break;
				case 10: PGA_gain = 20;
				         ConfPGA(0x55);
				         break;
				case 20: PGA_gain = 50;
				         ConfPGA(0x66);
				         break;

				default: break;
			}
			BacklightON();
			Flag = 1;	// Update LCD contents
		}
}


void AGCOnOffMenu(void)
{
	if (Flag)
	{
		g_clear();	// LCD löschen
		g_draw_string(3, 1,  "AGC on/off", 0);
		g_draw_line(0, 9, 128, 0); // hline
		if (autogain) g_draw_string(3, 16,  "Enabled", 0);
			else g_draw_string(3, 16,  "Disabled", 0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState )
	{
		autogain ^= 0x01;
		if (LCDBackLight) {
			BacklightON();
		}
		 else {
			 BacklightOFF();
		 }
		Flag = 1;	// Update LCD contents
	}
}



void FFTDisplayMenu(void)
{
	static unsigned char trig_old = 0, meanLSBs, wheelstate;
	u32 speedbuf;
	static uint8_t LCDGain = 18;

	if (speedbuf != (u32)((peakIdcsList[0]*samplerate/FFT_LENGTH/2))) Flag = 1;
	speedbuf = (u32)((peakIdcsList[0]*samplerate/FFT_LENGTH/2));

	if (Flag)
	{
		g_clear();	// LCD löschen
		DrawData(LCDGain); // Messdaten in LCD-Speicher schreiben
		g_draw_string(3, 1,  "RPM Signal", 0);
		g_draw_line(0, 9, 102, 0); // hline
		g_draw_string(3, 46, "Speed:", 0);

		if(out_of_range == 0){
			//itoa((u32)((peakIdcsList[0]*samplerate/FFT_LENGTH/2)/bladecnt*60), NumBuf, 10);
			itoa((u32) wheelfreq , NumBuf, 10);
			g_draw_string(46, 46,  NumBuf, 0);
			g_draw_string(85, 46,  "RPM", 0);
			//itoa((u32)((peakIdcsList[0]*samplerate/FFT_LENGTH/2)), NumBuf, 10);
			itoa((u32) wheelfreq/60*bladecnt , NumBuf, 10);
			g_draw_string(46, 55,  NumBuf, 0);
			g_draw_string(85, 55,  "Hz", 0);
			} else {
				g_draw_string(40, 46,  "error", 0);
				// g_draw_string(40, 55,  "underspeed", 0);
			}
	 }
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;

	if( ClickState == -1){
		if (LCDGain > 2) LCDGain--;
		BacklightON();
		Flag = 1;	// Update LCD contents
		wheelstate = 0;
	}

	if( ClickState == 1 ){
		if (LCDGain < 27) LCDGain++;
		BacklightON();
		Flag = 1;	// Update LCD contents
		wheelstate = 0;
	}

}

void LoggingMenu(void)
{
	if (Flag)
	{
		g_clear();	// LCD loeschen
		g_draw_string(3, 1,  "Logging on/off", 0);
		g_draw_line(0, 9, 128, 0); // hline
		if (logging) g_draw_string(3, 16,  "Enabled", 0);
			else g_draw_string(3, 16,  "Disabled", 0);
		disp_frame_end(); // Bildschirmausgabe an LCD senden
		Flag = 0;
	}

	if( ClickState )
	{
		logging ^= 0x01;

		if(logging){
		    /* Open or create a log file and ready to append */
		    f_mount(&fs, "", 0);
		    fr = open_append(&fil, "logfile.txt");
		    if (fr != FR_OK) return 1;
		} else f_close(&fil);

		if (LCDBackLight) {
			BacklightON();
		}
		 else {
			 BacklightOFF();
		 }
		Flag = 1;	// Update LCD contents
	}
}
