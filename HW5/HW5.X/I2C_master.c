// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k
#include "I2C.h"
#include <xc.h>
#define SLAVE 0b0010000 //A1,A2,A3 = 0

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
    //i2c_write(SLAVE,0x00,0b11110000); //set GP0-3 as output, GP4-7 are inputs
    //i2c_write(SLAVE,0x06,0b11110000); //turn on pull up resistors for output pins
    i2c_master_start();
    i2c_master_send(SLAVE<<1);
    i2c_master_send(0x00); // I/O register (0x00)
    i2c_master_send(0b11110000); // Pins 0-3 output, 4-7 input
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE<<1);
    i2c_master_send(0x06); // Pull up resister register (0x06)
    i2c_master_send(0b11110000); // Turn pull ups on for pins 4-7 (output pins)
    i2c_master_stop();
}

void setExpander(unsigned char level,unsigned char pin) {
    //unsigned char gpioVal = level << pin;
    unsigned char data = 0b11111111;
    if (level == 1) {
        data = 0b00000001; // Bit 0 high when button is pressed
    }
    if (level == 0) {
        data = 0b00000000; // Bit 0 low when button is pressed
    }
    //i2c_write(SLAVE,0x09,data);
    i2c_master_start();
    i2c_master_send(SLAVE<<1);
    i2c_master_send(0x09); //GPIO register (0x09)
    i2c_master_send(data);
    i2c_master_stop();
}

char getExpander(char pin) {
    unsigned char read = i2c_read(SLAVE,0x09);
    read = read >> pin;
    return read;
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
    i2c_master_send(address << 1);
    i2c_master_send(registerr);
    i2c_master_restart();
    i2c_master_send(address << 1 | 1);
    r = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return r;
}
