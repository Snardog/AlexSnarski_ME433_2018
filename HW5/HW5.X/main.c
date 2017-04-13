#include "PIC32.h"
#include "I2C.h"
#include "math.h"
#include <xc.h>

void main(void) {
    //put vars here
    __builtin_disable_interrupts();
    PIC32_Startup();
    initExpander();
    i2c_master_setup();
    TRISBbits.TRISB2 = 0; //B2 and B3 are outputs
    TRISBbits.TRISB3 = 0;
    
    __builtin_enable_interrupts();
    
    unsigned char i2c_read;
    
    while (1) {
        i2c_read = getExpander();
        i2c_read = i2c_read>>7; //check to see if gp7 is high or low
        if (i2c_read) {
            setExpander(1,0);
            LATAbits.LATA4 = 1;
        }
        else {
            setExpander(0,0);
            LATAbits.LATA4 = 0;
        }
        i2c_write(0b00100000,0x09,0b00000001);
    }

}