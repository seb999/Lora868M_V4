/**
  Generated Pin Manager File

  Company:
    Microchip Technology Inc.

  File Name:
    pin_manager.c

  Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F1709
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45
        MPLAB             :  MPLAB X 4.15

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include <xc.h>
#include "pin_manager.h"
#include "stdbool.h"
#include "../main.h"




void (*IOCCF1_InterruptHandler)(void);


void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATA = 0x00;
    LATB = 0x60;
    LATC = 0x00;

    /**
    TRISx registers
    */
    TRISA = 0x24;
    TRISB = 0x80;
    TRISC = 0x92;

    /**
    ANSELx registers
    */
    ANSELC = 0x84;
    ANSELB = 0x10;
    ANSELA = 0x00;

    /**
    WPUx registers
    */
    WPUB = 0x00;
    WPUA = 0x00;
    WPUC = 0x00;
    OPTION_REGbits.nWPUEN = 1;

    /**
    ODx registers
    */
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;

    /**
    SLRCONx registers
    */
    SLRCONA = 0x37;
    SLRCONB = 0xF0;
    SLRCONC = 0xFF;


    /**
    IOCx registers 
    */
    //interrupt on change for group IOCCF - flag
    IOCCFbits.IOCCF1 = 0;
    //interrupt on change for group IOCCN - negative
    IOCCNbits.IOCCN1 = 0;
    //interrupt on change for group IOCCP - positive
    IOCCPbits.IOCCP1 = 1;



    // register default IOC callback functions at runtime; use these methods to register a custom function
    IOCCF1_SetInterruptHandler(IOCCF1_DefaultInterruptHandler);
   
    // Enable IOCI interrupt 
    INTCONbits.IOCIE = 1; 
      
    SSPDATPPS = 0x02;   //RA2->MSSP:SDI;     
    RA1PPS = 0x12;   //RA1->MSSP:SDO;     
    SSPCLKPPS = 0x00;   //RA0->MSSP:SCK;    
    RA0PPS = 0x10;   //RA0->MSSP:SCK;   
    
    RXPPS = 0x14;   //RC4->EUSART:RX;   
    RA4PPS = 0x0;   //RA4->I/O;    
    RC5PPS = 0x14;   //RC5->EUSART:TX;   
}
  
void PIN_MANAGER_IOC(void)
{   
	// interrupt on change for pin IOCCF1
    if(IOCCFbits.IOCCF1 == 1)
    {
        IOCCF1_ISR();  
    }	
}

/**
   IOCCF1 Interrupt Service Routine
*/
void IOCCF1_ISR(void) {
    // Call the interrupt handler for the callback registered at runtime
    if(IOCCF1_InterruptHandler)
    {
        IOCCF1_InterruptHandler();
    }
    IOCCFbits.IOCCF1 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF1 at application runtime
*/
void IOCCF1_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCCF1_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF1
*/
void IOCCF1_DefaultInterruptHandler(void){
    Inter_adxl();
    // add your IOCCF1 interrupt custom code
    // or set custom function using IOCCF1_SetInterruptHandler()
}

/**
 End of File
*/