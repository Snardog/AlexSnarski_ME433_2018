#ifndef I2C__H__
#define I2C__H__
// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts

#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop
void initExpander(void);
void setExpander(unsigned char level, unsigned char pin);
char getExpander();
void i2c_write(unsigned char address, unsigned char registerr, unsigned char data);
unsigned char i2c_read(unsigned char address, unsigned char registerr);
#endif