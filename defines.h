#ifndef __DEFINES_H
#define __DEFINES_H

#define BOARD_REV	3				// Board Revision 3 (green PCB with BluetoothLE)
									// Board Revision 2 (blue PCB with LCD)
									// Revision 1 is green PCB without LCD

#define FWVERSION   10				// Firmware Version

#define DIVRADARFREQ 	22889		// 24 GHz (f-Radar/8/2^16)

#define CLICKWHEELSTEPS 2			// Steps to turn to switch to the next menuentry
#define LCDBACKLTIMEOUT 30			// Timout in sec. for LCD backlight
#define BACKLIGHTINITSTATE 1		// Backlight state after RESET 1-on 0-off

#define LOGGING 0					// Enable SD Card logging (0/1 off, on)

#define BLADECNT	12				// Number of Compresor wheel blades
#define PK_TRSH		1000000		// Treshold for FFT peak detection. Treshold max. value 2^32 ~4e9

#define PKTRSHSTEP  10000			// Clickwheel step for PK_TRSH changes via UI

#define NORMAL_MODE 140				// Number of timer ticks for 300kHz Samplerate
#define LAB_MODE	14000			// 4k Hz Number of timer ticks for  1kHz Samplerate

#define SAMPLERATE_LAB	30000		// Samplerate Lab
#define SAMPLERATE_NORM 3000		// 3k Hz Samplerate norm

#define PGAGAIN 	2				// Initial gain of programmable gain amplifier
#define AUTOGAIN	1               // 1-Enable gain autotune 0-disable
#define UPDATERATE 	3000			// RPM updaterate for USB text output in ms (via virtual COM-Port to PC)
#define RPM_UPDATERATE 20			// RPM updaterate for speed signal in ms

#define MATLAB_OUTPUT				// Enhanced output data for Matlab analysis
#define FFT_LENGTH 1024             // FFT_LENGTH = No of FFT points allowed 64, 256, 1024
#define FFT_LENGTH2 FFT_LENGTH << 1             // FFT_LENGTH = No of FFT points allowed 64, 256, 1024
#define FFT_LENGTH4 FFT_LENGTH << 2            // FFT_LENGTH = No of FFT points allowed 64, 256, 1024

#define OVERSAMPLING	1			// only valid for labmode

//#if OVERSAMPLING == 10
//#if FFT_LENGTH == 1024
//#undef MATLAB_OUTPUT			  	// due to RAM limit of µC
//#endif
//#endif

#define ADC_LSB	4095				// 12 Bit ADC
#define MAXPEAK	20					// Maximum number of output values from peak detector

#define ADC_VREF    	 3300  		// 3.3 * 1000 --> 3,3V

// DAC Setting for voltmeter output of RPM signal
#define DAC_VREF		33			// 3,3V
#define DAC_MAXVOLT		30			// 3,0V
#define DAC_LSB			4096		// 12Bit DAC

#define MAX_SPEED 100000			// Maximum speed for DAC_MAXVOLT

#define MAXBITS 	( DAC_MAXVOLT * 4096) / DAC_VREF	// Max. Number of LSB for DAC output

#endif	//__DEFINES_H
