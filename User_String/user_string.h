
#ifndef STRING_H_
#define STRING_H_

void inttostr16(char * target, unsigned int source, unsigned char digits);
void inttostr8(char * target, unsigned char source, unsigned char digits);

void str_cpy_p(char * t, const char * text);
void str_cpy(char * t, char * text);

void str_cat(char * a, char * b);
void str_cat_v(volatile unsigned char * a, char * b);
void str_cat_p(char * a, const char * b);
void str_cat_pv(volatile unsigned char * a, const char * b);

unsigned char str_len(char * t);
unsigned char str_len_p(const char * t);
unsigned char str_len_pv(volatile unsigned char * t);

#endif /*STRING_H_*/
