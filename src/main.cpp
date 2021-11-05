#include <Arduino.h>
// #include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;

const int transPin = 3;
// Generally, you should use "unsigned long" for variables that hold time

// const long interval = 125;	 // interval at which to blink (microseconds)
// unsigned long base_time = 0; // will store the start of the current cycles time

const byte STARTBYTE = 0x02;
const byte ENDBYTE = 0x03;

// message storage
const int buffer_size = 20;
char sample_buffer[buffer_size];
volatile int current_buffer_byte = 0;

// current working byte
volatile byte working_byte = 0b10000000;
volatile int currert_bit = 0;

// this is a flag for when the loop should print the buffer out
volatile bool ready_to_print = false;
bool has_recived_start = false;
bool has_recived_end = false;
volatile int pin_state = 0;

// working current byte

void setup()
{

	// TIMER 1 for interrupt frequency 4000 Hz:
	// TIMER 1 for interrupt frequency 8000 Hz:
	cli();		// stop interrupts
	TCCR1A = 0; // set entire TCCR1A register to 0
	TCCR1B = 0; // same for TCCR1B
	TCNT1 = 0;	// initialize counter value to 0
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

	pinMode(transPin, OUTPUT);
	pinMode(recPin, INPUT);
	Serial.begin(115200);
}

ISR(TIMER1_COMPA_vect)
{
	//interrupt commands for TIMER 1 here

	if (currert_bit == 8)
	{
		Serial.print((char)working_byte);
		// check if we have a start or an end bit.
		if (working_byte == STARTBYTE)
		{
			has_recived_start = true;
		}
		if (working_byte == ENDBYTE)
		{
			has_recived_end = true;
		}

		if (has_recived_start)
		{
			if (has_recived_end || current_buffer_byte == buffer_size)
			{
				// transmit the buffer and reset it
				Serial.println(sample_buffer);
				current_buffer_byte = 0;
				for (size_t i = 0; i < buffer_size; i++)
				{
					sample_buffer[current_buffer_byte] = 0;
				}
			}
			else
			{

				// if current buffer is not full
				sample_buffer[current_buffer_byte] = (char)working_byte;

				current_buffer_byte++;

				// reset the ccurrent byte
				currert_bit = 0;
				working_byte = 0;
			}
		}
	}

	pin_state = digitalRead(recPin);
	bitWrite(working_byte, currert_bit, pin_state);
	currert_bit++;
}

void loop()
{
	// your program here...
}