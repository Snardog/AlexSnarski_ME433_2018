#include <xc.h>
#include "PIC32.h"
#include "LCD.h"
#include <stdio.h>
#include <math.h>

int main() {
    __builtin_disable_interrupts();
    // initialize board, LCD, and SPI
    PIC32_Startup();
    SPI1_init();
    LCD_init();
    
    __builtin_enable_interrupts();
    LCD_clearScreen(WHITE); // clear screen at start
    char message[100];
    char loading[100];
    int length = 0;
    int delay = 960000; // 24,000,000/5Hz = 4,800,000 (changed to 25Hz because it was too slow at 5Hz)
    while(1) {
        _CP0_SET_COUNT(0);
        unsigned short bar_thick = 5; // loading bar is 5 pixels thick
        unsigned short bar_width = 0;
        unsigned short bar_x = 0;
        unsigned short bar_y = 64; // starts halfway down the screen
        int time_finish = 0;
        
        if (length >= 0) {
            bar_width = length; // length of bar
            bar_x = 14;
            draw_bar(bar_thick,bar_width,bar_x,bar_y,GREEN,WHITE);
            sprintf(loading,"Loading: %d",length);
            draw_string(loading,36,50,BLACK,WHITE);
            //delay = 960000; //25Hz delay, 100 numbers to cycle through = 4 second long loading bar
        }
        else if (length < 0) {
            bar_width = length + 100;
            bar_x = 14;
            draw_bar(bar_thick,bar_width,bar_x,bar_y,RED,WHITE);
            sprintf(loading,"Error: Restarting"); // added an error message after bar is finished for fun
            draw_string(loading,24,50,RED,WHITE);
            //delay = 240000; //100Hz delay, 100 numbers to cycle through = 1 second long error message
        }
        if (length == 100) {
            sprintf(loading,"                  "); // clear text above bar after a cycle
            draw_string(loading,24,50,BLACK,WHITE);
            length = -length; // reset length to be -100
        }
        if (length == 0) {
            sprintf(loading,"                  "); // clear text above bar after a cycle
            draw_string(loading,24,50,BLACK,WHITE);
        }
        
        
        
        sprintf(message,"Hello World");
        draw_string(message,39,32,BLACK,WHITE);
        
        time_finish = _CP0_GET_COUNT();
        float fps = 24000000/time_finish;
        sprintf(message,"FPS: %.1f",fps);
        draw_string(message,42,80,BLACK,WHITE);
        length++;
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < delay) {
            ; // 5Hz delay
        }
        

        
    }  
    }
	    

        

    