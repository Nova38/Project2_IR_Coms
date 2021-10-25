#include <Arduino.h>
#include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;
const int transPin = 2;
// Generally, you should use "unsigned long" for variables that hold time


IR_Reciver *reciver;
IR_Trasmitter *transmitter;

void setup()
{
	//start serial connection
	Serial.begin(115200);

	reciver = new IR_Reciver(recPin);
	//configure pin 2 as an input and enable the internal pull-up resistor
	// pinMode(13, OUTPUT);
}

void loop()
{
	reciver->Loop();
	
}

