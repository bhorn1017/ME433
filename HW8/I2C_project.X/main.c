#include "i2c_master_noint.h"
#define address 0b000 //the address of the IO expander (set by its pin A0,A1,A2)

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
    
    //initial I2C setup:
    
    //initialize the pins
    i2c_master_setup(); //initializes I2C
    
    //initialize Port A pins as output pins
    //IODIRA (sets Port A pins as input or output) register address=0x00
    setPin(0x00,0x00,0x00); //settings all Port A pins as outputs

    //initialize port B pins as input pins
    //IODIRB (sets Port B pins as input or output) register address=0x01
    setPin(address,0x01,0xFF); //settings all Port B pins as inputs
    
    //initialize Port A pins (outputs) as low using OLATA register 
    //OLATA address=0x14)
    setPin(address,0x14,0x00);
    
    //end of I2C setup        
    __builtin_enable_interrupts();
    

    while (1) {
        
        //blink the LED on and off (for testing)
        LATAbits.LATA4 = !LATAbits.LATA4; 
        
        //if the button is pushed (GPA0=low) then turn on 
        //GPA7 (turn the LED on)
        //reading from bit 0 of GPIOB(address=0x13)
        unsigned char pushed;
        pushed = readPin(address,0x13);
        
        //bit shift to isolate the 0th bit 
        pushed = pushed << 7;
        if (pushed == 0){ //low=button is pushed
            //blink the LED on pin GPA7 (using OLATA again)
            //GPA7 is the 7th bit in the register
            setPin(address,0x14,0b10000000);
        }
        else {  //button is not being pushed
            setPin(address,0x14,0b00000000);
            
        }

        _CP0_SET_COUNT(0); 
        while (_CP0_GET_COUNT() < 4800000 /2 ){ //delay for 0.1 seconds  
         //sysclk=48MHz -> CPU clock = 48MHz/2=24Mhz
        } //end of delay loop
         
    }//end of infinite while loop
}//end of main

