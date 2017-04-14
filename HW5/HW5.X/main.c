//#include "PIC32.h"
#include "I2C.h"
#include "math.h"
#include <xc.h>
#include<sys/attribs.h>  // __ISR macro
#define SLAVE 0b0100000

#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = OFF // USB clock on

// DEVCFG3
#pragma config USERID = 16 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

void PIC32_Startup() {
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // A4 is a digital output
    TRISBbits.TRISB4 = 1; // B4 is input
    LATAbits.LATA4 = 1; // turn the LED on
    __builtin_enable_interrupts();
}

int main() {
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