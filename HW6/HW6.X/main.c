#include <xc.h>
#include "PIC32.h"
#include "LCD.h"
#include <stdio.h>


int main() {
    __builtin_disable_interrupts();
    PIC32_Startup();
    LCD_init();
    SPI1_init();
    __builtin_enable_interrupts();
    LCD_clearScreen(0x07FF);
    //LCD_drawPixel(64,64,CYAN);
    char message[10];
    sprintf(message,"h");
    int i = 0;
//    while (message[i]) {
//        //char array[5] = ASCII['h'-0x20];
//        //LCD_drawPixel(1,1,0);
//    
//    
//    }
    while (1) {
        
    }
    return 0;
    
}