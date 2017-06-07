#include <stdio.h>
#include <string.h>

#define RS PORTAbits.RA0
#define EN PORTAbits.RA5
#define D4 PORTCbits.RC0
#define D5 PORTCbits.RC1
#define D6 PORTCbits.RC2
#define D7 PORTCbits.RC3

#define _XTAL_FREQ 4000000 //4Mhz

#include "pic.h"
#include "lcd.h"
#include "comm.h"


unsigned char state;
unsigned char flag;

unsigned char response_rcvd;
unsigned char responseID, response = 0;

char received_data[16], ip_address[16],len[4];
unsigned short i, tmp, DataReady;
unsigned char id;

const short OK=1;
const short ERROR = 2;
const short NO_CHANGE = 3;
const short FAIL = 4;
const short READY = 5;
const short WIFI_GOT_IP=6;
const short CONNECT=7;
const short SEND_OK=8;
const short SEND_SYMBOL=9;


char Get_Response() {
  if (response_rcvd) {
    response_rcvd = 0;
    return responseID;
  }
  else
    return 0;
}

short int button_debounce(){
    if(PORTAbits.RA4==0) //if the switch is pressed
        __delay_ms(100); //switch debounce
    if(PORTAbits.RA4==0) //if the switch is still pressed
        return 0;
    else return 1;   
}

void wifi_message_http(const char* text){
    int size;
    size=strlen(text);
    char sizeOfText[5];
    sprintf(sizeOfText,"%d",size);
   __delay_ms(1000);  
   eusartWriteText("AT+CIPSEND=");
   eusartWriteChar(id);
    eusartWriteText(",");
     eusartWriteText(sizeOfText);
   eusartWriteText("\r\n");
   while(Get_Response() != SEND_SYMBOL);
    eusartWriteText(text);
    while(Get_Response() != OK);
    eusartWriteText("AT+CIPCLOSE="); 
    eusartWriteChar(id);
    eusartWriteText("\r\n");
    __delay_ms(1000);
}

void wifi_message(const char* text){
    int size;
    size=strlen(text);
    char sizeOfText[5];
    sprintf(sizeOfText,"%d",size);
   __delay_ms(1000);
   eusartWriteText("AT+CIPSEND=");
   eusartWriteChar(id);
   eusartWriteText(",");
   eusartWriteText(sizeOfText);
   eusartWriteText("\r\n");
   while(Get_Response() != SEND_SYMBOL);
    eusartWriteText(text);
}
      
 void config(void){
     int count=0;
     //RESET ESP
     Lcd_Clear();
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("Reseting..");
     do {
     eusartWriteText("AT+RST\r\n");
        } while(Get_Response() != READY);
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String("ready");
      __delay_ms(2000);
    //SET MODE 1
     Lcd_Clear();
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("Set Mode 1..");
     do {
     eusartWriteText("AT+CWMODE=1\r\n");
        } while(Get_Response() != OK);
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String("OK SET");
      __delay_ms(2000);
    //QUIT WIFI  
     Lcd_Clear();
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("Wifi Disconnect");
     do {
     eusartWriteText("AT+CWQAP\r\n");//agia
        } while(Get_Response() != OK);
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String("OK");
     __delay_ms(2000);  
    //WIFI CONNECT
     Lcd_Clear();
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("Wifi connect..");
     do {
     //eusartWriteText("AT+CWJAP=\"COSMOTE-46A180\",\"PU5QxpKr5KxjVaCD\"\r\n");//grafeio
     eusartWriteText("AT+CWJAP=\"OTE31A900\",\"vYdAtjCYKyVteFys\"\r\n");//anna
     //eusartWriteText("AT+CWJAP=\"COSMOTE-5088E4\",\"uSxbHskcedkfZpgT\"\r\n");//agia
     //eusartWriteText("AT+CWJAP=\"Ikteo_foivos\",\"ikteo22000\"\r\n");//nikos
     count++;
     if(count>900)
     {
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("Wifi error     ");
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String("Please reset ");
     }
        } while(Get_Response() != WIFI_GOT_IP);
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String("WIFI GOT IP");
     __delay_ms(3000);
      //SET CIPMUX=1
     Lcd_Clear();
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("Set CIPMUX=1..");
     do {
     eusartWriteText("AT+CIPMUX=1\r\n");
        } while(Get_Response() != OK);
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String("OK SET");
      __delay_ms(2000);
      //SET CIPSERVER
     Lcd_Clear();
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("Set CIPSERVER");
     do {
     eusartWriteText("AT+CIPSERVER=1,80\r\n");
        } while(Get_Response() != OK);
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String("OK SET");
      __delay_ms(2000);
     //GET IP
     Lcd_Clear();
     Lcd_Set_Cursor(1,1);
     Lcd_Write_String("GOT IP");
     do {
     eusartWriteText("AT+CIFSR\r\n");
        } while(Get_Response() != OK);
      __delay_ms(2000);      
     Lcd_Set_Cursor(2,1);
     Lcd_Write_String(ip_address);
     __delay_ms(2000); 
 }     

