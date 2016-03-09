#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "gcode_parser.h"
#include "stm32serial.h"
#include "PGA_Ctrl.h"
#include "defines.h"
#include "stm32adc.h"
#include "util.h"

#define false 0
#define true !false

extern float Phasearray[FFT_LENGTH];
extern int32_t Vibration_A[FFT_LENGTH];
extern int32_t Freq1,Freq2;
extern int32_t Maxpeak1, Maxpeak2;

#define BUFFER_SIZE 256

#define DEBUG_PARSER


#ifdef DEBUG_PARSER
#define DEBUG(...) printf(__VA_ARGS__);
#else
#define DEBUG(...)
#endif

extern USB_OTG_CORE_HANDLE USB_OTG_dev;

typedef struct 
{
	int readPos;
	int writePos;
	int numBytes;
	uint8_t buffer[BUFFER_SIZE];
} RingBuffer;


void ringbuffer_init(RingBuffer* pBuffer)
{
	memset(pBuffer,0,sizeof(RingBuffer));
}

int ringbuffer_numFree(const RingBuffer* pBuffer)
{
	return BUFFER_SIZE-pBuffer->numBytes;
}

int ringbuffer_numAvailable(const RingBuffer* pBuffer)
{
	return pBuffer->numBytes;
}

uint8_t ringbuffer_get(RingBuffer* pBuffer)
{
	uint8_t b = pBuffer->buffer[pBuffer->readPos++];
	pBuffer->readPos &= (BUFFER_SIZE-1);
	pBuffer->numBytes--;
	return b;
}

void ringbuffer_put(RingBuffer* pBuffer,uint8_t b)
{
	pBuffer->buffer[pBuffer->writePos++] = b;
	pBuffer->writePos &= (BUFFER_SIZE-1);
	pBuffer->numBytes++;
}


//----------------------------------------------------------------------------------------------
static RingBuffer uartBuffer;

static void gcode_datareceived(unsigned char chr)
{
	if (ringbuffer_numFree(&uartBuffer))
		ringbuffer_put(&uartBuffer,chr);


}


typedef struct 
{
	int comment_mode : 1;
	int commandLen;
	uint32_t last_N;
	uint32_t line_N;
	char commandBuffer[BUFFER_SIZE];
	char* parsePos;
	ReplyFunction replyFunc;
} ParserState;


static ParserState parserState;

int32_t get_int(char chr)
{
	char* ptr = strchr(parserState.parsePos,chr);
	return ptr ? strtol(ptr+1,NULL,10) : 0;
}

uint32_t get_uint(char chr)
{
	char* ptr = strchr(parserState.parsePos,chr);
	return ptr ? strtoul(ptr+1,NULL,10) : 0;
}

float get_float(char chr)
{
	char* ptr = strchr(parserState.parsePos,chr);
	return ptr ? strtod(ptr+1,NULL) : 0;
}

uint32_t get_bool(char chr)
{
	return get_int(chr) ? 1 : 0;
}

const char* get_str(char chr)
{
	char *ptr = strchr(parserState.parsePos,chr);
	return ptr ? ptr+1 : NULL;
}

int has_code(char chr)
{
	return strchr(parserState.parsePos,chr) != NULL;
}

static uint8_t get_command()
{
	return parserState.parsePos[0];
}

static char* trim_line(char* line)
{
	int i;
	char command_chars[] = {'G','M','T'};
	
	while(*line)
	{
		for (i=0;i<sizeof(command_chars);i++)
		{
			if (*line == command_chars[i])
				return line;
		}
		line++;
	}
	return line;
}

static uint8_t calculate_checksum(const char* pLine)
{
	uint8_t checksum = 0;
	while(*pLine != '*')
		checksum ^= *pLine++;
		
	return checksum;
}

#define sendReply(...) { if (parserState.replyFunc) { parserState.replyFunc(__VA_ARGS__); } }

enum ProcessReply {
	NO_REPLY,
	SEND_REPLY,
};


