#include "make_wave.h"
#include "UART.h"
#define PI 3.14

 float make_sin(int x){ //sin wave of 2 Hz 
 //should have 50 values since the DAC is running at 100Hz
    //generate the sin wave array
    int ii ;
    unsigned short sin_array[50];
    for (ii=0; ii<50; ii++){
        sin_array[ii] = (512 * sin(2*PI*(ii+1)/50)) + 512;
     //multiply by 1024 bc DAC resolution is 2^10=1024
    } //end of for loop
    return sin_array[x]; //return the x value of the sin array
 } //end of make_sin() 
 
  float make_triangle(int y){ //triangle wave of 1Hz
      //should have 100 values since the DAC is running at 100Hz
 
    //generate the triangle wave array
    int jj ;
    unsigned short triangle_array[100];
    for (jj=0; jj<100; jj++){
        if (jj > 49){
            triangle_array[jj] = 1024 * .02 *(jj+1);
        }
        else{
            triangle_array[jj] = 1 - 1024 * .02 *(jj-49);
        }  
     //multiply by 1024 bc DAC resolution is 2^10=1024
    } //end of for loop
    return triangle_array[y];
 } //end of make_triangle() 
 
 unsigned short wave_to_ADC(unsigned char channel,unsigned short value){
     /*inputs:
     -the name of the channel (either A=0 or B=1)
     -the voltage, from 0 to 1023 (uses the full 2^10 bit resolution)
     
    output:
    -p: the 16 bit number to be sent to the DAC via SPI 
      */
     
    unsigned short p; //declare the output p
    
    //write the 0th bit of p (the one all the way on the left in this case)
    p = channel<<13;
     
    
    //write the 1st,2nd, and 3rd bits of p
    p = p | (0b111<<10);
    
            
    //write the final 12 bits of p
    p = p | value;
    
    //add the two ignored bits at the end
    p = p << 2;
    
    return p;
 } 

