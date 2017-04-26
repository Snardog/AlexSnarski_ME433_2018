// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k
#include "I2C.h"
#include <xc.h>


void i2c_master_setup(void) {
  I2C2BRG = 233;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 Fsck = 100kHz PGD = 104ns
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void initExpander(void) {
    ANSELBbits.ANSB2 = 0; //turn off analog on B2 (SDA2)
    ANSELBbits.ANSB3 = 0; //turn off analog on B3 (SCL2)
    //initialize on accelerometer
    i2c_write(SLAVE,CTRL1_XL,0b10000010); //bits 0-1: 100 Hz filter; bits 2-3: 2g sensitivity; bits 4-7: 1.66kHz sample rate
    //initialize gyroscope
    i2c_write(SLAVE,CTRL2_G,0b10001000); //bits 0: n/a; bit 1: n/a; bits 2-3: 1000dps; bits 4-7: 1.66kHz sample rate
    //initialize IF_INC
    i2c_write(SLAVE,CTRL3_C,0b00000100); //bit 3: IF_INC enabled
//    i2c_master_start();
//    i2c_master_send(SLAVE << 1); // Write 
//    i2c_master_send(0x10); // CTRL1_XL
//    i2c_master_send(0x82); // Set the sample rate to 1.66 kHz, with 2g sensitivity, and 100 Hz filter
//    i2c_master_stop();
//    // Turn on gyroscrope
//    i2c_master_start();
//    i2c_master_send(SLAVE << 1); // Write
//    i2c_master_send(0x11); // CTRL2_G
//    i2c_master_send(0x88); // Set the sample rate to 1.66 kHz, with 1000 dps sensitivity
//    i2c_master_stop();
//    // Enable reading multiple registers in a row
//    i2c_master_start();
//    i2c_master_send(SLAVE  << 1); // Write
//    i2c_master_send(0x12); // CTRL3_C
//    i2c_master_send(0x04); // Set IF_INC bit 1
//    i2c_master_stop();
}

void setExpander(unsigned char level,unsigned char pin) {
    unsigned char data = (0x01 & level) << pin; // manipulate the correct level for the pin
    i2c_write(SLAVE,0x09,data);
}

char getExpander() {
    unsigned char r = i2c_read(SLAVE,0x09);
    return r;
}

void i2c_write(unsigned char address, unsigned char registerr, unsigned char data) {
    //had to spell register with 2 r's because its already a thing
    i2c_master_start();
    i2c_master_send(address<<1|0);
    i2c_master_send(registerr);
    i2c_master_send(data);
    i2c_master_stop();
}

unsigned char i2c_read(unsigned char address, unsigned char registerr) {
    unsigned char r;
    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(registerr);
    i2c_master_restart();
    i2c_master_send(address << 1 | 1);
    r = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return r;
}

void I2C_read_multiple(unsigned char address, unsigned char registerr, unsigned char *data, int length) {
    int i = 0;
    //int ackval = 0;
    i2c_master_start();
    i2c_master_send(address << 1);
    i2c_master_send(registerr);
    i2c_master_restart();
    i2c_master_send((address << 1) | 1);
    for (i = 0; i < length; i++) {
        data[i] = i2c_master_recv();
        if ( i == length - 1) {
            i2c_master_ack(1);
        }
        else {
            i2c_master_ack(0);
        }
    }
    i2c_master_stop();
}


