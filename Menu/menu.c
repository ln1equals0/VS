
/*
*   MenuLib
*   A library for generating simple tree based menus on small (embedded) systems.
*   Download a demo application from www.tobias-schlegel.de
*
*   File: menu.c
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

//#include <inttypes.h>

#include "Menu/menu.h"
#include "User_String/user_string.h"

volatile unsigned char    mLayer = 0;
const menudata            * mMenu;
volatile unsigned char    mMenuLen = 0;
const menudata            * mActualMenuElement;

void init_menu(const menudata * menu, unsigned char entries){
    mMenu = menu;
    mActualMenuElement = &(mMenu[5]);
    mMenuLen = entries;
}

unsigned char getChild(unsigned char parent){
    unsigned char sum = 0, i = 0;

    for(i = 0 ; i < parent ; i++){
        sum += mMenu[i].children;
    }

    if(sum > mMenuLen){
        return parent;
    }

    return (sum+1);
}

unsigned char getParent(unsigned char child){
    unsigned char sum = 0, i = 0;

    for(i = 0 ; i < mMenuLen ; i++){
        sum += mMenu[i].children;

        if((child <= sum) && (child > (sum - mMenu[i].children))){
            return i;
        }
    }
    return child;
}

unsigned char getIndex(const menudata * ptr){
    unsigned char i = 0;

    for(i = 0 ; i < mMenuLen+1 ; i++){
        if(ptr == &mMenu[i]){
            return i;
        }
    }
    return 0;
}

#ifdef mOffsetString
void addOffset(char * t, unsigned char offset, unsigned char payload, unsigned char maxlen, const char * cOffset){
    unsigned char i = 0, real;

    real = offset + payload;

    if(real > maxlen){
        real = maxlen - payload;
    }else{
        real = offset;
    }

    if(real > 0){
        if(real != offset){
            str_cpy(t, (char *)mOffsetShortenedString);
        }else{
            str_cpy(t, (char *)cOffset);
        }

        for(i = 1 ; i < real ; i++){
            str_cat(t, (char *)cOffset);
        }
    }
}

void addOffset_p(char * t, unsigned char offset, unsigned char payload, unsigned char maxlen, const char * cOffset){
    unsigned char i = 0, real;

    real = offset + payload;

    if(real > maxlen){
        real = maxlen - payload;
    }else{
        real = offset;
    }

    if(real > 0){
        if(real != offset){
            str_cpy(t, (char *)mOffsetShortenedString);
        }else{
            str_cpy(t, (char *)cOffset);
        }

        for(i = 1 ; i < real ; i++){
            str_cat(t, (char *)cOffset);
        }
    }
}
#endif

unsigned char mGetText(char * txt, unsigned char lines, unsigned char linelen, unsigned char parents){
    unsigned char actual = 0, b4 = 0, aft = 0, showentries = 0;
    unsigned char showmax = 0, start = 0, stop = 0, parent = 0, parentcount =  0;
    unsigned char i = 0, space = 0 ;
    unsigned char line = 0, offset = 1, tmp = 0;

    for(i = 0 ; i < (lines * linelen +1) ; i++){
        txt[i] = (char) 0;
    }

    space = linelen +1;
    actual = getIndex(mActualMenuElement);
    start = getChild(getParent(actual));
    stop = start + mMenu[getParent(actual)].children - 1;
    showmax = mMenu[getParent(actual)].children;

    if(parents > 0){ //mode showing parent menu in first lines
        if(lines < parents){
            parents = lines -1;
        }

        parent = actual;

        for(i = 0 ; i < mMenuLen ; i++){
            if(parent == 0){
                parentcount = i;
                break;
            }
            parent = getParent(parent);
        }

        if(parentcount < parents){
            parents = parentcount;
        }

        if(parents > (lines -1)){
            parents = lines -1;
        }

        parent = actual;
        tmp = parents-1;

        for(i = 0 ; i < parents ; i++){
            #ifdef mOffsetString
            addOffset(&(txt[tmp * space]), tmp, str_len_p(mMenu[getParent(parent)].cText), linelen, mOffsetString);
            #endif
            str_cat_p(&(txt[tmp * space]), mMenu[getParent(parent)].cText);
            parent = getParent(parent);
            line++;
            tmp--;
            offset++;
        }
    }

    showentries = (lines - line);

    if(showentries > 1){
        if(actual == start){
            b4 = stop;
        }else{
            b4 = actual -1;
        }

        if(actual == stop){
            aft = start;
        }else{
            aft = actual + 1;
        }

        #ifdef mOffsetString
        addOffset(&(txt[line * space]), offset-1, str_len_p(mMenu[b4].cText), linelen, mOffsetString);
        #endif
        #ifdef mNotMarkedString
        str_cat(&(txt[line * space]), mNotMarkedString);
        #endif
        str_cat_p(&(txt[line * space]), mMenu[b4].cText);
        line++;
    }

    //add marked menu entry.
    #ifdef mOffsetString
    addOffset(&(txt[line * space]), offset-1, str_len_p(mMenu[actual].cText), linelen, mOffsetString);
    #endif
    str_cat(&(txt[line * space]), mMarkedString);
    str_cat_p(&(txt[line * space]), mMenu[actual].cText);
    line++;

    showentries = (lines - line);

    if(showentries > 0){
        for(i = 0 ; i < showentries ; i++){
            #ifdef mOffsetString
            addOffset(&(txt[line * space]), offset -1, str_len_p(mMenu[aft+i].cText), linelen, mOffsetString);
            #endif
            #ifdef mNotMarkedString
            str_cat(&(txt[line * space]), mNotMarkedString);
            #endif
            if ((aft+i) > stop) aft = start - 1;
            str_cat_p(&(txt[line * space]), mMenu[aft+i].cText);
            line++;
        }
    }

return showmax + 1;

}

void mStatemachine(unsigned char key){
    unsigned char start = 0, stop = 0, actual = 0;

    actual = getIndex(mActualMenuElement);
    start = getChild(getParent(actual));
    stop = start + mMenu[getParent(actual)].children - 1;

    if(key == mKeyUp){  //nav up (scroll)
        if((actual == start) || (actual > stop) || (actual < start) ){
            mActualMenuElement = &(mMenu[stop]);
        }else{
            mActualMenuElement = &(mMenu[actual -1]);
        }
        return;
    }

    if(key == mKeyDown){ //nav down (scroll)
        if((actual == stop) || (actual > stop) || (actual < start) ){
            mActualMenuElement = &(mMenu[start]);
        }else{
            mActualMenuElement = &(mMenu[actual +1]);
        }
        return;
    }

    if(key == mKeyOK){ //one layer deeper in the structure or Callback
        if(mActualMenuElement->children > 0){
            mActualMenuElement = &(mMenu[getChild(getIndex(mActualMenuElement))]);
        }else{
            if(mActualMenuElement->CallBack > 0){
                #ifdef mCallbackWithParams
                mActualMenuElement->CallBack((mActualMenuElement->CallBackArgument));
                #else
                mActualMenuElement->CallBack();
                #endif
            }
        }
        return;
    }

    if(key == mKeyBack){ //one layer up.
        mActualMenuElement = &(mMenu[getParent(getIndex(mActualMenuElement))]);
        return;
    }
    return;
}