int main()
{
    unsigned int a;
   
    OSCCON=0b01100000 ; //set internal oscillator to 4Mhz
    CMCON0 = 0b00000111 ; // Disable comparators
    ANSEL = 0b00000000; //All I/O pins are configured as digital
    ADCON0bits.ADON=0;//disable ADC
    TRISA=0b00010000; //PORT RA2 and PORT RA1 GOES TO RELAYS configure as output PORT RA4 SWITCH SENSOR configure as INPUT
    PORTAbits.RA1=0; //set port A1 low
    PORTAbits.RA2=0; //set port A2 low
   // PORTAbits.RA4=0;
    TRISCbits.TRISC0 =0; //configure as output
    TRISCbits.TRISC1 =0; //configure as output
    TRISCbits.TRISC2 =0; //configure as output
    TRISCbits.TRISC3 =0; //configure as output
    //PORTAbits.RA1=0;
    
    INTCONbits.PEIE=1;// enable periphereal interrupts
    INTCONbits.GIE=1; //enable general interrupts
    INTCONbits.RAIE=1; //enable the interrupt for sensor switch on port RA4
    IOCAbits.IOCA4=1; //enable pin interrupt (when port change) on pin RA4
   
   
    Lcd_Init();
    eusart_init();
    __delay_ms(2000);
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Welcome");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("Garaz Control");
    __delay_ms(2000);
    
    config(); //SETUP ESP
    response=0;

    unsigned int counter=0;

  while(1)
  {

       if(button_debounce()==0 && counter!=5)
              {
              wifi_message("Door is closed");
              counter++;
              }

      if(DataReady)
     {
      DataReady = 0;
      Lcd_Clear();
      Lcd_Set_Cursor(1,1);
      Lcd_Write_String(received_data);
     }

      switch(received_data[0])
      {
          case 'G' :{
             // __delay_ms(1000);
              wifi_message_http("Door is closed");
              received_data[0]='\0';
              break;
          }
          
          case 'U' :{
              PORTAbits.RA2=1;
              PORTAbits.RA1=0;
              Lcd_Clear();
              Lcd_Set_Cursor(1,1);
              Lcd_Write_String("Door moving up");
              wifi_message("Door moving up");
              received_data[0]='\0';
              counter=0;
              break;
          }   
          
          case 'D' :{
              PORTAbits.RA1=1;
              PORTAbits.RA2=0;
              Lcd_Clear();
              Lcd_Set_Cursor(1,1);
              Lcd_Write_String("Door moving down");
              wifi_message("Door moving down");
              received_data[0]='\0';
              counter=0;
              break;
          }
          
           case 'T' :{
              PORTAbits.RA1=0;
              PORTAbits.RA2=0;
              Lcd_Clear();
              Lcd_Set_Cursor(1,1);
              Lcd_Write_String("Door STOP moving");
              wifi_message("Door STOP moving");
              received_data[0]='\0';
              counter=0;
              break;
          }

           case 'S' :{
              if(button_debounce()==0)
              {
              wifi_message("Door is closed");
              received_data[0]='\0';
              break;
              }else
              {
              wifi_message("Door is open");
              received_data[0]='\0';
              break;
              }

          }
      }      
  }
  return 0;
}

