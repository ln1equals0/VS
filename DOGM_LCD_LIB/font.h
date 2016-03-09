#ifndef FONT_H
#define FONT_H

// Font for printing ASCII characters on a graphics display

// The first character has the ASCII code 0x20, the last one 0x7F.
// Adresses can be calculated like this:
// (where &data is the beginning adress of the data and asc the ascii code)
// addr = &data + 7 * (asc - 0x20);

// Each byte of a character is a column of it.
// The least significant bit is the top most pixel, the most sig. the bottom.
// I.e. 0x01 is a pixel at the top most row, where 0x80 is one at the bottom.

// An empty column is indicated by 0xAA and is ignored while drawing text.
// If no space indicater is set, or if all characters have the same count
// of space indicators, the font is monospace.


#define CHAR_UNDEFINED {0xFF, 0x81, 0x81, 0x81, 0xFF, 0xaa, 0xaa}


const uint8_t font_standard_[96][7]  = {
  /* 20   */  {0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 21 ! */  CHAR_UNDEFINED,
  /* 22 " */  CHAR_UNDEFINED,
  /* 23 # */  CHAR_UNDEFINED,
  /* 24 --*/  {0x08, 0x08, 0x08, 0x08, 0x08, 0xaa, 0xaa}, //  non-ASCII  $ => -- (long hyphen)
  /* 25 % */  CHAR_UNDEFINED,
  /* 26 d */  {0x60, 0x58, 0x46, 0x58, 0x60, 0xaa, 0xaa}, //  non-ASCII  & => (delta)
  /* 27 ' */  CHAR_UNDEFINED,
  /* 28 ( */  {0x3E, 0x41, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 29 ) */  {0x41, 0x3E, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 2A * */  CHAR_UNDEFINED,
  /* 2B + */  CHAR_UNDEFINED,
  /* 2C , */  {0x80, 0x60, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 2D - */  {0x10, 0x10, 0x10, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 2E . */  {0x60, 0x60, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 2F / */  {0x40, 0x10, 0x04, 0x01, 0xaa, 0xaa, 0xaa},

  /* 30 0 */  {0x3e, 0x41, 0x41, 0x41, 0x3e, 0xaa, 0xaa}, // digits are monospaced
  /* 31 1 */  {0x00, 0x42, 0x7F, 0x40, 0x00, 0xaa, 0xaa},
  /* 32 2 */  {0x71, 0x49, 0x49, 0x49, 0x46, 0xaa, 0xaa},
  /* 33 3 */  {0x41, 0x49, 0x49, 0x49, 0x36, 0xaa, 0xaa},
  /* 34 4 */  {0x1C, 0x12, 0x11, 0x7F, 0x10, 0xaa, 0xaa},
  /* 35 5 */  {0x4F, 0x49, 0x49, 0x49, 0x31, 0xaa, 0xaa},
  /* 36 6 */  {0x3E, 0x49, 0x49, 0x49, 0x30, 0xaa, 0xaa},
  /* 37 7 */  {0x01, 0x71, 0x09, 0x05, 0x03, 0xaa, 0xaa},
  /* 38 8 */  {0x36, 0x49, 0x49, 0x49, 0x36, 0xaa, 0xaa},
  /* 39 9 */  {0x06, 0x49, 0x49, 0x49, 0x3E, 0xaa, 0xaa},
  /* 3A : */  {0x24, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 3B   */  {0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa}, //  non-ASCII  ; => (space in same width as digits)
  /* 3C < */  CHAR_UNDEFINED,
  /* 3D = */  {0x14, 0x14, 0x14, 0x14, 0xaa, 0xaa, 0xaa},
  /* 3E > */  CHAR_UNDEFINED,
  /* 3F ? */  CHAR_UNDEFINED,

  /* 40 @ */  CHAR_UNDEFINED,
  /* 41 A */  {0x78, 0x16, 0x11, 0x16, 0x78, 0xaa, 0xaa},
  /* 42 B */  {0x7f, 0x49, 0x49, 0x49, 0x36, 0xaa, 0xaa},
  /* 43 C */  {0x3e, 0x41, 0x41, 0x41, 0x41, 0xaa, 0xaa},
  /* 44 D */  {0x7f, 0x41, 0x41, 0x41, 0x3E, 0xaa, 0xaa},
  /* 45 E */  {0x7f, 0x49, 0x49, 0x49, 0x41, 0xaa, 0xaa},
  /* 46 F */  {0x7f, 0x09, 0x09, 0x09, 0x01, 0xaa, 0xaa},
  /* 47 G */  {0x3e, 0x41, 0x49, 0x49, 0x79, 0xaa, 0xaa},
  /* 48 H */  {0x7f, 0x08, 0x08, 0x08, 0x7f, 0xaa, 0xaa},
  /* 49 I */  {0x7f, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 4A J */  {0x20, 0x40, 0x40, 0x3f, 0xaa, 0xaa, 0xaa},
  /* 4B K */  {0x7f, 0x08, 0x14, 0x22, 0x41, 0xaa, 0xaa},
  /* 4C L */  {0x7f, 0x40, 0x40, 0x40, 0x40, 0xaa, 0xaa},
  /* 4D M */  {0x7f, 0x06, 0x18, 0x06, 0x7f, 0xaa, 0xaa},
  /* 4E N */  {0x7f, 0x06, 0x08, 0x30, 0x7f, 0xaa, 0xaa},
  /* 4F O */  {0x3e, 0x41, 0x41, 0x41, 0x3e, 0xaa, 0xaa},

  /* 50 P */  {0x7f, 0x09, 0x09, 0x09, 0x06, 0xaa, 0xaa},
  /* 51 Q */  {0x3e, 0x41, 0x41, 0x21, 0x5e, 0x40, 0xaa},
  /* 52 R */  {0x7f, 0x09, 0x19, 0x29, 0x46, 0xaa, 0xaa},
  /* 53 S */  {0x46, 0x49, 0x49, 0x49, 0x31, 0xaa, 0xaa},
  /* 54 T */  {0x01, 0x01, 0x7f, 0x01, 0x01, 0xaa, 0xaa},
  /* 55 U */  {0x3f, 0x40, 0x40, 0x40, 0x3f, 0xaa, 0xaa},
  /* 56 V */  {0x07, 0x38, 0x40, 0x38, 0x07, 0xaa, 0xaa},
  /* 57 W */  {0x07, 0x38, 0x40, 0x38, 0x40, 0x38, 0x07},
  /* 58 X */  {0x63, 0x14, 0x08, 0x14, 0x63, 0xaa, 0xaa},
  /* 59 Y */  {0x03, 0x04, 0x78, 0x04, 0x03, 0xaa, 0xaa},
  /* 5A Z */  {0x61, 0x51, 0x49, 0x45, 0x43, 0xaa, 0xaa},
  /* 5B [ */  CHAR_UNDEFINED,
  /* 5C \ */  CHAR_UNDEFINED,
  /* 5D ] */  CHAR_UNDEFINED,
  /* 5E ^ */  CHAR_UNDEFINED,
  /* 5F _ */  CHAR_UNDEFINED,

  /* 60 � */  {0x02, 0x05, 0x02, 0xaa, 0xaa, 0xaa, 0xaa}, // non-ASCII  ` => �
  /* 61 a */  {0x20, 0x54, 0x54, 0x54, 0x78, 0xaa, 0xaa},
  /* 62 b */  {0x7f, 0x44, 0x44, 0x44, 0x38, 0xaa, 0xaa},
  /* 63 c */  {0x38, 0x44, 0x44, 0x44, 0x44, 0xaa, 0xaa},
  /* 64 d */  {0x38, 0x44, 0x44, 0x44, 0x7f, 0xaa, 0xaa},
  /* 65 e */  {0x38, 0x54, 0x54, 0x54, 0x58, 0xaa, 0xaa},
  /* 66 f */  {0x7e, 0x05, 0x05, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 67 g */  {0x18, 0xa4, 0xa4, 0xa4, 0x7c, 0xaa, 0xaa},
  /* 68 h */  {0x7f, 0x04, 0x04, 0x04, 0x78, 0xaa, 0xaa},
  /* 69 i */  {0x7d, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 6A j */  {0x80, 0x7d, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 6B k */  {0x7f, 0x10, 0x28, 0x44, 0xaa, 0xaa, 0xaa},
  /* 6C l */  {0x7f, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 6D m */  {0x7c, 0x04, 0x7c, 0x04, 0x78, 0xaa, 0xaa},
  /* 6E n */  {0x7c, 0x04, 0x04, 0x04, 0x78, 0xaa, 0xaa},
  /* 6F o */  {0x38, 0x44, 0x44, 0x44, 0x38, 0xaa, 0xaa},

  /* 70 p */  {0xfc, 0x24, 0x24, 0x24, 0x18, 0xaa, 0xaa},
  /* 71 q */  {0x18, 0x24, 0x24, 0x24, 0xfc, 0xaa, 0xaa},
  /* 72 r */  {0x7c, 0x08, 0x04, 0x04, 0xaa, 0xaa, 0xaa},
  /* 73 s */  {0x58, 0x54, 0x54, 0x34, 0xaa, 0xaa, 0xaa},
  /* 74 t */  {0x3f, 0x44, 0x44, 0xaa, 0xaa, 0xaa, 0xaa},
  /* 75 u */  {0x3c, 0x40, 0x40, 0x40, 0x7c, 0xaa, 0xaa},
  /* 76 v */  {0x0c, 0x30, 0x40, 0x30, 0x0c, 0xaa, 0xaa},
  /* 77 w */  {0x3c, 0x40, 0x38, 0x40, 0x3c, 0xaa, 0xaa},
  /* 78 x */  {0x44, 0x28, 0x10, 0x28, 0x44, 0xaa, 0xaa},
  /* 79 y */  {0x1c, 0xa0, 0xa0, 0xa0, 0x7c, 0xaa, 0xaa},
  /* 7A z */  {0x64, 0x54, 0x54, 0x4c, 0xaa, 0xaa, 0xaa},
  /* 7B � */  {0x21, 0x54, 0x54, 0x54, 0x79, 0xaa, 0xaa}, // non-ASCII  { => �
  /* 7C | */  CHAR_UNDEFINED,
  /* 7D � */  {0x39, 0x44, 0x44, 0x44, 0x39, 0xaa, 0xaa}, // non-ASCII  } => �
  /* 7E � */  {0x3d, 0x40, 0x40, 0x40, 0x7d, 0xaa, 0xaa}, // non-ASCII  ~ => �
  /* 7F   */  {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}  // non-ASCII  ? => (empty char) (1 pixel space in text)

};


const uint8_t font_standard[96][5]  = {
	{0x00, 0x00, 0x00, 0x00, 0x00},// (space)
	{0x00, 0x00, 0x5F, 0x00, 0x00},// !
	{0x00, 0x07, 0x00, 0x07, 0x00},// "
	{0x14, 0x7F, 0x14, 0x7F, 0x14},// #
	{0x24, 0x2A, 0x7F, 0x2A, 0x12},// $
	{0x23, 0x13, 0x08, 0x64, 0x62},// %
	{0x36, 0x49, 0x55, 0x22, 0x50},// &
	{0x00, 0x05, 0x03, 0x00, 0x00},// '
	{0x00, 0x1C, 0x22, 0x41, 0x00},// (
	{0x00, 0x41, 0x22, 0x1C, 0x00},// )
	{0x08, 0x2A, 0x1C, 0x2A, 0x08},// *			10
	{0x08, 0x08, 0x3E, 0x08, 0x08},// +
	{0x00, 0x50, 0x30, 0x00, 0x00},// ,
	{0x08, 0x08, 0x08, 0x08, 0x08},// -
	{0x00, 0x60, 0x60, 0x00, 0x00},// .
	{0x20, 0x10, 0x08, 0x04, 0x02},// /
	{0x3E, 0x51, 0x49, 0x45, 0x3E},// 0
	{0x00, 0x42, 0x7F, 0x40, 0x00},// 1
	{0x42, 0x61, 0x51, 0x49, 0x46},// 2
	{0x21, 0x41, 0x45, 0x4B, 0x31},// 3
	{0x18, 0x14, 0x12, 0x7F, 0x10},// 4			20
	{0x27, 0x45, 0x45, 0x45, 0x39},// 5
	{0x3C, 0x4A, 0x49, 0x49, 0x30},// 6
	{0x01, 0x71, 0x09, 0x05, 0x03},// 7
	{0x36, 0x49, 0x49, 0x49, 0x36},// 8
	{0x06, 0x49, 0x49, 0x29, 0x1E},// 9
	{0x00, 0x36, 0x36, 0x00, 0x00},// :
	{0x00, 0x56, 0x36, 0x00, 0x00},// ;
	//{0x00, 0x08, 0x14, 0x22, 0x41},// <
	{0x00, 0x08, 0x1C, 0x3E, 0x7F},// <
	{0x14, 0x14, 0x14, 0x14, 0x14},// =
	//{0x41, 0x22, 0x14, 0x08, 0x00},// >			30
	{0x7F, 0x3E, 0x1C, 0x08, 0x00},// >			30
	{0x02, 0x01, 0x51, 0x09, 0x06},// ?
	{0x32, 0x49, 0x79, 0x41, 0x3E},// @
	{0x7E, 0x11, 0x11, 0x11, 0x7E},// A
	{0x7F, 0x49, 0x49, 0x49, 0x36},// B
	{0x3E, 0x41, 0x41, 0x41, 0x22},// C
	{0x7F, 0x41, 0x41, 0x22, 0x1C},// D
	{0x7F, 0x49, 0x49, 0x49, 0x41},// E
	{0x7F, 0x09, 0x09, 0x01, 0x01},// F
	{0x3E, 0x41, 0x41, 0x51, 0x32},// G
	{0x7F, 0x08, 0x08, 0x08, 0x7F},// H			40
	{0x00, 0x41, 0x7F, 0x41, 0x00},// I
	{0x20, 0x40, 0x41, 0x3F, 0x01},// J
	{0x7F, 0x08, 0x14, 0x22, 0x41},// K
	{0x7F, 0x40, 0x40, 0x40, 0x40},// L
	{0x7F, 0x02, 0x04, 0x02, 0x7F},// M
	{0x7F, 0x04, 0x08, 0x10, 0x7F},// N
	{0x3E, 0x41, 0x41, 0x41, 0x3E},// O
	{0x7F, 0x09, 0x09, 0x09, 0x06},// P
	{0x3E, 0x41, 0x51, 0x21, 0x5E},// Q
	{0x7F, 0x09, 0x19, 0x29, 0x46},// R
	{0x46, 0x49, 0x49, 0x49, 0x31},// S
	{0x01, 0x01, 0x7F, 0x01, 0x01},// T
	{0x3F, 0x40, 0x40, 0x40, 0x3F},// U
	{0x1F, 0x20, 0x40, 0x20, 0x1F},// V
	{0x7F, 0x20, 0x18, 0x20, 0x7F},// W
	{0x63, 0x14, 0x08, 0x14, 0x63},// X
	{0x03, 0x04, 0x78, 0x04, 0x03},// Y
	{0x61, 0x51, 0x49, 0x45, 0x43},// Z
	{0x00, 0x00, 0x7F, 0x41, 0x41},// [
	{0x02, 0x04, 0x08, 0x10, 0x20},// "\"
	{0x41, 0x41, 0x7F, 0x00, 0x00},// ]
//	{0x04, 0x02, 0x01, 0x02, 0x04},// ^
//	{0x40, 0x40, 0x40, 0x40, 0x40},// _
	{0x08, 0x18, 0x38, 0x18, 0x08},// ^ Arrow up
	{0x10, 0x18, 0x1C, 0x18, 0x10},// _ Arrow down
	{0x00, 0x01, 0x02, 0x04, 0x00},// `
	{0x20, 0x54, 0x54, 0x54, 0x78},// a
	{0x7F, 0x48, 0x44, 0x44, 0x38},// b
	{0x38, 0x44, 0x44, 0x44, 0x20},// c
	{0x38, 0x44, 0x44, 0x48, 0x7F},// d
	{0x38, 0x54, 0x54, 0x54, 0x18},// e
	{0x08, 0x7E, 0x09, 0x01, 0x02},// f
	{0x08, 0x14, 0x54, 0x54, 0x3C},// g
	{0x7F, 0x08, 0x04, 0x04, 0x78},// h
	{0x00, 0x44, 0x7D, 0x40, 0x00},// i
	{0x20, 0x40, 0x44, 0x3D, 0x00},// j
	{0x00, 0x7F, 0x10, 0x28, 0x44},// k
	{0x00, 0x41, 0x7F, 0x40, 0x00},// l
	{0x7C, 0x04, 0x18, 0x04, 0x78},// m
	{0x7C, 0x08, 0x04, 0x04, 0x78},// n
	{0x38, 0x44, 0x44, 0x44, 0x38},// o
	{0x7C, 0x14, 0x14, 0x14, 0x08},// p
	{0x08, 0x14, 0x14, 0x18, 0x7C},// q
	{0x7C, 0x08, 0x04, 0x04, 0x08},// r
	{0x48, 0x54, 0x54, 0x54, 0x20},// s
	{0x04, 0x3F, 0x44, 0x40, 0x20},// t
	{0x3C, 0x40, 0x40, 0x20, 0x7C},// u
	{0x1C, 0x20, 0x40, 0x20, 0x1C},// v
	{0x3C, 0x40, 0x30, 0x40, 0x3C},// w
	{0x44, 0x28, 0x10, 0x28, 0x44},// x
	{0x0C, 0x50, 0x50, 0x50, 0x3C},// y
	{0x44, 0x64, 0x54, 0x4C, 0x44},// z
	{0x00, 0x08, 0x36, 0x41, 0x00},// {
	{0x00, 0x00, 0x7F, 0x00, 0x00},// |
	{0x00, 0x41, 0x36, 0x08, 0x00},// }
	{0x08, 0x08, 0x2A, 0x1C, 0x08},// ->
	{0x08, 0x1C, 0x2A, 0x08, 0x08} // <-
};

#endif

