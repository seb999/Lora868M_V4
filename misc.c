#include "misc.h"
#include <stdint.h>
#include <xc.h>
#include <stdbool.h>
#include <string.h>
//#include "mcc_generated_files/eusart.h"
#include "mcc_generated_files/mcc.h"


//----------OTHERS----------
void __delay_sec(int n){
    while(n--) __delay_ms(1000); 
}

void blinkRed(int n){
    while(n--){
        LED_RED_SetHigh();
        __delay_ms(75);
        LED_RED_SetLow();
        __delay_ms(75);
    }
}

void blinkGreen(int n){
    while(n--){
        LED_GREEN_SetHigh();
        __delay_ms(75);
        LED_GREEN_SetLow();
        __delay_ms(75);
    }
}

void blinkOrange(int n){
    while(n--){
        LED_ORANGE_SetHigh();
        __delay_ms(75);
        LED_ORANGE_SetLow();
        __delay_ms(75);
    }
}

//----------EUART PIN MANAGER----------
void EUART_GPS(void)
{   
    bool state = (unsigned char)GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
    
    RXPPS = 0x14;   //RC4->EUSART:RX;   
    RA4PPS = 0x0;   //RA4->I/O;    
    RC5PPS = 0x14;   //RC5->EUSART:TX;   

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    GIE = state;
    __delay_ms(100);   
}

void EUART_LORA(void)
{
    bool state = (unsigned char)GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
    
    RA4PPS = 0x14;   //RA4->EUSART:TX;
    RXPPS = 0x05;   //RA5->EUSART:RX;
    RC5PPS = 0x0;   //RC5->I/O;
     
    //PORTCbits.RC4 = 0; //important, keep it high for GPS, otherwise send error message

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    GIE = state;
    __delay_ms(100);
}

//----------EUART EXTENTION METHODS----------
void ReadUartCmd(char *output)
{
//    memset(*output, 0, sizeof(output)/sizeof(char));
    memset(output, 0, 20);
    int i = 0;
    char token;
    token = EUSART_Read();
    while(token!='\n'){
        output[i] = token;
        token = EUSART_Read();
        i++;
    }
}

void SendUartCmd(char *cmd)
{
    for(int i=0;cmd[i] !=  '\0' ;i++)
        EUSART_Write(cmd[i]);
}

//---------GPS COMMANDS---------

void InitGPS(){
    
    //Keep only GPGLL
    SendUartCmd("$PMTK314,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    
    //Change GPS baud rate to 57600
    SendUartCmd("$PMTK251,57600*2c\r\n");
    __delay_ms(100);
}

void GpsOn(){
    SendUartCmd("$PMTK225,0*2B\r\n");
}

void GpsOff(){
    SendUartCmd("$PMTK161,0*28\r\n");
}


void ChangeBaudRate57600(){
     //Change PIC Baud Rate to 57600;
    SP1BRGL = 0x8A;
    SP1BRGH = 0x00;
    __delay_ms(500);  
}

void GPSSleepMode(){
    SendUartCmd("$PMTK,40,GGA,0,0,0,0\r\n");
   // $PMTK161,0*28<CR><LF>
}

//--------------Decimal / Hex ----------
char * NormalizeHex(char *hex){
    char norm[20]="";
    int i;
    for (i = 0; i < hex[i]; i++) {};
    if(i==1) strcpy(norm, "000");
    if(i==2) strcpy(norm, "00");
    if(i==3) strcpy(norm, "0");
    strcat(norm, hex);
    return norm;
}