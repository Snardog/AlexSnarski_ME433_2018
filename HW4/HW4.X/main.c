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
#define NUMSAMPS 1000
#define CS LATBbits.LATB7       // chip select pin

static int Waveform1[NUMSAMPS];
static int Waveform2[NUMSAMPS];


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
	
}



void setVolatage(char channel, char voltage) {
	
}

void initSPI1() {
	RPB7Rbits.RPB7R = 0b0011; //RB7 is SS1 (pin 16)
    RPB15Rbits.RPB15R = 0b0101; //RB15 is OC1 (pin 25)
    RPB8Rbits.RPB8R = 0b0011; //RB8 is SDO1 (pin 17)
    SDI1Rbits.SDI1R = 0b0000; // RA1 is SDI1 (pin 3)
}

void makeWaveform() {
    // 10Hz sin wave
    int A = 1.5;
    int center = 1.5;
    int i = 0;
    for (i = 0; i < NUMSAMPS; i++) {
        Waveform1[i] = center + A*sin(20*pi*i/NUMSAMPS)
    }
    // 5Hz triangle wave
    int counter;
    int x = 0;
    for (i = 0; i < NUMSAMPS; i++) {
        if (counter == NUMSAMPS/5) {
            x = 0;
            counter = 0;
        }
        Waveform2[i] = 3*x/(NUMSAMPS/5);
        x++;
        counter++;
    }
}   
