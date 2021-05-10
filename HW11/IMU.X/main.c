#include "i2c_master_noint.h"
#include "imu.h"
#include "spi.h"
#include "ST7789.h"
#include "font1.h"
#include <stdio.h>


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
    
    //do initializations
    i2c_master_setup(); //initializes I2C
    imu_setup(); //setup IMU
    //LCD screen initialization
    initSPI();
    LCD_init();
    
         
    __builtin_enable_interrupts();
    
    //clear the screen before starting
    LCD_clearScreen(0x0000);
        
    //variable initialization for while loop
    signed short short_data[7]; //to store data
    char m[20]; //for sprintf
    unsigned short time1=0; //FPS calculation variables
    unsigned long time2=0;
    float difference;
    
    while (1) {
                
        i2c_master_read_multiple(IMU_ADDR,IMU_OUT_TEMP_L,short_data,14);
        
        //make if statement=1 to display the data
        if (0){
            //print out the data values
            //print out the angular velocity data
            sprintf(m, "g: %d %d %d ", short_data[1],short_data[2],short_data[3]);
            drawString(10,10,0xFFFF,m);
            //print out the acceleration data
            sprintf(m, "a: %d %d %d ", short_data[4],short_data[5],short_data[6]);
            drawString(10,20,0xFFFF,m);
            //print out the temperature data
            sprintf(m, "t: %d ", short_data[0]);
            drawString(10,30,0xFFFF,m);
        }
        
        //make if statement=1 to display the bar
        if (1){
            LCD_clearScreen(0x0000);
            xbar(short_data[5]);
            ybar(short_data[4]);
        }
           
        //calculate FPS       
        LATAbits.LATA4 = !LATAbits.LATA4; //heartbeat LED
        
        time2 = _CP0_GET_COUNT();
        difference = 24000000.0 / (time2-time1) ; //convert to Hz
        
        _CP0_SET_COUNT(0);
        time1 = _CP0_GET_COUNT();
        //delay for ???? seconds
        while (_CP0_GET_COUNT() < 30000 ){ 
        } //end of delay loop
                            
        
    }//end of infinite while loop
}//end of main

