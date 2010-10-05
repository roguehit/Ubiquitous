#include <avr/io.h>
#include <util/delay.h>

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <stdio.h>

#define F_CPU 1000000	// 1 MHz oscillator.
#define BaudRate 9600
#define MYUBRR (F_CPU / 16 / BaudRate ) - 1 


unsigned char serialCheckRxComplete(void)
{
	return( UCSR0A & _BV(RXC0)) ;		// nonzero if serial data is available to read.
}

unsigned char serialCheckTxReady(void)
{
	return( UCSR0A & _BV(UDRE0) ) ;		// nonzero if transmit register is ready to receive new data.
}

unsigned char serialRead(void)
{
	while (serialCheckRxComplete() == 0)		// While data is NOT available to read
	{;;} 
	return UDR0;
}

void serialWrite(unsigned char DataOut)
{
	while (serialCheckTxReady() == 0)		// while NOT ready to transmit 
	{;;} 
	UDR0 = DataOut;
}



void establishContact() {
	while (serialCheckRxComplete() == 0) { 
		serialWrite('U');
		PORTC=0xff;
		_delay_ms(10000);
		PORTC=0x00;
		_delay_ms(1000);
		}
}

int main ()
{
unsigned char a,b,c,temp;
_delay_ms(1000);

_delay_ms(1000);

DDRC=0xff;
#if 1
	DDRD = _BV(1);
	DDRB = _BV(0) | _BV(1) | _BV(3) | _BV(5);
	 
	//Serial Initialization
	
 	/*Set baud rate */ 
	UBRR0H = (unsigned char)(MYUBRR>>8); 
	UBRR0L = (unsigned char) MYUBRR; 
	/* Enable receiver and transmitter   */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0); 
	/* Frame format: 8data, No parity, 1stop bit */ 
	UCSR0C = (3<<UCSZ00);  
//_delay_ms(2000);
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR);
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

establishContact(); 

//serialWrite(0x45); // Clears the screen

#if 1
serialWrite(0x42);
serialWrite(0x00);
serialWrite(0x00);
#endif
_delay_ms(8000);   
 
//serialWrite(0x70); //Sets the Pen Size
//serialWrite(0x00); //Sets the Pen Contour
#if 0
while(1){
        
	
        ADCSRA |= _BV(ADEN);
        ADCSRA |= _BV(ADSC);
        while (bit_is_set(ADCSRA, ADSC));

	temp=ADCH;
	a=temp/100;
	temp%=100;
	b=temp/10;
	temp%=10;
	c=temp;

	serialWrite(0x73); 
	serialWrite(0x00); 
	serialWrite(0x00); 
	serialWrite(0x00); 
	serialWrite(0xff); 
	serialWrite(0xff); 
	
	serialWrite(a+48); 
	serialWrite(b+48); 
	serialWrite(c+48); 
	serialWrite(0x00); 
 	_delay_ms(5000);
	serialWrite(0x45);
        _delay_ms(180);
	
}
#endif
//for(i=0;i<5;i++)

/*This Changes the Screen to the set color*/
#if 1
serialWrite(0x42);
serialWrite(0x22);
serialWrite(0x22);
#endif

_delay_ms(8000);
/*Writes Rohit on the Screen*/
#if 0
serialWrite(0x73); 
serialWrite(0x01); 
serialWrite(0x01); 
serialWrite(0x02); 
serialWrite(0x44); 
serialWrite(0x44); 
serialWrite(0x52); 
serialWrite(0x4f); 
serialWrite(0x48); 
serialWrite(0x49); 
serialWrite(0x54); 
serialWrite(0x00); 
#endif

/*The following command draws a Circle*/
#if 0
serialWrite(0x43); 
serialWrite(0x20);
serialWrite(0x20);
serialWrite(0x10);
serialWrite(0x00);
serialWrite(0x1f);
#endif



/*This Powers down/up the Screen*/
#if 0
serialWrite(0x59);
serialWrite(0x01);
serialWrite(0x01);
#endif

/*This Powers down/up the entire OLED*/
#if 1
serialWrite(0x59);
serialWrite(0x03);
serialWrite(0x00);
#endif

#endif
#if 1


while(1){
PORTC=0xFF;
_delay_ms(1200);
PORTC=0x00;
_delay_ms(0);
}
#endif


}

