    // I2C Master utilities, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// I2C pins need pull-up resistors, 2k-10k
#include "i2c_master_noint.h"

void i2c_master_setup(void) {
    // using a large BRG to see it on the nScope, make it smaller after verifying that code works
    // look up TPGD in the datasheet
    I2C1BRG = 100; // I2CBRG = [1/(2*Fsck) - TPGD]*Pblck - 2 (TPGD is the Pulse Gobbler Delay)
    I2C1CONbits.ON = 1; // turn on the I2C1 module
}

void i2c_master_start(void) {
    I2C1CONbits.SEN = 1; // send the start bit
    while (I2C1CONbits.SEN) {
        ;
    } // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C1CONbits.RSEN = 1; // send a restart 
    while (I2C1CONbits.RSEN) {
        ;
    } // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C1TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while (I2C1STATbits.TRSTAT) {
        ;
    } // wait for the transmission to finish
    if (I2C1STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
        // ("I2C1 Master: failed to receive ACK\r\n");
        while(1){
//            LATAbits.LATA4 = !LATAbits.LATA4;
//           _CP0_SET_COUNT(0);
//           while (_CP0_GET_COUNT() < 4800000){}
        } // get stuck here if the chip does not ACK back
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C1CONbits.RCEN = 1; // start receiving data
    while (!I2C1STATbits.RBF) {
//                LATAbits.LATA4 = !LATAbits.LATA4; 
//        _CP0_SET_COUNT(0);
//        while (_CP0_GET_COUNT() < 4800000){}
        ;
    } // wait to receive the data
    return I2C1RCV; // read and return the data
}

void i2c_master_ack(int val) { // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C1CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1; // send ACKDT
    while (I2C1CONbits.ACKEN) {
        ;
    } // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) { // send a STOP:
    I2C1CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while (I2C1CONbits.PEN) {
        ;
    } // wait for STOP to complete
}


/****up to this point everything has been copied from Nick 
 * the remainder is my own stuff */

void setPin(unsigned char address, unsigned char reg, unsigned char value){
    //general write function for I2C

    //Step 1:send the start bit
    i2c_master_start(); 
    
    //Step 2: send the address to slave
    address = address << 1; //shift to make A2 bit 3, A2 bit 2...
    address = address | 0b01000000; //bc R/W=0 for writing
    i2c_master_send(address); //send the address
    
    //Step 3: send the command (ie the register to be changed)
    i2c_master_send(reg);
    
    //Step 4: send the value
    i2c_master_send(value);
    
    //Step 5: send the stop bit
    i2c_master_stop();
}
unsigned char readPin(unsigned char address, unsigned char reg){
    //general read function for I2C

    //Step 1:send the start bit
    i2c_master_start(); 
  
    //Step 2: send the address to slave (so with the write bit)
    unsigned char address_write;
    address_write = address << 1;
    address_write = address_write | 0b0000000; //bc R/W=0 for writing
    i2c_master_send(address_write); //send the address
    
    //Step 3: send the command (ie the register to be changed)
    i2c_master_send(reg);

    //Step 4: do a Restart
    i2c_master_restart();
    
    //Step 5: send the address with the read bit
    unsigned char address_read;
    address_read = address << 1; 
    address_read = address_read | 0b00000001; //bc R/W=1 for reading
    i2c_master_send(address_read); //send the address
 
    //Step 6: Receive a byte from the slave
    unsigned char received;
    received = i2c_master_recv();
    
    //Step 7: acknowledge data has been received
    i2c_master_ack(1);
    //val=0 if more bytes should be received, val=1 if done
    //directly inputting 1 for now since we
    //only need one byte of data sent to us
          
    //Step 8: send the stop bit
    i2c_master_stop();
 
    return received; //return the byte received from the slave
}

void i2c_master_read_multiple(unsigned char address, unsigned char reg, signed short * short_data, int length){
    //general multi-read function for I2C

    //Step 1:send the start bit
    i2c_master_start(); 
  
    //Step 2: send the address to slave (so with the write bit)
    unsigned char address_write;
    address_write = address << 1; 
    address_write = address_write | 0b0000000; //bc R/W=0 for writing
    i2c_master_send(address_write); //send the address
    
    //Step 3: send the command (ie the register to be changed)
    i2c_master_send(reg);

    //Step 4: do a Restart
    i2c_master_restart();
    
    //Step 5: send the address with the read bit
    unsigned char address_read;
    address_read = address << 1; 
    address_read = address_read | 0b00000001; //bc R/W=1 for reading
    i2c_master_send(address_read); //send the address
 
    
    //Step 6/7: loop through receive and non-ack (length-1) times
    int ii; //loop index 
    unsigned char char_data[length];
    
    for (ii=0;ii<(length-1);ii++){
        char_data[ii] = i2c_master_recv();
        i2c_master_ack(0); //send a non-ack to keep going
    }
    
    //for the final cycle, send an ack
    char_data[length-1] = i2c_master_recv();
    i2c_master_ack(1);
          
    //Step 8: send the stop bit
    i2c_master_stop();
    
    //turn the unsigned char data into signed short data
    int j = 0;
    for (j=0;j<7;j++) {
        short_data[j] = (char_data[(2*j)+1] << 8) | char_data[2*j];
    }
}
