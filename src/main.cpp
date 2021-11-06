#include <Arduino.h>
// #include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;
const int transPin = 3;

// message storage
const int buffer_size = 7;
byte buffer = 0;
volatile int current_buffer_bit = 0;

// this is a flag for when the loop should print the buffer out
volatile bool ready_to_print = false;
volatile bool has_recived_start = false;
volatile bool has_recived_end = false;

volatile bool has_sampled_3times = false;

//pin states
volatile int pin_in[3] = {0, 0, 0};
volatile int current_pin_in = 0;

volatile bool pin_last_state = 1;
volatile bool pin_current_state = 1;

// working current byte

void setup()
{

	// TIMER 1 for interrupt frequency 2000 Hz:
	cli();		// stop interrupts
	TCCR1A = 0; // set entire TCCR1A register to 0
	TCCR1B = 0; // same for TCCR1B
	TCNT1 = 0;	// initialize counter value to 0
	// set compare match register for 2000 Hz increments
	OCR1A = 7999; // = 16000000 / (1 * 2000) - 1 (must be <65536)
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

	current_pin_in = digitalRead(recPin);
	int sum = pin_in[0] + pin_in[1] + pin_in[2];
	if (sum > 1)
	{
		pin_current_state = false;
	}
	else
	{
		pin_current_state = true;
	}
	// pin_current_state = digitalRead(recPin);

	if (has_recived_start != true)
	{
		// look for a change from high to low
		// pin_current_state = digitalRead(recPin);

		if (pin_last_state && !pin_current_state)
		{
			has_recived_start = true;
			pin_last_state = pin_current_state;
			Serial.println("Found Start Bit");
		}
	}
	else
	{
		if (has_recived_end && pin_current_state)
		{
			//
			has_recived_start = true;
			has_recived_end = false;
			Serial.println("Cont Start ...");
			return;
		}

		if (!has_recived_end)
		{

			if (current_buffer_bit == 15)
			{
				bitWrite(buffer, current_buffer_bit, pin_current_state);
				current_buffer_bit++;
				Serial.println(buffer, HEX);
				Serial.println(buffer, BIN);

				buffer = 0;
				current_buffer_bit = 0;
				// has_recived_start = false;
				has_recived_end = true;
			}
			else
			{
				bitWrite(buffer, current_buffer_bit, pin_current_state);
				current_buffer_bit++;
			}
		}
	}
}

void loop()
{
	// your program here...
}