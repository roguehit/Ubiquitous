#ifndef OLED_H_ASL
#define OLED_H_ASL

#include <avr/io.h>
#include <util/delay.h>

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

unsigned char serialCheckRxComplete(void)
{
  return( UCSR0A & _BV(RXC0)) ; // nonzero if serial data is available to read.
}

unsigned char serialCheckTxReady(void)
{
  return( UCSR0A & _BV(UDRE0) ) ; // nonzero if transmit register is ready to receive new data.
}

unsigned char serialRead(void)
{
  while (serialCheckRxComplete() == 0) // While data is NOT available to read
  {;;}
  return UDR0;
}

void serialWrite(unsigned char DataOut)
{
  while (serialCheckTxReady() == 0) // while NOT ready to transmit
  {;;}
  UDR0 = DataOut;
}

void establishContact() 
{
	DDRB  |= _BV(0);
	PORTB |= _BV(0);
		
	while (serialCheckRxComplete() == 0)
	{
		serialWrite('U');
	}
  	PORTB &= ~(0x01);
}

void GetColor(float red, float green, float blue, char *half1, char *half2)
{
	int redInt = (32 * red) - 1;
	if(redInt < 0) redInt = 0;
	int greenInt = (64 * green) - 1;
	if(greenInt < 0) greenInt = 0;
	int blueInt = (32 * blue) - 1;
	if(blueInt < 0) blueInt = 0;
	
	uint16_t combined = (redInt << 11) | (greenInt << 5) | blueInt;

	*half1 = combined >> 8;
	*half2 = combined;

/*
	int temp, a, b, c;

	temp = *half1;
    a=temp/100;
    temp%=100;
    b=temp/10;
    temp%=10;
    c=temp;

	_delay_ms(8000);

	serialWrite(0x45);

	_delay_ms(8000);	
  
    serialWrite(0x73);
    serialWrite(0x00);
    serialWrite(0x00);
    serialWrite(0x01);
    serialWrite(0xff);
    serialWrite(0xff);
    
    serialWrite(a+48);
    serialWrite(b+48);
    serialWrite(c+48);
    serialWrite(0x00);

	while(1)
	{
		_delay_ms(80000);
	} 
*/

}

// Syntax : cmd, size
void SetPenSize(int size)
{
	// send pen size command
	serialWrite(0x70);

	// draw as solid (fill)
	if(size > 0)
	{
		serialWrite(0x01);		
	}
	// draw as wire frame (outline)
	else
	{
		serialWrite(0x00);
	}

}

void SetBackgroundColor(float red, float green, float blue)
{
	serialWrite(0x42);

	char half1 = 0;
	char half2 = 0;

	GetColor(red, green, blue, &half1, &half2);
	
	serialWrite(half1);
	serialWrite(half2);

	_delay_ms(8000);
	
}

// Syntax : cmd, x1, y1, x2, y2, colour(msb:lsb)
void DrawRectangle(int x1, int y1, int x2, int y2, float red, float green, float blue)
{
	// send draw rectangle command
	serialWrite(0x72);

	if(x1 < 0) {x1 = 0;}
	if(x1 > 95) {x1 = 95;}
	if(y1 < 0) {y1 = 0;}
	if(y1 > 63) {y1 = 63;}

	if(x2 < 0) {x2 = 0;}
	if(x2 > 95) {x2 = 95;}
	if(y2 < 0) {y2 = 0;}
	if(y2 > 63) {y2 = 63;}

	// send rectangle coordinates
	serialWrite(x1);
	serialWrite(y1);
	serialWrite(x2);
	serialWrite(y2);

	// get color
	char half1 = 0;
	char half2 = 0;
	GetColor(red, green, blue, &half1, &half2);

	// send color (2 bytes)
	serialWrite(half1);
	serialWrite(half2);

}

// Syntax : cmd, x, y, rad, colour(msb:lsb)
void DrawCircle(unsigned char x, unsigned char y, unsigned char r,
				float red, float green, float blue)
{
	// send draw circle command
	serialWrite(0x43);

	// check bounds
	if(x < 0) {x = 0;}
	if(x > 95) {x = 95;}
	if(y < 0) {y = 0;}
	if(y > 63) {y = 63;}
	if(r < 0) {r = 0;}
	if(r > 63) {r = 63;}

	// send center of circle
	serialWrite(x);
	serialWrite(y);

	// send radius of circle
	serialWrite(r);

	// get color
	char half1 = 0;
	char half2 = 0;
	GetColor(red, green, blue, &half1, &half2);

	// send color (2 bytes)
	serialWrite(half1);
	serialWrite(half2);
}

// Syntax : cmd, x1, y1, x2, y2, x3, y3, colour(msb:lsb)
// x2 < x1, x3 > x2, y2 > y1, y3 > y1
void DrawTriangle(unsigned char x1, unsigned char y1, unsigned char x2,
				  unsigned char y2, unsigned char x3, unsigned char y3,
				   float r, float g, float b)
{
	//serialWrite(0x47);

}

// Syntax : cmd, x1, y1, x2, y2, colour(msb:lsb)
void DrawLine(unsigned char x1, unsigned char y1, unsigned char x2,
			  unsigned char y2, float r, float g, float b)
{
	//serialWrite(0x4C);
}

// Syntax : cmd, x, y, font, colour(msb:lsb), width, height, char1, .. , charN,
// terminator

void WriteStringUnformatted(unsigned char x, unsigned char y, unsigned char f,
				 float r, float g, float b, unsigned char w,
				 unsigned char h, char *s)
{
	// start place String of ascii
	serialWrite(0x53);
	
	// send x and y positions (in pixels)
	serialWrite(x);
	serialWrite(y);

	// send font type
	serialWrite(f);

	// get color
	char half1 = 0;
	char half2 = 0;
	GetColor(r, g, b, &half1, &half2);

	// send color (2 bytes)
	serialWrite(half1);
	serialWrite(half2);

	serialWrite(w);
	serialWrite(h);

	UDR0 = 0;

	while(*s != '\0')
	{
		serialWrite(*s);
		s++;
	}

	// terminate string
	serialWrite(0x00);

}

void displayImage()
{
	_delay_ms(10000);
	serialWrite(0x40);
	serialWrite(0x49);
	serialWrite(0x05);
	serialWrite(0x00);

	serialWrite(0x56);
	serialWrite(0x40);
	
	serialWrite(0x10);
	serialWrite(0x00);
	serialWrite(0x10);
	
	serialWrite(0x42);

	_delay_ms(10000);
}


void SetContrast(unsigned char c)
{
	serialWrite(0x59);
	serialWrite(0x02);

	if(c < 0) c = 0;
	if(c > 15) c = 15;

	serialWrite(c);
}

void ClearScreen()
{
	serialWrite(0x45);
	_delay_ms(1000);
}

void PowerOnOLED()
{
	serialWrite(0x59);
	serialWrite(0x03);
	serialWrite(0x01);
}


void PowerDownOLED()
{
	serialWrite(0x59);
	serialWrite(0x03);
	serialWrite(0x00);
}

void PowerOnScreen()
{
	serialWrite(0x59);
	serialWrite(0x01);
	serialWrite(0x01);
}

void PowerDownScreen()
{
	serialWrite(0x59);
	serialWrite(0x01);
	serialWrite(0x00);
}

#endif
