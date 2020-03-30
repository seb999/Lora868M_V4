#include "mcc_generated_files/mcc.h"
#include "main.h"
#include "misc.h"
#include "adxl.h"
#include <string.h>
#include <stdio.h>

bool isMotionStopped = false;
        
char lora[40]="";
char gps[40]="";
char hex[10]="";
char gpsHex[30]="";
char str[50]="";

char * token;
char * latitude;
char * longitude;
char * latitudeDec;
char * longitudeDec;

int counter = 0;
int counterMotion = 0;
int debugger = 0;

void main(void)
{
    SYSTEM_Initialize();

    INTERRUPT_GlobalInterruptDisable();
    
    SWDTEN = 1;
    CLRWDT();  
    
    InitADXL();
    InitGPS();
    ChangeBaudRate57600();
    
    blinkGreen(5);  
    blinkRed(5); 

    INTERRUPT_GlobalInterruptEnable();
    
    //SleepNow();
   
    //TImer0 interruption Off is needed to debug
    INTCONbits.TMR0IE = 0;
    
    
      EUART_LORA();
        __delay_ms(200);
        
       LORA_RESET_SetLow();
    __delay_ms(90);
    LORA_RESET_SetHigh();
    __delay_ms(90);
    
    

    while (1)
    {
        CLRWDT();
    }
}

void Inter_adxl()
{
    blinkGreen(1);
    blinkRed(1);
    
    RA4_SetLow();
     __delay_ms(500);
      RA4_SetHigh();
       __delay_ms(10);
        SP1BRGL = 0x40;

    // SP1BRGH 3; 
    SP1BRGH = 0x03;

     EUSART_Write(0x55); 
   
//      LORA_RESET_SetLow();
//    __delay_ms(90);
//    LORA_RESET_SetHigh();
//    __delay_ms(90);
//    ReadUartCmd(lora);
    
    SendUartCmd("sys get ver\r\n"); 
}

void Inter_timer()
{
    CLRWDT();
    counter++;
    if(counter==300){
        counter = 0;
        
        //1 - read GPS if available
        GpsOn();
        for(int i=0;i<=10;i++){  
            CLRWDT();
            if(ReadGPS())break; 
        }
         
        //2 - connect to LoraWan
        EUART_LORA();
        __delay_ms(200);
        for(int i=0;i<=4;i++)
        {
            CLRWDT();
            if(LoraConnect())break;
        }
        
        //3 - Wait x seconds
//        for(int i=0;i<=120;i++)
        
        EUART_GPS();
        __delay_ms(200);
         GpsOff();
        for(int i=0;i<=1;i++)
        {
            CLRWDT();
            __delay_ms(1000);
        }
//        
//        //5 -Go to sleep and wait new motion
        SleepNow();
    } 
}

//----------GPS---LORA---METHODS------------
bool ReadGPS()
{  
    //clear buffer
    strcpy(gpsHex,"");
     
    ReadUartCmd(gps);
    
    token = strtok (gps," ,");//READ root code
    while (strcmp(token,"$GPGLL") != 0) //Read GPGLL line
    {
        ReadUartCmd(gps);
        token = strtok (gps," ,");
    }
    
    latitude = strtok (NULL,"."); //Latitude
    latitudeDec = strtok (NULL,","); //Latitude decimal .
    token = strtok (NULL,","); //READ 'N' or exit
    longitude = strtok (NULL,"."); //Longitude 
    longitudeDec = strtok (NULL,","); //Longitude decimal
    
    if(strcmp(token,"N") != 0){
        blinkGreen(1);
        blinkRed(1);
        return false;
    }
    
    //Convert to hex and concatenate
    sprintf(hex, "%X", atoi(latitude));
    strcat(gpsHex,NormalizeHex(hex)); 
    sprintf(hex, "%X", atoi(latitudeDec));
    strcat(gpsHex,NormalizeHex(hex)); 
    sprintf(hex, "%X", atoi(longitude));
    strcat(gpsHex,NormalizeHex(hex)); 
    sprintf(hex, "%X", atoi(longitudeDec));
    strcat(gpsHex,NormalizeHex(hex)); 
    SendUartCmd(gpsHex);
    blinkGreen(1);
    return true;
}

bool LoraConnect(){
    LORA_RESET_SetLow();
    __delay_ms(90);
    LORA_RESET_SetHigh();
    __delay_ms(90);
    ReadUartCmd(lora);
    
    SendUartCmd("sys reset\r\n"); 
    ReadUartCmd(lora);  //read serial number

    SendUartCmd("mac set pwridx 5\r\n"); //Set power max 5
    ReadUartCmd(lora);  //read ok
    
    SendUartCmd("mac join otaa\r\n");
    ReadUartCmd(lora);  //read ok
    ReadUartCmd(lora);  //read denied or accepted
   
    token = strtok(lora," \r\n");
    if(strcmp(token,"accepted") != 0){
        blinkRed(1);
        return false;
    } 
    
     //send GPS position
    if(strlen(gpsHex) == 0){
        SendUartCmd("mac tx uncnf 9 00\r\n");
     }
     else{
        strcpy(str,"mac tx uncnf 9 ");
        strcat(str, gpsHex);
        strcat(str, "\r\n");
        SendUartCmd(str);
     }
    
    ReadUartCmd(lora);  //read ok 
    ReadUartCmd(lora);   //read mac_tx_ok / mac_rx 1 0000 / mac_err
    
    token = strtok(lora," \r\n");
    if(strcmp(token,"mac_err") == 0){
        blinkRed(1);
        return false;
    }
    blinkGreen(3);
    return true;
}

void SleepNow()
{
    INTCONbits.TMR0IE = 0; // Timer off
    SWDTEN = 0;            // WatchDog Off
    SLEEP();               // Sleep mode                       //Put here lora in sleep mode
    NOP();
    INTCONbits.TMR0IE = 1;
    SWDTEN = 1; 
}