#define GET(code,default_value) has_code(code) ? get_int(code) : default_value

extern int Send_Ok;

//process the actual gcode command
static int gcode_process_command()
{
	int i;
	switch(get_command())
	{
		case 'G':
		{
			switch(get_int('G'))
			{
				case 0:
					if (has_code('S')) sample_interval = get_uint('S');

					switch(sample_interval){

					case 0:{
											usb_printf(&USB_OTG_dev, "Labmode (20kHz Samplerate)\n");
											//sample_interval = LAB_MODE;
											samplerate = SAMPLERATE_LAB;
											break;
					}

					case 1:{
											usb_printf(&USB_OTG_dev, "Normal Mode (200kHz Samplerate)\n");
											//sample_interval = NORMAL_MODE;
											samplerate = SAMPLERATE_NORM;
											break;
					}

					default: 			 {
											usb_printf(&USB_OTG_dev, "Mode unknown, using Labmode (20kHz Samplerate)\n");
											//sample_interval = LAB_MODE;
											samplerate = SAMPLERATE_LAB;
											initadc(0);
											break;
					}

					}
					break;
				case 1:
					if (has_code('S')) bladecnt = get_uint('S');
					switch(bladecnt){
					case 7: usb_printf(&USB_OTG_dev, "Bladecount: 7\n");
						break;
					case 8: usb_printf(&USB_OTG_dev, "Bladecount: 8\n");
						break;
					case 9: usb_printf(&USB_OTG_dev, "Bladecount: 9\n");
						break;
					case 10: usb_printf(&USB_OTG_dev, "Bladecount: 10\n");
						break;
					case 11: usb_printf(&USB_OTG_dev, "Bladecount: 11\n");
						break;
					case 12: usb_printf(&USB_OTG_dev, "Bladecount: 12\n");
						break;
					case 13:	usb_printf(&USB_OTG_dev, "Bladecount: 13\n");
						break;
					case 14:	usb_printf(&USB_OTG_dev, "Bladecount: 14\n");
						break;
					default: {
								usb_printf(&USB_OTG_dev, "Bladecount unsupported, using 12 Blades\n");
								if ((bladecnt > 14) || (bladecnt < 7)) bladecnt = 12;
							 }
					}
					break;
				case 2:
					if (has_code('S')) pk_treshold = get_uint('S');
					usb_printf(&USB_OTG_dev, "Treshold for peak detect: %u LSB\n",pk_treshold);
					break;

				case 3:
					if (has_code('S')) PGA_gain = get_uint('S');
					switch(PGA_gain){
					case 0: usb_printf(&USB_OTG_dev, "PGA Gain: 0\n");
							ConfPGA(0x00);
							break;
					case 1: usb_printf(&USB_OTG_dev, "PGA Gain: 1\n");
							ConfPGA(0x11);
							break;
					case 2: usb_printf(&USB_OTG_dev, "PGA Gain: 2\n");
							ConfPGA(0x22);
							break;
					case 5: usb_printf(&USB_OTG_dev, "PGA Gain: 5\n");
							ConfPGA(0x33);
							break;
					case 10: usb_printf(&USB_OTG_dev, "PGA Gain: 10\n");
							ConfPGA(0x44);
							break;
					case 20:	usb_printf(&USB_OTG_dev, "PGA Gain: 20\n");
							ConfPGA(0x55);
							break;
					case 50:	usb_printf(&USB_OTG_dev, "PGA Gain: 50\n");
							ConfPGA(0x66);
							break;
					case 100:	usb_printf(&USB_OTG_dev, "PGA Gain: 100\n");
							ConfPGA(0x77);
							break;
					case 101:	usb_printf(&USB_OTG_dev, "PGA high imp.\n");
							ConfPGA(0x88);
							break;
					default: {
								usb_printf(&USB_OTG_dev, "Gainvalue unsupported, using Gain 1\n");
								ConfPGA(0);
							 }
					}
					break;

				case 4:
					if (has_code('S')) update_rate = get_uint('S');
					if ((update_rate < 100) && (update_rate > 0)) {
						update_rate = 1000;
						usb_printf(&USB_OTG_dev, "Update rate invalid using ");
					}
					usb_printf(&USB_OTG_dev, "USB Update rate: %u ms\n", update_rate);
					break;

				case 5:
					if (has_code('S')) autogain = get_uint('S');
					if (autogain > 1 || autogain < 0) {
						autogain = 0;
						usb_printf(&USB_OTG_dev, "Autogain setting invalid.");
					}
					if(autogain == 1) usb_printf(&USB_OTG_dev, "AGC: enabled\n");
					else usb_printf(&USB_OTG_dev, "AGC: disabled\n");
					break;

				case 6:
					if (has_code('S')) rpm_updaterate = get_uint('S');
					if (rpm_updaterate > 500 || rpm_updaterate < 5) {
						rpm_updaterate = 20;
						usb_printf(&USB_OTG_dev, "Updaterate setting invalid.");
					}

					usb_printf(&USB_OTG_dev, "RPM Updaterate: %u ms\n",rpm_updaterate);
					break;


				case 99:
					usb_printf(&USB_OTG_dev, "HELP\n");
					usb_printf(&USB_OTG_dev, "----------------------------------------\n");
					usb_printf(&USB_OTG_dev, "Set Mode (0-Lab, 1-Norm.:	G0 Sx    x=0, 1\n");
					usb_printf(&USB_OTG_dev, "Set Bladecount:           G1 Sx    x=8..14\n");
					usb_printf(&USB_OTG_dev, "Set Peak Detect Treshold: G2 Sx    x=0..65635\n");
					usb_printf(&USB_OTG_dev, "Set PGA Gain:             G3 Sx    x=0, 1, 2, 5, 10, 20, 50, 100, 101\n");
					usb_printf(&USB_OTG_dev, "Set USB update rate [ms]: G4 Sx    x=100..65635\n");
					usb_printf(&USB_OTG_dev, "Enable/disable AGC        G5 Sx    x=1, 0\n");
					usb_printf(&USB_OTG_dev, "Set RPM Updaterate [ms]:  G6 Sx    x=5..500\n");
					usb_printf(&USB_OTG_dev, "Status:                   G100\n");
					break;

				case 100:
					usb_printf(&USB_OTG_dev, "STATUS  \n");
					usb_printf(&USB_OTG_dev, "----------------------------------------\n");
					usb_printf(&USB_OTG_dev, "Samplerate:           %u Hz\n", samplerate);
					usb_printf(&USB_OTG_dev, "Bladecount:           %u\n", bladecnt);
					usb_printf(&USB_OTG_dev, "Peak Detect Treshold: %u LSB\n",pk_treshold);
					usb_printf(&USB_OTG_dev, "AGC status:           %u\n",autogain);
					usb_printf(&USB_OTG_dev, "PGA Gain:             %u\n", PGA_gain);
					usb_printf(&USB_OTG_dev, "RPM Updaterate:       %u ms\n",rpm_updaterate);
					break;

				default:
					sendReply("Unknown G%d\n\r",get_int('G'));
					return NO_REPLY;
			}
			break;
		}

		case 'M':
		{
			int mcode = get_int('M');

			// Wait that all buffers are filled
			//while( !DMA_suspended );

			switch(mcode)
			{

				case 200:
					while(Send_Ok==0);
					for (i=0;i<FFT_LENGTH2;i++){
						usb_printf(&USB_OTG_dev," %d",DeBuf1[i]);
						if (i != FFT_LENGTH - 1) usb_printf(&USB_OTG_dev,";");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 20:	// fft bins
					//usb_printf(&USB_OTG_dev, "% FFT output\r\n");
					for (i=0;i<FFT_LENGTH/2;i++){
					//	usb_printf(&USB_OTG_dev," %u",fft_output1[i]);
					//	if (i != FFT_LENGTH/2 - 1) usb_printf(&USB_OTG_dev,";");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 21:	// peak detector treshold
					//usb_printf(&USB_OTG_dev, "% Noise treshold\r\n");
					for (i=0;i<FFT_LENGTH/2;i++){
					//	usb_printf(&USB_OTG_dev," %u",temp_TresholdVek[i]);
					//	if (i != FFT_LENGTH/2 - 1) usb_printf(&USB_OTG_dev,";");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 30:	// fft bins
					//usb_printf(&USB_OTG_dev, "% FFT output\r\n");
					for (i=0;i<FFT_LENGTH/2;i++){
					//	usb_printf(&USB_OTG_dev," %u",fft_output2[i]);
					//	if (i != FFT_LENGTH/2 - 1) usb_printf(&USB_OTG_dev,";");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

#ifdef MATLAB_OUTPUT
				case 22:	// ADC Values
					//usb_printf(&USB_OTG_dev, "% ADC Values\r\n");
					for (i=0;i<FFT_LENGTH;i++){
					//	usb_printf(&USB_OTG_dev," %d",DeBuf1[i << 1]);
					//	if (i != FFT_LENGTH - 1) usb_printf(&USB_OTG_dev,";");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					//usb_printf(&USB_OTG_dev,"%c",-1);
					break;

				case 32:	// ADC Values
					//usb_printf(&USB_OTG_dev, "% ADC Values\r\n");
					for (i=0;i<FFT_LENGTH;i++){
						usb_printf(&USB_OTG_dev," %d",DeBuf2[i << 1]);
						if (i != FFT_LENGTH - 1) usb_printf(&USB_OTG_dev,";");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					//usb_printf(&USB_OTG_dev,"%c",-1);
					break;

				case 40:	//Frequenz 1
					usb_printf(&USB_OTG_dev," %d",Freq1);
					usb_printf(&USB_OTG_dev,";");
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 41:	//Frequenz 2
					usb_printf(&USB_OTG_dev," %d",Freq2);
					usb_printf(&USB_OTG_dev,";");
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 50:	//Peak 1
					usb_printf(&USB_OTG_dev," %d",Maxpeak1);
					usb_printf(&USB_OTG_dev,";");
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 51:	//Peak 2
					usb_printf(&USB_OTG_dev," %d",Maxpeak2);
					usb_printf(&USB_OTG_dev,";");
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 60:	// Phase Value
					for (i=0;i<FFT_LENGTH;i++){
						usb_printf(&USB_OTG_dev," %d",Vibration_A[i]);
						if (i != FFT_LENGTH - 1) usb_printf(&USB_OTG_dev,";");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					break;


#else
				case 22:	// ADC Values
					usb_printf(&USB_OTG_dev," %d",0);
					usb_printf(&USB_OTG_dev,"\r\n");
					//usb_printf(&USB_OTG_dev,"%c",-1);
					break;
#endif

#ifdef MATLAB_OUTPUT
				case 23:	// Hanning filtered ADC vals.
						//usb_printf(&USB_OTG_dev, "% Hanning filtered ADC\r\n");
						for (i=0;i<FFT_LENGTH;i++){
							usb_printf(&USB_OTG_dev," %d",DeHannBuf1[i << 1]);
							if (i != FFT_LENGTH - 1) usb_printf(&USB_OTG_dev,";");
						}
					usb_printf(&USB_OTG_dev,"\r\n");
					flag=0;
					break;
#else
				case 23:	// Hanning filtered ADC vals.
					usb_printf(&USB_OTG_dev," %d",0);
					usb_printf(&USB_OTG_dev,"\r\n");
					flag=0;
					break;
#endif

				case 24:	// Fundamental freq. , rpm
					usb_printf(&USB_OTG_dev,"%u ;",(u32)(peakIdcsList[0]*samplerate/FFT_LENGTH/2));
					usb_printf(&USB_OTG_dev,"%u \r\n",(u32)(peakIdcsList[0]*samplerate/FFT_LENGTH/2)/bladecnt*60);
					break;

				case 25:	// peak list freq.
					for (i=0;i<no_of_peaks;i++){
						usb_printf(&USB_OTG_dev,"%u",(u32)(peakIdcsList[i]*samplerate/FFT_LENGTH/2));
						if (i != no_of_peaks - 1) usb_printf(&USB_OTG_dev,"; ");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 26:	// peak list rpm
					for (i=0;i<no_of_peaks;i++){
						usb_printf(&USB_OTG_dev,"%u",(u32)(peakIdcsList[i]*samplerate/FFT_LENGTH/2)/bladecnt*60);
						if (i != no_of_peaks - 1) usb_printf(&USB_OTG_dev,"; ");
					}
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 27:	//ADC max
					usb_printf(&USB_OTG_dev,"%u",ADC_max_value);
					usb_printf(&USB_OTG_dev,"\r\n");
					break;

				case 28:	//Actual gain info
					usb_printf(&USB_OTG_dev,"%u",PGA_gain);
					usb_printf(&USB_OTG_dev,"\r\n");
					break;


				default:
					sendReply("Unknown M%d\n\r",get_int('M'));
					return NO_REPLY;
			}
			break;
		}
		default:
			sendReply("Unknown command %c\n\r",get_command());
			return NO_REPLY;
	}
	return SEND_REPLY;
}



//full line has been received, process it for line number, checksum, etc. before processing the actual command
static void gcode_line_received()
{
	if (parserState.commandLen)
	{
		if (parserState.commandBuffer[0] == 'N')
		{
			int32_t line = get_int('N');
			
			if (line != parserState.last_N+1 && (!has_code('M') || get_uint('M') != 110))
			{
				sendReply("rs %u line number incorrect\r\n",parserState.last_N+1);
				return;
			}
			parserState.line_N = line;

			char* ptr;
			if ((ptr = strchr(parserState.parsePos,'*')) != NULL)
			{
				if (get_uint('*') != calculate_checksum(parserState.commandBuffer))
				{
					sendReply("rs %u incorrect checksum\r\n",parserState.last_N+1);
					return;
				}
				*ptr = 0;
			}
			else
			{
				sendReply("No checksum with line number\n\r");
				return;
			}
			parserState.last_N = parserState.line_N;			
		}
		else if (strchr(parserState.commandBuffer,'*') != NULL)
		{
			sendReply("No line number with checksum\n\r");
			return;
		}

		parserState.parsePos = trim_line(parserState.commandBuffer);

//		DEBUG("gcode line: '%s'\n\r",parserState.parsePos);
		if (gcode_process_command() == SEND_REPLY)
		{
			sendReply("ok\r\n");
//			previous_millis_cmd = timestamp;
		}
	}
	
}

void gcode_init(ReplyFunction replyFunc)
{
	ringbuffer_init(&uartBuffer);
	memset(&parserState,0,sizeof(ParserState));
	parserState.replyFunc = replyFunc;
	
	samserial_setcallback(gcode_datareceived);
}

void gcode_update()
{
	while (ringbuffer_numAvailable(&uartBuffer) > 0)
	{
		uint8_t chr = ringbuffer_get(&uartBuffer);
		
		switch(chr)
		{
			case ';':
			case '(':
				parserState.comment_mode = true;
				break;
			case '\n':
			case '\r':
				parserState.commandBuffer[parserState.commandLen] = 0;
				parserState.parsePos = parserState.commandBuffer;
				gcode_line_received();
				parserState.comment_mode = false;
				parserState.commandLen = 0;
				
				break;
			default:
				if (parserState.commandLen >= BUFFER_SIZE)
				{
					printf("error: command buffer full!\n\r");
				}
				else
				{
					if (!parserState.comment_mode)
						parserState.commandBuffer[parserState.commandLen++] = chr;
				}
				break;
		}
		
	}
	
}


