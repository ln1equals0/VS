#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h> 

// GRAPHICS /////////////////////////////////////////////////////////////////////////


// Using standard fonts


//#define G_FONT_DEFAULT font_standard


// Variables
#define true  1
#define false 0


void g_init( void );
void g_clear( void );
void g_clear_dont_refresh( void );
void g_draw_pixel(uint8_t x, uint8_t y);
void g_clear_pixel(uint8_t x, uint8_t y);
void g_clear_line(uint8_t x, uint8_t y, int16_t len, uint8_t direction);
void g_draw_line(uint8_t x, uint8_t y, int16_t len, uint8_t direction);

#define g_draw_line_h(x,y,len) g_draw_line(x,y,len,0)
#define g_clear_line_h(x,y,len) g_clear_line(x,y,len,0)
#define g_draw_line_v(x,y,len) g_draw_line(x,y,len,1)

void g_clear_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void g_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void g_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void g_draw_data_column_transparentBG(int8_t x, uint8_t y, uint8_t data);
void g_draw_data_column_clearBG(int8_t x, uint8_t y, uint8_t data);
void g_draw_data_column_blackBG(int8_t x, uint8_t y, uint8_t data);
uint8_t g_draw_char_transparentBG(uint8_t x, uint8_t y, uint8_t c);
uint8_t g_draw_char_clearBG(uint8_t x, uint8_t y, uint8_t c);
uint8_t g_draw_char_WonB(uint8_t x, uint8_t y, uint8_t c);
uint8_t g_draw_string(uint8_t x, uint8_t y, char *s, uint8_t transparent_background);
void g_frame_begin( void );
void g_frame_end( void );

#endif
