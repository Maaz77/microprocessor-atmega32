/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
� Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2019/07/03
Author  : 
Company : 
Comments: 


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 1.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32.h>

// Alphanumeric LCD functions
#include <alcd.h>
#include <stdio.h>
#include <delay.h>

// Declare your global variables here
char key1 = 0;
char key2 = 0;
int  digit = 0;
int j =0;

#define keypin PINA
char read_key = '\0', read_val =  '\0';
char toggle = 0;


#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)
#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)

// USART Receiver buffer
#define RX_BUFFER_SIZE 8
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index=0,rx_rd_index=0;
#else
unsigned int rx_wr_index=0,rx_rd_index=0;
#endif

#if RX_BUFFER_SIZE < 256
unsigned char rx_counter=0;
#else
unsigned int rx_counter=0;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
char * str = "";
status=UCSRA;
data=UDR;
/*if(data ==  'a'  || data == 'b'){
    lcd_gotoxy(0,1);
    puts("warning"); 
    delay_ms(500);
}
*/
lcd_gotoxy(0,0);
if(data == 'a'){
    //lcd_gotoxy(0,3); 
   // lcd_clear();
    //lcd_puts("XC");
    putchar(0x03);
    delay_ms(100);
    putchar('a');
    delay_ms(100);
}
else if(data == 'b'){
    //lcd_gotoxy(0,3); 
    //lcd_clear();
   // lcd_puts("AC");
    putchar(0x03);
    delay_ms(100);
    putchar('b');
    delay_ms(100);
}
else{
    //lcd_putchar(data);
    //lcd_gotoxy(0,0);
    sprintf(str,"temp:%d",data);
    lcd_puts(str);
}

/*if (data != 'a' && data !='x'){
lcd_putchar(data);
j = j +1;
}
else {
}
if(j==3){
 lcd_clear();
 j=0;
} 
*/
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index++]=data;
#if RX_BUFFER_SIZE == 256
   // special case for receiver buffer size=256
   if (++rx_counter == 0) rx_buffer_overflow=1;
#else
   if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
      rx_buffer_overflow=1;
      }
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter==0);
data=rx_buffer[rx_rd_index++];
#if RX_BUFFER_SIZE != 256
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#endif
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE 8
char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE <= 256
unsigned char tx_wr_index=0,tx_rd_index=0;
#else
unsigned int tx_wr_index=0,tx_rd_index=0;
#endif

#if TX_BUFFER_SIZE < 256
unsigned char tx_counter=0;
#else
unsigned int tx_counter=0;
#endif

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{
if (tx_counter)
   {
   --tx_counter;
   UDR=tx_buffer[tx_rd_index++];
#if TX_BUFFER_SIZE != 256
   if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter == TX_BUFFER_SIZE);
#asm("cli")
if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer[tx_wr_index++]=c;
#if TX_BUFFER_SIZE != 256
   if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
#endif
   ++tx_counter;
   }
else
   UDR=c;
#asm("sei")
}
#pragma used-
#endif


/*void send(char c){
    while(!UCSRA.5);
    putchar(c);
}
*/


// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
// Place your code here
   char * str =  "";
   int num ;
    /*if (key2 >20){
        key2 = 20;
    } 
    */
 
   // sprintf(str,"%d ",key2);   
    //lcd_clear();
    //lcd_gotoxy(0,2);
    num = (key1 * 10 )  + key2;
    if (num > 20) num = 20;
    sprintf(str,"%d",num);
    lcd_puts(str);  
    putchar(0x02);
    delay_ms(50);
    putchar(num);
    delay_ms(50);
    //key2 = key2/10;
    //send(key2%10);        
    key1 = 0;
    key2 = 0; 
    
}


// External Interrupt 1 service routine
interrupt [EXT_INT1] void ext_int1_isr(void)
{
// Place your code here
read_val = 0x0f & keypin;
   
   switch (read_val){ 
   
        case 0x00:
            read_key = '0';
            break;
        case 0x01:
            read_key = '1';
            break;
        case 0x02:
            read_key = '2';
            break;
       case 0x03:
            read_key = '3';
            break;    
       case 0x04:
            read_key = '4';
            break; 
       case 0x05:
            read_key = '5';
            break;
       case 0x06:
            read_key = '6';
            break;
       case 0x07:
            read_key = '7';
            break;         
       case 0x08:
            read_key = '8';
            break;
       case 0x09:
            read_key = '9';
            break;
       case 0x0a:
            read_key = 'A';
            break;
       case 0x0b:
            read_key = 'B';
            break;
       case 0x0c:
            read_key = 'C';
            break;
       case 0x0d:
            read_key = 'D';
            break;
       case 0x0e:
            read_key = 'E';
            break;
       case 0x0f:
            read_key = 'F';
            break;
       }  
       
       

       
       if(toggle < 1){
       
       toggle++;
            if(digit == 0){
                lcd_gotoxy(0,2);
                lcd_putchar(read_key);
                key1 = read_key - '0';
                digit = 1; 
            }
            else {
                
                lcd_gotoxy(1,2);
                lcd_putchar(read_key); 
                //lcd_gotoxy(0,1); 
                key2 = read_key -'0';
                digit  = 0;
            } 
       }
       else{
        toggle = 0;
       }

}

