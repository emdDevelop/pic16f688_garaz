void eusart_init(void){
    
 TXSTAbits.TXEN=1;  //Transmit Enable bit 1= Transmit enabled  
 TXSTAbits.BRGH=1;  //High Baud Rate Select bit 1= High speed  
 BAUDCTLbits.BRG16=0;  //16-bit Baud Rate Generator bit 0= 8-bit baud rate generator is used
 TXSTAbits.TX9=0;
 TXSTAbits.SYNC=0;  //USART Mode Select bit 0 = Asynchronous mode 
 RCSTAbits.CREN=1;  //Continuous Receive Enable bit 1= Enables receiver
 
 //Configure I/O pins
TRISCbits.TRISC4=1;   //port C4 for eusart i/o TX input
TRISCbits.TRISC5=1;   //port C5 for eusart i/o RX input automatically configured

SPBRG=25;  //Writing SPBRG Register for 9600 baud rate for FOSC=4MHz
 
PIE1bits.RCIE=1; //Enable usart receive interrupt
PIE1bits.TXIE=0; //disable uart transmitt
RCSTAbits.SPEN=1;  //Serial Port Enable bit 1= Serial port enabled (configures RX/DT and TX/CK pins as serial port pins)
}

void eusartWriteChar(const char c){
      
    while(!TXSTAbits.TRMT)//wait until transmit shift register is empty
         ;
    TXREG=c; //write character to TXREG and start transmission
}

void eusartWriteText(const char *text){
    
    while(*text){
     eusartWriteChar(*text);
     text++;
    }
       
}


void eusart_check_tr(void){
    
       
}

char eusart_read_char(void){
    while(!RCIF)
        ;
     return RCREG;
    
}

void eusart_read_text(void){
    
       
}

void eusart_check_rx(void){
    
       
}
  