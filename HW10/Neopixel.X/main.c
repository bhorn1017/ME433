#include "ws2812b.h"
#include "UART.h" //for debugging

//********start of pragmas*********
// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use internal oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // internal RC
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
//********end of pragmas********

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

//    //set pins to enable UART   //commented out so B6 can be used for I/O
//    U1RXRbits.U1RXR = 0b0001; //U1RX is B6
//    RPB7Rbits.RPB7R = 0b0001; //U1TX is B7
//    
//    //turn on UART3 without an interrupt (copied from NU32.c)
//    U1MODEbits.BRGH = 0; //set baud to NU3@_DESIRED_BAUD
//    U1BRG = ((48e6 / 115200) / 16) - 1;
//    
//    //8 bit, no parity bit, and 1 stop bit (8N1 setup)
//    U1MODEbits.PDSEL = 0;
//    U1MODEbits.STSEL = 0;
//    
//    //configure TX and RX pins as output & input pins
//    U1STAbits.UTXEN = 1;
//    U1STAbits.URXEN = 1;
//    
//    //enabled the UART
//    U1MODEbits.ON = 1;
    
    // do your TRIS and LAT commands here (using this to test program is loaded)
    TRISBbits.TRISB4 = 1; //initialize B4 as an input
    TRISAbits.TRISA4 = 0; //initialize A4 as an output
    LATAbits.LATA4 = 0; //initialize A4 as low
    
    //initial set up while interrupts are disabled
    ws2812b_setup();
    
    //end of I2C setup        
    __builtin_enable_interrupts();
    
    wsColor c[4]; //initialize the struct to hold the RGB arrays
       
    //reset the lights before use
    LATBbits.LATB6 = 0; 
    TMR2 = 0;
    while(TMR2 < 2400){} // wait 50uS, reset condition
    
    //initialize the counting variables, offset by 90 degrees
     int i1=0;
     int i2=90;
     int i3=180;
     int i4=270;
    while (1) { //enter the infinite while loop
       //fill the RBG struct arrays 
       c[0] = HSBtoRGB(i1,1,1); 
       c[1] = HSBtoRGB(i2,1,1);
       c[2] = HSBtoRGB(i3,1,1);
       c[3] = HSBtoRGB(i4,1,1);
       
       //send the struct and number of LEDS to the function
       ws2812b_setColor(c,4);
       
       //increment the counting variables
       if (i1==350){ 
           i1=0;
       }
       else{
           i1 = i1 + 10;
       }
       if (i2==350){ 
           i2=0;
       }
       else{
           i2 = i2 + 10;
       }
       if (i3==350){ 
           i3=0;
       }
       else{
           i3 = i3 + 10;
       }
       if (i4==350){ 
           i4=0;
       }
       else{
           i4 = i4 + 10;
       }
       
       //delay for 0.1s before changing colors again
        _CP0_SET_COUNT(0); 
        while (_CP0_GET_COUNT() < 4800000 /2 ){ 
        } //end of delay loop
    }//end of infinite while loop
}//end of main