/*int get_key(void){              //For reading from keypad
    DDRA=0b00000000;                //Portb as input
    PORTA=0b00001111;               //Pull up 4 pins
     
    DDRA=0b00010000; 
    if(PINA.0==0)return 1;
    if(PINA.1==0)return 4;
    if(PINA.2==0)return 7;
    if(PINA.3==0)return 10;
     
    DDRA=0b00100000;
    if(PINA.0==0)return 2;
    if(PINA.1==0)return 5;
    if(PINA.2==0)return 8;
    if(PINA.3==0)return 0;
     
    DDRA=0b01000000;
    if(PINA.0==0)return 3;
    if(PINA.1==0)return 6;
    if(PINA.2==0)return 9;
    if(PINA.3==0)return 11;
     
    return 255;
}

char receive(){
    char temp;
    while (UCSRA.7!=0);
    temp = getchar();
    return temp;  
}
*/

/*unsigned char receive(void)
{
UCSRA=0x00;
while(!UCSRA.7);
return UDR;
}

*/
void main(void)
{
// Declare your local variables here
int first ;
int  second;
int third;
int forth; 
char * str = "";
char myrecieve ;
int number;
int  i =0;

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=Out 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (1<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=0 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Any change
// INT1: Off
// INT2: Off
GICR|=(1<<INT1) | (1<<INT0) | (0<<INT2);
MCUCR=(0<<ISC11) | (1<<ISC10) | (0<<ISC01) | (1<<ISC00);
MCUCSR=(0<<ISC2);
GIFR=(1<<INTF1) | (1<<INTF0) | (0<<INTF2);

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Sync. Master UCPOL=0
// USART Baud Rate: 9600
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
UCSRB=(1<<RXCIE) | (1<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (1<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x00;
UBRRL=0x33;

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
SFIOR=(0<<ACME);

// ADC initialization
// ADC disabled
ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 20

DDRA = 0x00;
PORTA = 0xff;

lcd_init(20);

// Global enable interrupts
#asm("sei")

while (1)
      {
      // Place your code here 
      
      /*key=get_key(); 
      delay_ms(500);
      
      if(key<255){  
          key2 =  key;
      }
      */
     // lcd_putchar('1');
     // putchar('j');
      //delay_ms(100);
      //lcd_putchar('2');
      
     // myrecieve = getchar();
      //lcd_putchar('3');
      //lcd_putchar(myrecieve);
      
      //myrecieve = getchar();
      //lcd_putchar(myrecieve);
      
     // myrecieve = getchar();
     // lcd_putchar(myrecieve);
      
     // myrecieve = getchar();
     // lcd_putchar(myrecieve);
      
     // myrecieve = getchar();
     // lcd_putchar(myrecieve);
      
      
      /*lcd_gotoxy(0,0);
      if (i == 0){
      putchar (0x02);
      lcd_clear();
      lcd_puts("send");
      i = i+1;
      }
      */    
      
     // putchar(0x02);
      //putchar('0' + 200); 
      
      
 
      
      
      
      
     
      
      
     // myrecieve =  recieve();
      //lcd_gotoxy(0,1);
     // lcd_putchar(myrecieve); 
      
     // delay_ms(1000);
     // lcd_clear(); 
      
     
      
      
      
      //lcd_puts("temp:");
      //first = recieve();
     // delay_ms(100);
      //lcd_putchar(recieve);
      
     // second = getchar();
     // delay_ms(100);
      //lcd_putchar(recieve);
      
     // third = getchar();
      //delay_ms(100);
     // lcd_putchar(recieve); 
      
     // forth = getchar();
     // delay_ms(100);
      //lcd_putchar(recieve);
     // number = (first *1000) + (second * 100) + (third * 10) + forth;
     // sprintf(str,"temp:%d",number);  
     // lcd_puts(str);
      
     /* recieve = getchar();
      if (recieve == 'x'){
        lcd_clear();
        lcd_puts("warning");
        putchar(0x03);
        putchar('x');
      } 
      */
      
      
      
      
      
      

      }
}