void interrupt ISR(void)
{
    
    if(INTCONbits.RAIF)
    {
        if(button_debounce()==0)
        {
        PORTAbits.RA2=0; //relay off  
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Garaz sensor on");
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String("Door is closed");
        flag=0;
        }else
        {
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Garaz sensor off");
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String("Door is open");
        flag=1;
        }     
        INTCONbits.RAIF=0;  // reset receive interrupt flag
    }
    
    if (PIR1bits.RCIF)  // check if receive interrupt has fired
    {
        tmp = RCREG;      // read received character to buffer

        switch(state)
        {
         case 0: {
                response = 0;                   // Clear response
                if (tmp == 'O')
                  state = 1;
                if (tmp == 'E')
                  state = 10;
                if (tmp == 'n')
                  state = 20;
                if (tmp == 'F')
                  state = 30;
                if (tmp == 'S')
                  state = 40;
                if (tmp == 'r')
                  state = 50;
                if (tmp == 'W')
                  state = 60;
                if (tmp == '+')
                  state = 70;
                if (tmp == '>')
                  state = 80;
                break;
                 }   
                case 1: {
                if (tmp == 'K'){
                  response = OK;                // OK
                  state = 2;
                }
                else
                  state = 0;
                break;
                }

      case 10: {
                if (tmp == 'R')
                  state = 11;
                else
                  state = 0;
                break;
                 }

      case 11: {
                if (tmp == 'R')
                  state = 12;
                else
                  state = 0;
                break;
                 }

      case 12: {
                if (tmp == 'O')
                  state = 13;
                else
                  state = 0;
                break;
                 }

      case 13: {
                if (tmp == 'R') {
                  response = ERROR;             // ERROR
                  state = 2;
                }
                else
                  state = 0;
                break;
                 }

      case 20: {
                if (tmp == 'o')
                  state = 21;
                else
                  state = 0;
                break;
      }

      case 21: {
                if (tmp == ' ')
                  state = 22;
                else
                  state = 0;
                break;
      }

      case 22: {
                if (tmp == 'c')
                  state = 23;
                else
                  state = 0;
                break;
      }

      case 23: {
                if (tmp == 'h')
                  state = 24;
                else
                  state = 0;
                break;
      }

      case 24: {
                if (tmp == 'a')
                  state = 25;
                else
                  state = 0;
                break;
                }

      case 25: {
                if (tmp == 'n')
                  state = 26;
                else
                  state = 0;
                break;
                 }

      case 26: {
                if (tmp == 'g')
                  state = 27;
                else
                  state = 0;
                break;
                 }

      case 27: {
                if (tmp == 'e'){
                  response = NO_CHANGE;         // NO CHANGE
                  state = 2;
                }
                else
                  state = 0;
                break;
                }

      case 30: {
                if (tmp == 'A')
                  state = 31;
                else
                  state = 0;
                break;
                }

      case 31: {
                if (tmp == 'I')
                  state = 32;
                else
                  state = 0;
                break;
                }

      case 32: {
                if (tmp == 'L') {
                  response = FAIL;              // FAIL
                  state = 2;
                }
                else
                  state = 0;
                break;
                }

      case 40: {
                if (tmp == 'T')
                  state = 41;
                else
                  state = 0;
                break;
                }

      case 41: {
                if (tmp == 'A')
                  state = 42;
                else
                  state = 0;
                break;
               }

      case 42: {
                if (tmp == 'I')
                  state = 43;
                else
                  state = 0;
                break;
               }

      case 43: {
                if (tmp == 'P')
                  state = 44;
                else
                  state = 0;
                break;
                }

      case 44: {
                if (tmp == ',')
                  state = 45;
                else
                  state = 0;
                break;
                 }

      case 45: {
                if (tmp == '\"')
                  state = 46;
                else
                  state = 0;
                break;
                 }

      case 46: {
                if (tmp == '\"'){
                  state = 3;
                  response = OK;
                }
                else {
                  ip_address[i] = tmp;              // get IP address
                  i++;
                }
                break;
                 }

      case 50: {
                if (tmp == 'e')
                  state = 51;
                else
                  state = 0;
                break;
                 }
     
      case 51: {
                if (tmp == 'a')
                  state = 52;
                else
                  state = 0;
                break;
                 }

      case 52: {
                if (tmp == 'd')
                  state = 53;
                else
                  state = 0;
                break;
                }

      case 53: {
                if (tmp == 'y') {
                  response = READY;              // READY
                  state = 2;
                }
                else
                  state = 0;
                break;
                }
      
      case 60: {
                if (tmp == 'I')
                  state = 61;
                else
                  state = 0;
                break;
                }
      
      case 61: {
                if (tmp == 'F')
                  state = 62;
                else
                  state = 0;
                break;
                }
      
      case 62: {
                if (tmp == 'I')
                  state = 63;
                else
                  state = 0;
                break;
                }
      
      case 63: {
                if (tmp == ' ')
                  state = 64;
                else
                  state = 0;
                break;
                }
      
      case 64: {
                if (tmp == 'G')
                  state = 65;
                else
                  state = 0;
                break;
                }
      
      case 65: {
                if (tmp == 'O')
                  state = 66;
                else
                  state = 0;
                break;
                }
      
      case 66: {
                if (tmp == 'T')
                  state = 67;
                else
                  state = 0;
                break;
                }
      
      case 67: {
                if (tmp == ' ')
                  state = 68;
                else
                  state = 0;
                break;
                }
      
      case 68: {
                if (tmp == 'I')
                  state = 69;
                else
                  state = 0;
                break;
                }
      
      case 69: {
                if (tmp == 'P') {
                  response = WIFI_GOT_IP;      // WIFI GOT IP
                  state = 2;
                }
                else
                  state = 0;
                break;
                }
      
       case 70: {
                if (tmp == 'I')
                  state = 71;
                else
                  state = 0;
                break;
                }
       
       case 71: {
                if (tmp == 'P')
                  state = 72;
                else
                  state = 0;
                break;
                }
       
       case 72: {
                if (tmp == 'D')
                  state = 73;
                else
                  state = 0;
                break;
                }
       
       case 73: {
                if (tmp == ',')
                  state = 74;
                else
                  state = 0;
                break;
                }
       
       case 74: {
                  id=tmp;
                  state = 75;
                break;
                }
       
       case 75: {
                if (tmp == ',')
                {
                   state = 76;
                   i=0;
                }
                else
                  state = 0;
                break;
                }
       
       case 76: {
                if (tmp != ':')
                {
                    len[i]=tmp;
                    i++;
                    len[i]='\0';
                }
                else
                  state = 100;
                  i=0;

                break;
                } 
       
       case 80: {
                  response=SEND_SYMBOL;
                  response_rcvd = 1;
                  responseID = response;
                  state = 0;
                break;
              }
     
      case 100 : {        
                    if((tmp == '\r' && i!=0) )
                  {
                    received_data[i] = '\0';
                    DataReady = 1;
                    state = 0;
                    i=0;
                  }
                  else
                  {
                      if((tmp=='\n'&& i==0) || (tmp=='\r' && i==0))
                      {
                         DataReady=0;
                         state=0;
                      }else if(tmp!='\n'|| tmp!='\r')
                      {
                         received_data[i] = tmp;           // get data
                         i++;
                        if (i == 16) 
                        {
                          received_data[i-1] ='\0';
                          // i = 16;
                          //DataReady = 1;
                          i=0;
                          state = 0;
                        }     
                      }                  
                  }
                  break;
                 }
      case 2: {
                if (tmp == 13)
                  state = 3;
                else
                  state = 0;
                break;
              }

      case 3: {
                if (tmp == 10){
                  response_rcvd = 1;
                  responseID = response;
                }
                state = 0;
                break;
              }

      default: {
                state = 0;
                break;
               }     
        }
        PIR1bits.RCIF = 0;      // reset receive interrupt flag
    }
}