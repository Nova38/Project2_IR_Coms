#include <Arduino.h>
// #include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;

const int transPin = 3;
// Generally, you should use "unsigned long" for variables that hold time

// const long interval = 125;	 // interval at which to blink (microseconds)
// unsigned long base_time = 0; // will store the start of the current cycles time

// unsigned long last_input_time = 0;
// IR_Reciver *reciver;
// IR_Trasmitter *transmitter;

// const int size_sample_buffer = 20;
// const int syncSize = 4;

// volatile double sample_buffer[size_sample_buffer];
// volatile int sample_value_buffer[size_sample_buffer];

// volatile int current_sample = 0; // variable to be updated by the interrupt
// volatile bool buffer_Full = false;

#define BASECLOCK 16000000
int compareMatchReg;
volatile int interrupts;
volatile int pin_state = 0;
void setup()
{
	pinMode(recPin,INPUT);
	pinMode(transPin,OUTPUT);
	
	Serial.begin(115200);
	// initialize timer1 
	noInterrupts();           // disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;

	// Set compareMatchReg to the correct value for our interrupt interval
	// compareMatchReg = [16, 000, 000Hz / (prescaler * desired interrupt frequency)] - 1
	
	/* E.g. 1Hz with 1024 Pre-Scaler:
z		compareMatchReg = [16, 000, 000 / (1024 * 1)] - 1 = 15624
	
	    As this is > 256 Timer 1 Must be used for this..
	*/
	compareMatchReg = 15624;   // preload timer from calc above
	TCNT1 = compareMatchReg;   // preload timer
	

	/*
	Prescaler:
		(timer speed(Hz)) = (Arduino clock speed(16MHz)) / prescaler
			So 1Hz = 16000000 / 1 --> Prescaler: 1
			Prescaler can equal the below values and needs the relevant bits setting
			1    [CS10]
			8    [CS11]
			64   [CS11 + CS10]
			256  [CS12]
			1024 [CS12 + CS10]
	*/
	TCCR1B |= (1 << CS11);    // 8 prescaler 

	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

	
	interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
	TCNT1 = compareMatchReg;   // preload timer
	interrupts++;
	// pin_state = digitalRead(recPin);
	pin_state = !pin_state;
	digitalWrite(transPin,pin_state);
	// Serial.print("Total Ticks:");
	// Serial.print(interrupts);
	// Serial.print("\tPin State:");
	// Serial.println(pin_state);

}

void loop()
{
	// your program here...
}