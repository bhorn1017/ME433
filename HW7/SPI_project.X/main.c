#include "spi.h"
#include "make_wave.h"
#include "UART.h"


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
    
    
    
    initSPI(); //initialize for SPI

    __builtin_enable_interrupts();
    
    //create arrays to store the wave values 
    unsigned short sin_array[50];
    int j;
    for (j=0;j<50;j++){
        sin_array[j]=make_sin(j);
    }
    unsigned short triangle_array[100];
    int k;
    for (k=0;k<100;k++){
        triangle_array[k]=make_triangle(k);
    }    
        
//declare counting variables
unsigned char i = 0; 
unsigned char l = 0;

    while (1) {
        
        unsigned char p1;
        unsigned char p2;
        unsigned char q1;
        unsigned char q2;
        
        p1 = (wave_to_ADC(0,triangle_array[i]) >> 8); //first 8 bits
        p2 = wave_to_ADC(0,triangle_array[i]); //second 8 bits 
        q1 = (wave_to_ADC(1,sin_array[l]) >> 8); //repeat for other wave
        q2 = wave_to_ADC(1,sin_array[l]);
        
        LATAbits.LATA0 = 0; //bring CS low
                //CS=low means that device is selected for communication
        spi_io(p1);
        spi_io(p2);
        LATAbits.LATA0 = 1; //bring CS high (communication is over)
        
        //repeat for other wave
        LATAbits.LATA0 = 0; //bring CS low
        spi_io(q1);
        spi_io(q2);
        LATAbits.LATA0 = 1; //bring CS high (communication is over

        //increment the counting variables
        i++;
        if (i==99) {
            i=0;
        }
        l++;
        if (l==49) {
            l=0;
        }
        _CP0_SET_COUNT(0); 
        while (_CP0_GET_COUNT() < 480000 /2 ){ //delay for .01 seconds  
         //sysclk=48MHz -> CPU clock = 48MHz/2=24Mhz
        } 
        
        }
        {
    }//end of infinite while loop
}//end of main

