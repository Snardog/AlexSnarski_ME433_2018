// Demonstrates spi by accessing external ram
// PIC is the master, ram is the slave
// Uses microchip 23K256 ram chip (see the data sheet for protocol details)
// SDO4 -> SI (pin F5 -> pin 5)
// SDI4 -> SO (pin F4 -> pin 2)
// SCK4 -> SCK (pin B14 -> pin 6)
// SS4 -> CS (pin B8 -> pin 1)
// Additional SRAM connections
// Vss (Pin 4) -> ground
// Vcc (Pin 8) -> 3.3 V
// Hold (pin 7) -> 3.3 V (we don't use the hold function)
// 
// Only uses the SRAM's sequential mode

// DEVCFG0
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
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#include <math.h>
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#define NUMSAMPS 1000
#define CS LATBbits.LATB7       // chip select pin
#define pi 3.1415

void setVoltage(char channel, char voltage);

static int sineWave[NUMSAMPS];
static int sawWave[NUMSAMPS];

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) spi(void) {
    static int count = 0;
    
    setVoltage(0,sineWave[count]);
    setVoltage(1,sawWave[count]);
    count++;
    if (count == NUMSAMPS) {
        count = 0;
    }
    
    
    IFS0bits.T2IF = 0;              // clear interrupt flag
}

unsigned char SPI1_IO(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

int main(void) {
	initSPI1();
	makeWaveform();
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    T2CKRbits.T2CKR = 0x0000; //set RA0 as timer 2
    T2CONbits.TCKPS = 0; //set prescaler 1:1
    PR2 = 47999; //roll over at 48000, 1000Hz
    TMR2 = 0; //initialize count to 0
    T2CONbits.ON = 1; //timer2 on
    IPC2CLR = 0x1F;
    IPC2bits.T2IP = 5; //priority 5
    IPC2bits.T2IS = 0; //sub-priority 0
    IFS0bits.T2IF = 0; //clear interrupt flag
    IEC0bits.T2IE = 1; //enable interrupt
    
    
    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // A4 is a digital output
    TRISBbits.TRISB4 = 1; // B4 is input
    LATAbits.LATA4 = 1; // turn the LED on
    __builtin_enable_interrupts();
    
    
    
	return 0;
    while (1) {
    }
}



void setVoltage(char channel, char voltage) {
	CS = 0;
    SPI1_IO(channel);
    SPI1_IO(0x7);
    SPI1_IO(voltage);
    CS = 1;
}

void initSPI1() {
    SPI1CON = 0;              // turn off the spi module and reset it
	RPB7Rbits.RPB7R = 0b0011; //RB7 is SS1 (pin 16)
    RPB15Rbits.RPB15R = 0b0101; //RB15 is OC1 (pin 26)
    RPB8Rbits.RPB8R = 0b0011; //RB8 is SDO1 (pin 17)
    SDI1Rbits.SDI1R = 0b0000; // RA1 is SDI1 (pin 3)
    
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 1;            // baud rate to 10 MHz [SPI4BRG = (80000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 
}

void makeWaveform() {
    // 10Hz sin wave
    int A = 1.5;
    int center = 1.5;
    int i = 0;
    for (i = 0; i < NUMSAMPS; i++) {
        sineWave[i] = center + A*sin(20*pi*i/NUMSAMPS);
    }
    // 5Hz triangle wave
    int counter;
    int x = 0;
    for (i = 0; i < NUMSAMPS; i++) {
        if (counter == NUMSAMPS/5) {
            x = 0;
            counter = 0;
        }
        sawWave[i] = 3*x/(NUMSAMPS/5);
        x++;
        counter++;
    }
}