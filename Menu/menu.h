/*
*   MenuLib
*   A library for generating simple tree based menus on small (embedded) systems.
*   Download a demo application from www.tobias-schlegel.de
*
*   File: menu.h
*
*   Version:    1.0 (2.Jan.2011)
*   Author:     Tobias Schlegel, DK7SCH
*   Web:        www.tobias-schlegel.de
*   Contact:    tobias@drschlegel.de
*   Date:       2.Jan.2011
*   License:    Creative Commons CC-BY 3.0
*               http://creativecommons.org/licenses/by/3.0/deed.en
*
*               Attribution:    Attribute with a copy of this code header and (if applicable)
*                               a notation in the "About" page of your product.
*
*/

#ifndef MENU_H_
#define MENU_H_


#define cMenuLine 21 //Maximum length of a menu entry
#define mCallbackWithParams 1 //Use Callback functions with a (preshared) parameter

//Undefine to deactivate feature
//#define mOffsetString "+" //A char (string) to display a father-> child relation
#define mOffsetShortenedString ">" //At some point the tree history must be shortened. Symbos for this. Undefine only in combination with mOffsetString
#define mMarkedString ">" //Char (string) to mark the selected item
//#define mNotMarkedString "-" //Char (string) to mark not selected item

#define mKeyUp      8 //Keycode for "up"
#define mKeyDown    2 //Keycode for "down"
#define mKeyOK      6 //Keycode for "OK" (deeper)
#define mKeyBack    4 //Keycode for "back"

//Do not edit below.

#ifndef __menu_type
#define __menu_type 1


typedef void (*menucallback)(unsigned char);

typedef struct {
    const menucallback CallBack;
    const unsigned char CallBackArgument;
    const char cText[cMenuLine];
    const unsigned char children;
}menudata;


#define menuCnull (menucallback) 0  //placeholder for "nothing"

#endif

void init_menu(const menudata * menu, unsigned char entries);
unsigned char mGetText(char * txt, unsigned char lines, unsigned char linelen, unsigned char parents);
void mStatemachine(unsigned char key);


#endif /*MENU_H_*/
