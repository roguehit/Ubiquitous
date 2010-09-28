#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000


int main ()
{
DDRC=0xff;

while(1){
PORTB=0xff;
PORTC=0xff;
PORTD=0xff;
_delay_ms(1000);


PORTB=0x00;
PORTC=0x00;
PORTD=0x00;

_delay_ms(1000);

}
}

