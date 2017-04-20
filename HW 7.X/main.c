#include <xc.h>
#include "PIC32.h"
#include "LCD.h"
#include "I2C.h"
#include <stdio.h>
#include <math.h>

int main() {
    __builtin_disable_interrupts();
    // initialize board, LCD, and SPI
    PIC32_Startup();
    SPI1_init();
    LCD_init();
    i2c_master_setup();
    initExpander();
    __builtin_enable_interrupts();
    LCD_clearScreen(WHITE); // clear screen at start
    
    
    
}