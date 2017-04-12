#include "PIC32.h"
#include "I2C.h"
#include "math.h"


void main(void) {
    //put vars here
    __builtin_disable_interrupts();
    PIC32_Startup();
    initExpander();
    i2c_master_setup();
    TRISBbits.TRISB2 = 0; //B2 and B3 are outputs
    TRISBbits.TRISB3 = 0;
    
    __builtin_enable_interuppts();
    
    _CP0_SET_COUNT(0);
    
    
    while (1) {
        
    }

}