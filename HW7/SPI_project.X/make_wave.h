#ifndef make_wave__H__
#define make_wave__H__

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

float make_sin(int y);
float make_triangle(int x);
unsigned short wave_to_ADC(unsigned char channel,unsigned short value);


#endif // make_wave__H__
