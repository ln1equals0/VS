
//#include <inttypes.h>
#include "user_string.h"

//collection of most often used string funcitons, simplified and ported
//for use in limited environment - such as microcontrollers.

void inttostr16(char * target, unsigned int source, unsigned char digits){
	unsigned char		i = 0;
	unsigned char		position = 0;
	unsigned char		num = 0;

	position = digits - 1;

	for(i = 0 ; i < digits ; i++){

		num = source % 10;
		source /= 10;

		target[position] = (48 + num);

		position--;

	}

	target[digits] = 0;

}


void inttostr8(char * target, unsigned char source, unsigned char digits){
	unsigned char		i = 0;
	unsigned char		position = 0;
	unsigned char		num = 0;

	position = digits - 1;

	for(i = 0 ; i < digits ; i++){

		num = source % 10;
		source /= 10;

		target[position] = (48 + num);

		position--;

	}

	target[digits] = 0;

}


void str_cpy_p(char * t, const char * text){
    unsigned char i = 0, j = 0;

    j = str_len(t);

    for(i = 0 ; i < j+1 ; i++){
        t[i] = (char) text[i];
    }

    t[j+1] = (char) 0;
}


void str_cpy(char * t, char * text){
    unsigned char i = 0, j = 0;

    j = str_len(t);

    for(i = 0 ; i < j+1 ; i++){
        t[i] = (char) text[i];
    }

    t[j+1] = (char) 0;
}


void str_cat(char * a, char * b){
    unsigned char     i = 0, j = 0;

    j = str_len(a);

    for(i = 0 ; i < 255 ; i++){
        if(b[i] != (char) 0){
            a[j] = b[i];
            j++;
        }else{
            break;
        }
    }

    a[j] = (char) 0;
}

void str_cat_v(volatile unsigned char * a, char * b){
    unsigned char     i = 0, j = 0;

    j = str_len_pv(a);

    for(i = 0 ; i < 255 ; i++){
        if(b[i] != (char) 0){
            a[j] = b[i];
            j++;
        }else{
            break;
        }
    }

    a[j] = (char) 0;
}


void str_cat_p(char * a, const char * b){
    unsigned char     i = 0, j = 0;

    j = str_len(a);

    for(i = 0 ; i < 255 ; i++){
        if(b[i] != (char) 0){
            a[j] = b[i];
            j++;
        }else{
            break;
        }
    }

    a[j] = (char) 0;
}

void str_cat_pv(volatile unsigned char * a, const char * b){
    unsigned char     i = 0, j = 0;

    j = str_len_pv(a);

    for(i = 0 ; i < 255 ; i++){
        if(b[i] != (char) 0){
            a[j] = b[i];
            j++;
        }else{
            break;
        }
    }

    a[j] = (char) 0;
}

unsigned char str_len_pv(volatile unsigned char * t){
    unsigned char i = 0;

    for(i = 0 ; i < 255 ; i++){
        if(t[i] == (char) 0){
            return i;
        }
    }

    return 0;
}


unsigned char str_len(char * t){
    unsigned char i = 0;

    for(i = 0 ; i < 255 ; i++){
        if(t[i] == (char) 0){
            return i;
        }
    }

    return 0;
}

unsigned char str_len_p(const char * t){
    unsigned char i = 0;

    for(i = 0 ; i < 255 ; i++){
        if(t[i] == (char) 0){
            return i;
        }
    }

    return 0;
}


