#include <xc.h>
#include "PIC32.h"
#include "LCD.h"
#include "I2C.h"
#include <stdio.h>
#include <math.h>
#define SCALE 60.0/16383.0
#define BARWIDTH 10
static volatile unsigned char data[14];
//static volatile signed short final[7];
static int length = 14;

short combine_data(char *data, int index) {
    short high_bit;
    short low_bit;
    high_bit = data[index+1];
    low_bit = data[index];
    short val = (high_bit << 8) | low_bit;
    return val;
}

int main() {
    __builtin_disable_interrupts();
    // initialize board, LCD, and SPI
    PIC32_Startup();
    SPI1_init();
    LCD_init();
    i2c_master_setup();
    initExpander();
    __builtin_enable_interrupts();
    
    unsigned char test;
    short x_val;
    unsigned char message[30];
    TRISAbits.TRISA0 = 0; // set A0 to output for testing
    LATAbits.LATA0 = 1; // default high
    LCD_clearScreen(WHITE); // clear screen at start
    
    short temp;
    short gyro_x;
    short gyro_y;
    short gyro_z;
    short accel_x;
    short accel_y;
    short accel_z;
    int x_index = 0;
    int y_index = 0;
    int x,y;
    int i = 0;
    int j = 0;
    while (1) {
//        test = i2c_read(SLAVE,WHOAMI);
//        sprintf(message,"WHOAMI(105) = %3d",test);
//        draw_string(message,5,5,BLACK,WHITE);
        
        I2C_read_multiple(SLAVE,OUT_TEMP_L,data,length);
        
        temp = combine_data(data,0);
        gyro_x = combine_data(data,2);
        gyro_y = combine_data(data,4);
        gyro_z = combine_data(data,6);
        accel_x = combine_data(data,8);
        accel_y = combine_data(data,10);
        accel_z = combine_data(data,12);
        
        
        
//        sprintf(message,"accel_x= %d",accel_x);
//        draw_string(message,10,15,BLACK,WHITE);
//        sprintf(message,"accel_y= %d",accel_y);
//        draw_string(message,10,25,BLACK,WHITE);
        
        //void drawBar(unsigned short x, unsigned short y, char length, char width, unsigned short color)
        if (accel_x < 0) {
            x = accel_x*SCALE*-1;
            if (x_index == 1) {
                drawBar(0,64,128,5,WHITE); //clear bar if it switches signs
            }
            drawBar(0,64,64-x,5,WHITE);
            drawBar(64-x,64,x,5,RED);
            x_index = 0;
        }
        else {
            x = accel_x*SCALE;
            if (x_index == 0) {
                drawBar(0,64,128,5,WHITE); //clear bar if it switches signs
            }
            drawBar(64,64,x,5,RED);
            drawBar(64+x,64,64-x,5,WHITE);
            x_index = 1;
        }
        
        if (accel_y < 0) {
            y = accel_y*SCALE*-1;
            if (y_index == 1) {
                drawBar(64,0,5,128,WHITE); //clear bar if it switches signs
            }
            drawBar(64,0,5,64-y,WHITE);
            drawBar(64,64-y,5,y,GREEN);
            y_index = 0;
        }
        else {
            y = accel_y*SCALE;
            if (y_index == 0) {
                drawBar(64,0,5,128,WHITE); //clear bar if it switches signs
            }
            drawBar(64,64,5,y,GREEN);
            drawBar(64,64+y,5,64-y,WHITE);
            y_index = 1;
        }
        
        delay(4800000);
    }
        
}
    
    
    
