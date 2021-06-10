#include "OC_PWM.h"
#define PI 3.14

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
int main(void) {
  // call initializations
        __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    //set pins to enable UART
    U1RXRbits.U1RXR = 0b0001; //U1RX is B6
    RPB7Rbits.RPB7R = 0b0001; //U1TX is B7
    
    //turn on UART3 without an interrupt (copied from NU32.c)
    U1MODEbits.BRGH = 0; //set baud to NU3@_DESIRED_BAUD
    U1BRG = ((48e6 / 115200) / 16) - 1;
    
    //8 bit, no parity bit, and 1 stop bit (8N1 setup)
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;
    
    //configure TX and RX pins as output & input pins
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;
    
    //enabled the UART
    U1MODEbits.ON = 1;
    
    // do your TRIS and LAT commands here (using this to test program is loaded)
    TRISBbits.TRISB4 = 1; //initialize B4 as an input
    TRISAbits.TRISA4 = 0; //initialize A4 as an output
    LATAbits.LATA4 = 0; //initialize A4 as low
    
    
    //end of I2C setup        
    __builtin_enable_interrupts();
  
  // set the pin you want to use to OCx
    RPB13Rbits.RPB13R = 0b0110; //set B13 to OC5
    
//calculate prescaler: (1/80e6)(PR2=50000)(N) = (1/50) -> N=32
  T2CONbits.TCKPS = 0b101;     // set the timer prescaler so that you can use the largest PR2 value as possible without going over 65535 and the frequency is 50Hz
  // possible values for TCKPS are 0 for 1:1, 1 for 1:2, 2 for 1:4, 3 for 1:8, 4 for 1:16, 5 for 1:32, ...
  PR2 = 50000;              // max value for PR2 is 65535
  TMR2 = 0;                // initial TMR2 count is 0
  OC5CONbits.OCM = 0b110;  // PWM mode without fault pin; other OCxCON bits are defaults
  //calculate OC5RS for 0 degrees: (.5e-3)/(1/50)=.025=2.5% duty cycle
  //(.025)(50000) = 1250
  OC5RS = 1250;             // duty cycle = OCxRS/(PR2+1)
  OC5R = 1250;              // initialize before turning OCx on; afterward it is read-only
  T2CONbits.ON = 1;        // turn on Timer2
  OC5CONbits.ON = 1;       // turn on OCx
  
  //initialize indices for the infinite while loop
  int ii = 0;
  while (1) {
      pos_to_duty(make_sin(ii));     
      ii++;
      if (ii > 199){ //reset the index to 0 once it reaches 200
          ii=0;
      }
  }
}


void pos_to_duty(float pos){
    //input: position (degrees)
    //applies the correct duty cycle to the PWM
    
    //translate the position value from 0-180 degrees
    //to a time value between 0.5e-3s and 2.5e-3s
    float time; // stores this time value of 0.5e-3 to 2.5e-3
    time = 0.5e-3 + (pos/180)*(2e-3);
    
    //turn this time value into a PWM value
    int OC_value; // stores the OC value 
    OC_value = (time / .02) * 50000;
        //set the new OC5RS value
    OC5RS = OC_value;    
}

 float make_sin(int x){ //sin wave of 2 Hz 
 //should have 50Hz/0.5Hz=100 values for one cycle, or 200 values for 2 cycles
    int ii ;
    unsigned short sin_array[200];
    for (ii=0; ii<199; ii++){
        sin_array[ii] = (80*sin(2*PI*(ii+1)/200)) + 90;
     //multiply by 80 and offset by 90 so the sin wave goes from
     //10 to 170
    } //end of for loop
    return sin_array[x]; //return the x value of the sin array
 } //end of make_sin() 