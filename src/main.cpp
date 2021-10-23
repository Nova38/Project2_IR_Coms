#include <Arduino.h>
#include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;

// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMicro = 0; // will store last time LED was updated



int currentPos = 0;
const int sampleValSize = 8;
int sampledVals[sampleValSize] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup()
{
	//start serial connection
	Serial.begin(115200);
	//configure pin 2 as an input and enable the internal pull-up resistor
	pinMode(recPin, INPUT_PULLUP);
	pinMode(13, OUTPUT);
}

void loop()
{

	unsigned long currentMicro = micros();
	unsigned long intervalMicro = currentMicro - previousMicro;

	if (intervalMicro >= interval)
	{
		onStateChange();

		previousMicro = currentMicro;
	}
	if (currentPos == sampleValSize)
	{

		onByteRec();
		currentPos = 0;
	}
}

void onStateChange()
{
	int sample = digitalRead(2);
	sampledVals[currentPos] = sample;
	currentPos++;
}

void onByteRec()
{
	Serial.print("\nRecived: ");
	for (int i = 0; i < sampleValSize; i++)
	{
		Serial.print(sampledVals[i]);
		sampledVals[i] = 0;
	}
}