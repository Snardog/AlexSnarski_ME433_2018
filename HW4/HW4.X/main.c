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
#pragma config USERID = 16 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#include <math.h>
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#define NUMSAMPS 100
#define CS LATAbits.LATA0       // chip select pin
#define pi 3.1415

void setVoltage(char channel, char voltage);

static unsigned int sineWave[NUMSAMPS];
static unsigned int sawWave[NUMSAMPS];

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
    
//    T2CKRbits.T2CKR = 0x0000; //set RA0 as timer 2
//    T2CONbits.TCKPS = 0; //set prescaler 1:1
//    PR2 = 47999; //roll over at 48000, 1000Hz
//    TMR2 = 0; //initialize count to 0
//    T2CONbits.ON = 1; //timer2 on
//    IPC2CLR = 0x1F;
//    IPC2bits.T2IP = 5; //priority 5
//    IPC2bits.T2IS = 0; //sub-priority 0
//    IFS0bits.T2IF = 0; //clear interrupt flag
//    IEC0bits.T2IE = 1; //enable interrupt
    
    
    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // A4 is a digital output
    TRISBbits.TRISB4 = 1; // B4 is input
    LATAbits.LATA4 = 1; // turn the LED on
    __builtin_enable_interrupts();
    
    char channelA = 0;
    char channelB = 1;
    unsigned int sine;
    unsigned int saw;
    int count = 0;
    int x = 0;
    while (1) {
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 48000000/2000) {
        ;//do nothing
        }
        sine = (127/2.0) + (127/2.0)*sin(2.0*3.14*count/NUMSAMPS);
        setVoltage(0, sine);
        saw = 256*x/NUMSAMPS;
        setVoltage(1, saw);
        count = count + 2;
        x++;
        if (x == NUMSAMPS) {
            x = 0;
        }
        if (count == NUMSAMPS) {
            count = 0;
        }
        
    }
    }



void setVoltage(char channel, char voltage) {
    short data = 0b0000000000000000; //16 bit number
    short b1;
    short b2;
//    data |= channel << 15; //puts the channel on the 16th bit
//    data |= 0b111 << 12; // set next 3 config bits to 1
//    data |= voltage << 4; // voltage stored in next 8 bits
    b1 = channel << 7;
    b1 = b1 | 0b1110000;
    b1 = b1 | (voltage >> 4);
    //b2 = voltage << 4;
            
       
	CS = 0;
//    char configbits = channel << 3 | 0b0111;
//    unsigned short word = (configbits << 12) | (voltage << 4);
    
    SPI1_IO(b1);
    SPI1_IO(voltage << 4);
    CS = 1;
}

void initSPI1() {
    //ANSELAbits.ANSA0 = 0; //turn off analog for A0
    TRISAbits.TRISA0 = 0; //set A0 as output
    SDI1Rbits.SDI1R = 0b0000; //set A1 as SDI1 (input, not used)
    //RPA0Rbits.RPA0R = 0b0011; //set A0 as SS1 (output))
    RPB8Rbits.RPB8R = 0b0011; //set B8 as SDO1 (output)
    
    CS = 1;
    SPI1CON = 0;              // turn off the spi module and reset it
	
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0x300;            // baud rate to 10 MHz [SPI4BRG = (80000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 
}

void makeWaveform() {
    // 10Hz sin wave
    double A = 200;
    double center = 200;
    int i = 0.0;
    double x = 0;
    unsigned int temp;
    for (i = 0; i < NUMSAMPS; i++) {
        temp = (center/2.0) + (A/2.0)*sin(2.0*3.14*i/NUMSAMPS);
        sineWave[i] = temp;
        temp = x*255.0/NUMSAMPS;
        sawWave[i] = temp;
        x = x + 1.0;
    }
}



//notes from class 4/11/17
//10k pull up to 3.3 for SCL and SDA
//Conditions:
//  Start
//  Write
//      1st write: address << 1 | 1
//      2nd write: data (8bits)
//  Stop
//
//  Start
//  Write
//      Write address << 1 | w
//  Restart
//  Write
//      address << 1 | R
//  Read
