/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include <xc.h>
#include "LCD.h"
#include "I2C.h"
#include <math.h>
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define SLAVE 0b1101011
#define SCALE 60.0/16383.0
#define BARWIDTH 10
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

short combine_data(char *data, int index) {
    short high_bit;
    short low_bit;
    high_bit = data[index+1];
    low_bit = data[index];
    short val = (high_bit << 8) | low_bit;
    return val;
}

void delay(int time) {
    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < time) {;}
}
// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    //static volatile signed short final[7];
    SPI1_init();
    LCD_init();
    i2c_master_setup();
    initExpander();
    TRISAbits.TRISA4 = 0; // A4 is a digital output
    TRISBbits.TRISB4 = 1; // B4 is input
    LATAbits.LATA4 = 1; // turn the LED on
    LCD_clearScreen(WHITE); // clear screen at start
            
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            static volatile unsigned char data[14];
            static int length = 14;
            unsigned char test;
            short x_val;
            unsigned char message[30];
            

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
            I2C_read_multiple(SLAVE,OUT_TEMP_L,data,length);

            temp = combine_data(data,0);
            gyro_x = combine_data(data,2);
            gyro_y = combine_data(data,4);
            gyro_z = combine_data(data,6);
            accel_x = combine_data(data,8);
            accel_y = combine_data(data,10);
            accel_z = combine_data(data,12);

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

            delay(480000);
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
