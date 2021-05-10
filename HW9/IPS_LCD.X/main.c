#include <xc.h>
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include "ST7789.h"
#include "spi.h"
#include "font1.h"   

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

    // do your TRIS and LAT commands here (using this to test program is loaded)
    TRISBbits.TRISB4 = 1; //initialize B4 as an input
    TRISAbits.TRISA4 = 0; //initialize A4 as an output
    LATAbits.LATA4 = 0; //initialize A4 as low
    
    //initial set up while interrupts are disabled
    initSPI();
    LCD_init();

    __builtin_enable_interrupts();
    
    //clear the screen before starting
    LCD_clearScreen(0x0000);
    
    //variable initialization for while loop
    char m[20]; //initialize arrays to store string
    char n[20];
    int j=0; //while loop index 
    int k=0; //for loop indeces
    int l=0;
    unsigned short time1=0; //FPS calculation variables
    unsigned long time2=0;
    float difference;
    

    while (1) { //enter the infinite while loop
        
        //print statement and counter from 0-99
        sprintf(m,"Hello world %d!",j);
        drawString(28,32,0xFF,m);
        
        //print FPS
        sprintf(n,"FPS = %f",difference);
        drawString(28,100,0xFF,n);
        
        //draw loading bar
        for (k=0;k<=j;k++){
            for (l=0;l<5;l++){
                LCD_drawPixel(28+k,40+l,0x0F); 
            } //end of inner for loop
        } //end of outer for loop
        for (k=100;k>j;k--){
            for (l=0;l<5;l++){
                LCD_drawPixel(28+k,40+l,0xFFFF);  
            } //end of inner for loop
        } //end of outer for loop
                
        //calculate FPS
        
        LATAbits.LATA4 = !LATAbits.LATA4; //heartbeat LED
        
        time2 = _CP0_GET_COUNT();
        difference = 24000000.0 / (time2-time1) ; //convert to Hz
        
        _CP0_SET_COUNT(0);
        time1 = _CP0_GET_COUNT();
        //delay for ???? seconds
        while (_CP0_GET_COUNT() < 30000 ){ 
        } //end of delay loop
        
        //increment the loop index
        if (j==99){ //j goes from 0 to 99
            j=0;
        }
        else{
            j++;
        }
        
    }//end of infinite while loop
}//end of main

