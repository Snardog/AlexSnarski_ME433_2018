#include <xc.h>
#include "PIC32.h"
#include <stdio.h>

void main() {
    PIC32_Startup();
    TRISBbits.TRISB4 = 1;
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 1;
    while (1) {
        if (PORTBbits.RB4 == 1) {
            LATAbits.LATA0 = 1;
        }
        else {
            LATAbits.LATA0 = 0;
        }
    }
}