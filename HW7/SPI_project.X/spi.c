#include "spi.h"

void initSPI() {
    //pin summary: CS:pin2, SCK: pin 25, SDO: pin 3, SDI: pin 14
     
    // Pin B14 has to be SCK1 (B14= pin 25)
    // Turn off analog pins (because analog takes priority over reprogrammable pins)
    ANSELA = 0; //setting to 1 turns on analog (the default)
    // Make an output pin for CS (low=communicating with selected device; high=not)
    TRISAbits.TRISA0 = 0; //make A0 an output
    LATAbits.LATA0 = 1; //set A0 to high (so default is no communication)
    // Set SDO1 (Table 3: RPA1R=pin 3) (reprogrammable bits)
    RPA1Rbits.RPA1R = 0b0011; //using output pin selection table 11-2
    // Set SDI1  
    SDI1Rbits.SDI1R = 0b0001; //using input pin selection table 11-1
    //0b0001 = RPB5 = pin 14 (from Table 3)
    
    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1000; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}


// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o; //store the input data into a buffer
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF; //SPI doesn't only send data; it also receives
}