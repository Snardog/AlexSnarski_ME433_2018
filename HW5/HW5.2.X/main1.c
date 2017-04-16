#include "PIC32.h"
#include "I2C.h"
#include "math.h"
#include <xc.h>
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>

int main() {
    __builtin_disable_interrupts();
    //initialize PIC32, I2C, and expander
    PIC32_Startup();
    i2c_master_setup();
    initExpander();
    __builtin_enable_interrupts();
    
    while(1) {
        unsigned char read = getExpander();
        read = read >> 7;
        if(read == 0){ //check if button is pressed
            setExpander(1,0); //turn on LED if button is pressed
        }
        else{
            setExpander(0,0); //LED off if button is not pressed
        }
    }   
}