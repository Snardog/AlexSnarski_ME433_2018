#include "PIC32.h"
#include <xc.h>




//void PIC32_Startup() {
//    __builtin_disable_interrupts();
//
//    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
//    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
//
//    // 0 data RAM access wait states
//    BMXCONbits.BMXWSDRM = 0x0;
//
//    // enable multi vector interrupts
//    INTCONbits.MVEC = 0x1;
//
//    // disable JTAG to get pins back
//    DDPCONbits.JTAGEN = 0;
//
//    // do your TRIS and LAT commands here
//    TRISAbits.TRISA4 = 0; // A4 is a digital output
//    TRISBbits.TRISB4 = 1; // B4 is input
//    LATAbits.LATA4 = 1; // turn the LED on
//    __builtin_enable_interrupts();
//}