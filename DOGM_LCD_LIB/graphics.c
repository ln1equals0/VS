#include "graphics.h"
#include "driver_dogm128.h"
#include "font.h"
#include <stdint.h> 

// Invert all draw functions
uint8_t g_inverted = false;


void g_init()
{
	disp_init();
}


void g_clear()
{
	disp_clear();
}


void g_clear_dont_refresh()
{
	disp_clear_dont_refresh();
}


void g_draw_pixel(uint8_t x, uint8_t y)
{
	disp_set_pixel(x, y, 1 - g_inverted);
}


void g_clear_pixel(uint8_t x, uint8_t y)
{
	disp_set_pixel(x, y, 0 + g_inverted);
}


void g_draw_line(uint8_t x, uint8_t y, int16_t len, uint8_t direction)
{
	// direction: 0 = horizontal,
	//            1 = vertical

	if (direction == 0)
	{
		if (len > 0)
			for (; len > 0; len--, x++)
				disp_set_pixel(x, y, 1);
		else
			for (; len < 0; len++, x--)
				disp_set_pixel(x, y, 1);
	}
	else
	{
		if (len > 0)
			for (; len > 0; len--, y++)
				disp_set_pixel(x, y, 1);
		else
			for (; len < 0; len++, y--)
				disp_set_pixel(x, y, 1);
	}
}

void g_clear_line(uint8_t x, uint8_t y, int16_t len, uint8_t direction)
{
	// direction: 0 = horizontal,
	//            1 = vertical

	if (direction == 0)
	{
		if (len > 0)
			for (; len > 0; len--, x++)
				disp_set_pixel(x, y, 0);
		else
			for (; len < 0; len++, x--)
				disp_set_pixel(x, y, 0);
	}
	else
	{
		if (len > 0)
			for (; len > 0; len--, y++)
				disp_set_pixel(x, y, 0);
		else
			for (; len < 0; len++, y--)
				disp_set_pixel(x, y, 0);
	}
}


void g_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	g_draw_line_h(x,         y,         (int16_t)w);
	g_draw_line_h(x,         y + h - 1, (int16_t)w);
	g_draw_line_v(x,         y,         (int16_t)h);
	g_draw_line_v(x + w - 1, y,         (int16_t)h);
}



void g_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	for (; h > 0; h--, y++)
		g_draw_line_h(x, y, (int16_t)w);
}


void g_clear_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	for (; h > 0; h--, y++)
		g_clear_line_h(x, y, (int16_t)w);
}



void g_draw_data_column_transparentBG(int8_t x, uint8_t y, uint8_t data)
{
	uint8_t row;
	for (row = 0; row < 8; row++,y++)
	{
		if ((data & (1<<row)) != 0)
			g_draw_pixel(x, y);
	}
}


void g_draw_data_column_clearBG(int8_t x, uint8_t y, uint8_t data)
{
	uint8_t row;
	for (row = 0; row < 8; row++,y++)
	{
		if ((data & (1<<row)) != 0)
			g_draw_pixel(x, y);
		else
			g_clear_pixel(x, y);
	}
}


void g_draw_data_column_blackBG(int8_t x, uint8_t y, uint8_t data)
{
	uint8_t row;
	for (row = 0; row < 8; row++,y++)
	{
		if ((data & (1<<row)) != 0)
			g_clear_pixel(x, y);
		else
			g_draw_pixel(x, y);
	}
}

/*
uint8_t g_draw_char_transparentBG(uint8_t x, uint8_t y, uint8_t c)
{
	uint8_t w = 0; //width of drawn character (return value)

	uint16_t addr = (uint16_t)(&font_standard[c-0x20]);
	uint8_t data = 0x00;

	while (data != 0xAA && w++ < 7)
	{
		data = pgm_read_byte(addr);
		if (data != 0xAA)
			g_draw_data_column_transparentBG(x++, y, data);
		else
			w--;
		addr++;
	}
	if (w > 7) w = 7;
	return (w);
}
*/

uint8_t g_draw_char_transparentBG(uint8_t x, uint8_t y, uint8_t c)
{
	//uint8_t w = 0; //width of drawn character (return value)
unsigned char i;
	
for (i=0; i<5; i++)
	{
	 g_draw_data_column_transparentBG(x++, y, font_standard[c-0x20][i]);
	}

return 5;

}


/*
uint8_t g_draw_char_clearBG(uint8_t x, uint8_t y, uint8_t c)
{
	uint8_t w = 0; //width of drawn character (return value)

	uint16_t addr = (uint16_t)(&font_standard[c-0x20]);
	uint8_t data = 0x00;

	while (data != 0xAA && w++ < 7)
	{
		data = pgm_read_byte(addr);
		if (data != 0xAA)
			g_draw_data_column_clearBG(x++, y, data);
		else
			w--;
		addr++;
	}
	if (w > 7) w = 7;
	return (w);
}
*/

uint8_t g_draw_char_clearBG(uint8_t x, uint8_t y, uint8_t c)
{
unsigned char i;
	
for (i=0; i<5; i++)
	{
	 g_draw_data_column_clearBG(x++, y, font_standard[c-0x20][i]);
	}

return 5;

}

uint8_t g_draw_char_WonB(uint8_t x, uint8_t y, uint8_t c)
{
unsigned char i;
	
for (i=0; i<5; i++)
	{
	 g_draw_data_column_blackBG(x++, y, font_standard[c-0x20][i]);
	}

return 5;

}


uint8_t g_draw_string(uint8_t x, uint8_t y, char *s, uint8_t transparent_background)
{
	switch ( transparent_background )
	{
		case 	1: 	while (*s)  // Transparent
					{   // so lange *s != '\0' also ungleich dem "String-Endezeichen"
					 x += g_draw_char_transparentBG(x, y, *s) + 1;
					 s++;
					}
					break;
					
		case	2:	while (*s)	// White on Black, User has to take care that Background is black
					{   // so lange *s != '\0' also ungleich dem "String-Endezeichen"
					 x += g_draw_char_WonB(x, y, *s) + 1;
					 s++;
					}
				
		default:	while (*s) // Black on white
					{   // so lange *s != '\0' also ungleich dem "String-Endezeichen"
					 x += g_draw_char_clearBG(x, y, *s) + 1;
					 s++;
					}
					break;
	}
			
return 0;
}



void g_frame_begin()
{
	disp_frame_begin();
}

void g_frame_end()
{
	disp_frame_end();
}

