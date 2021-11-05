#include <Arduino.h>
// #include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;

const int transPin = 3;
// Generally, you should use "unsigned long" for variables that hold time

// const long interval = 125;	 // interval at which to blink (microseconds)
// unsigned long base_time = 0; // will store the start of the current cycles time

const byte STARTBYTE =0x02;
const byte ENDBYTE =0x03;


const int buffer_size = 20;
// const int syncSize = 4;

volatile byte sample_buffer[buffer_size];

// volatile int current_sample = 0; // variable to be updated by the interrupt
// volatile bool buffer_Full = false;

volatile int pin_state = 0;
volatile int arry_count;



void setup()
{

		// TIMER 1 for interrupt frequency 4000 Hz:
	// TIMER 1 for interrupt frequency 8000 Hz:
	cli(); // stop interrupts
	TCCR1A = 0; // set entire TCCR1A register to 0
	TCCR1B = 0; // same for TCCR1B
	TCNT1  = 0; // initialize counter value to 0
	// set compare match register for 8000 Hz increments
	OCR1A = 1999; // = 16000000 / (1 * 8000) - 1 (must be <65536)
	OCR1A = 3999; // = 16000000 / (1 * 4000) - 1 (must be <65536)

	// turn on CTC mode
	TCCR1B |= (1 << WGM12);
	// Set CS12, CS11 and CS10 bits for 1 prescaler
	TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	sei(); // allow interrupts

	pinMode(transPin,OUTPUT);
	pinMode(recPin, INPUT);	
	Serial.begin(115200);

}



ISR(TIMER1_COMPA_vect){
   //interrupt commands for TIMER 1 here
	pin_state = digitalRead(recPin);
	//  digitalRead(recPin);
	// pin_state = !pin_state;
	// digitalWrite(transPin,pin_state);

}



void loop()
{
	// your program here...
}