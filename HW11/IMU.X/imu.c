#include "imu.h" 
#include "i2c_master_noint.h"
#include "ST7789.h"

void imu_setup(){
    unsigned char who = 0;
    //read from IMU_WHOAMI
    who = readPin(IMU_ADDR,IMU_WHOAMI);
  
    if (who != 0b01101001) { //will stay in while loop if I2C bus isn't working
        while(1){
//        LATAbits.LATA4 = !LATAbits.LATA4;
//                _CP0_SET_COUNT(0);
//        while (_CP0_GET_COUNT() < 4800000){}
    }
    }

    //init IMU_CTRL1_XL (accelerometer register)
    //set sample rate to 1.66kHz,sensitivity to 2g,filter to 100Hz
    setPin(IMU_ADDR,IMU_CTRL1_XL,0b10000010);
    
    
    //init IMU_CTRL@_G (gyroscope register)
    //set sample rate tto 1.66Hz, senstivity to 1000 dps
    setPin(IMU_ADDR,IMU_CTRL2_G,0b10001000);
    
    //init IMU_CTRL3_C (contol register, contains IF_INC,
                    //if =1 reads out sequentially)
    setPin(IMU_ADDR,IMU_CTRL3_C,0b00000100);
   
    }

void xbar(signed short x_acc){
    int k;
    int l;
    
    //normalize the x_acc so the full bar fits on the screen
    x_acc = x_acc / 100;
    
     //redraw the relevant background (much faster than clearing the entire screen)
    for (k=0;k<240;k++){
        for (l=0;l<3;l++){
            LCD_drawPixel(k,100+l,0x0000); 
        }
    }
    
    //draw acceleration bar
    if (x_acc > 0){
        for (k=0;k<=x_acc;k++){
            for (l=0;l<3;l++){
                LCD_drawPixel(100+k,100+l,0x0F); 
                } //end of inner for loop
            } //end of outer for loop
    }
    else{
        for (k=0;k<=-x_acc;k++){
            for (l=0;l<3;l++){
                LCD_drawPixel(100-k,100+l,0x0F); 
                } //end of inner for loop
            } //end of outer for loop       
    }
}

void ybar(signed short y_acc){
    int k;
    int l;
    
    //normalize the y_acc so the full bar fits on the screen
    y_acc = y_acc / 100;
    
    //redraw the relevant background (much faster than clearing the entire screen)
    for (k=0;k<240;k++){ //clear the ybar
        for (l=0;l<3;l++){
            LCD_drawPixel(100+l,k,0x0000); 
        }
    }
            
    //draw acceleration bar
    if (y_acc > 0){
        for (k=0;k<=y_acc;k++){
            for (l=0;l<3;l++){
                LCD_drawPixel(100+l,100+k,0x0F); 
                } //end of inner for loop
            } //end of outer for loop
    }
    else{
        for (k=0;k<=-y_acc;k++){
            for (l=0;l<3;l++){
                LCD_drawPixel(100+l,100-k,0x0F); 
                } //end of inner for loop
            } //end of outer for loop       
    }
}