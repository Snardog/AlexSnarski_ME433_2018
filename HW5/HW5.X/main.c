#include "PIC32.h"
#include "I2C.h"
#include "math.h"
#include <xc.h>
#define SLAVE 0b0100000
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
    //i2c_write(SLAVE,0x0A,0b00000011);
    while (1) {
        //Button defaults high, goes low when pressed
        if (getExpander(7) == 1) {
            setExpander(0,0); //LED off when button is not pressed
            while (getExpander(7)) {;} //Keep led off until button is pressed
        }
        else { 
            setExpander(1,0); // turn LED on when buton is pressed
            while (!getExpander(7)) {;} //Keep LED on while button is pressed 
        }
    }
}