#include <Arduino.h>
// #include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;
// const int transPin = 2;
// Generally, you should use "unsigned long" for variables that hold time

const long interval = 125;	 // interval at which to blink (microseconds)
unsigned long base_time = 0; // will store the start of the current cycles time

unsigned long last_input_time = 0;
// IR_Reciver *reciver;
// IR_Trasmitter *transmitter;

const int size_sample_buffer = 20;
const int syncSize = 4;

volatile double sample_buffer[size_sample_buffer];
volatile int sample_value_buffer[size_sample_buffer];

volatile int current_sample = 0; // variable to be updated by the interrupt
volatile bool buffer_Full = false;

// Interrupt service routine for interrupt 0
void increment()
{
	// unsigned long input_time = micros();; // current input time

	// for testing with button
	unsigned long input_time = micros();; // current input time
	if(base_time==0) {
		base_time =input_time;
	}
	


	//check to see if increment() was called in the last 250 milliseconds
	if (input_time - last_input_time > interval)
	{
		if (buffer_Full || current_sample == size_sample_buffer)
		{
			return; // dont add data till the main loop has a chance to use it
		}
		else
		{
			last_input_time = input_time;
			sample_buffer[current_sample] = floor((input_time - base_time) / (float)interval);

			current_sample++;
			if (current_sample == size_sample_buffer)
			{
				buffer_Full = true;
			}
		}
	}
}

void setup()
{
	//start serial connection
	// Serial.begin(115200);

	Serial.begin(9600);

	pinMode(recPin, INPUT_PULLUP);
	attachInterrupt(0, increment, CHANGE);

	for (int i = 0; i < size_sample_buffer; i++)
	{
		sample_buffer[i] = 0;
	}

	// reciver = new IR_Reciver(recPin);
	//configure pin 2 as an input and enable the internal pull-up resistor
	// pinMode(13, OUTPUT);
}

//variables to keep track of the timing of recent interrupts

void loop()
{
	// delay(300);							 //pretend to be doing something useful
	// Serial.println(current_sample, DEC); //print current_sample to serial monitor

	if (buffer_Full)
	{
		noInterrupts();
		Serial.print("\nBuffer Full;\n\tValues:");
		for (int i = 0; i < size_sample_buffer; i++)
		{
			Serial.print(sample_buffer[i]);
			Serial.print(" ");
		}

		buffer_Full = false;
		current_sample = 0;
		base_time = 0;
		interrupts();
	}
}
