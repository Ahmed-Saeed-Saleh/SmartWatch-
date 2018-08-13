/*
 * DigitalWatch.c
 *
 *  Created on: Aug 13, 2018
 *  Author: Ahmed Saeed
 */

/* internal crystal 1000000 MHZ */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define seg1 0x01
#define seg2 0x02
#define seg3 0x04
#define seg4 0x08
#define seg5 0x10
#define seg6 0x20

unsigned short Hour=0, Minute=0 ,Second=0;

void TIMER1_COMPARE_MODE(void){
	/*
	 * IT WILL COUNT TO 0XFFFF (65535)
	 * T (AFTER PRESCALER) = 1.024 MS
	 * SO I NEED 1024 COUNTING TO GET 1 S
	 */
	TCNT1 = 0; 	//timer initial value
	OCR1A = 1024 ; // top value of compare mode
	TCCR1A = (1<<FOC1A) ; // NORMAL MODE
	TCCR1B=(1<<CS10)|(1<<CS12)|(1<<WGM12); // PRESCALER 1024
	TIMSK |=(1<<OCIE1A); // Compare A Match Interrupt Enable
}

void INT2_init(void){
	SREG  &=~(1<<7);  		 // Disable interrupts by clearing I-bit
	DDRB   &= (~(1<<PB2));   // Configure INT2/PB2 as input pin
	PORTB |=(1<<PB2); 		// Configure PB2 as internal Pull up resistor
	GICR |= (1<<INT2);		// Enable external interrupt pin INT2
	MCUCSR &=~(1<<ISC2);	// Trigger INT2 with the falling edge
	SREG  |=(1<<7);			 // Enable interrupts by setting I-bit
}
/* compare on match Interrupt for counting */
ISR(TIMER1_COMPA_vect){
	Second++;
	if(Second == 60){
	   Second = 0;
	   Minute++;
	}
	if(Minute == 60){
		Minute = 0;
		Hour++;
	}
	if(Hour == 24){
		Hour = 0;
	}
	SREG  |=(1<<7);
}

ISR(INT2_vect){
	Hour=0, Minute=0 ,Second=0;
}
void Display(void){
	PORTD &= 0xC0; PORTD |= seg6;
	PORTC = Second % 10;
	_delay_ms(5);

	PORTD &= 0xC0; PORTD |= seg5;
	PORTC = Second / 10;
	_delay_ms(5);
	////////////////////////////
	PORTD &= 0xC0; PORTD |= seg4;
	PORTC = Minute % 10;
	_delay_ms(5);

	PORTD &= 0xC0; PORTD |= seg3;
	PORTC = Minute / 10;
	_delay_ms(5);

	////////////////////////////
	PORTD &= 0xC0; PORTD |= seg2;
	PORTC = Hour % 10;
	_delay_ms(5);

	PORTD &= 0xC0; PORTD |= seg1;
	PORTC = Hour / 10;
	_delay_ms(5);
}

int main(void){
	INT2_init(); // interrupt 2 initalize
	TIMER1_COMPARE_MODE();  // Timer 1 initialize
	DDRC =(1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3);  // using portc as an output of 7seg
	PORTC &= 0xf0; // initial value 0
	DDRD = (1<<PD0)|(1<<PD1)|(1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5); // Portd as output to control sevenSegment
	PORTD = 0xff;	//portD intialized zer0
	while(1){
		Display();
	}
	return 0 ;
}
