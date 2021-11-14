//#include <Arduino.h>
// #include "IR_Coms.hpp"

// constants won't change. Used here to set a pin number:
const int recPin = 2;
const int errorPin = 3;


// message storage
const int buffer_size = 7;
volatile byte  buffer = 0;
volatile int current_buffer_bit = 0;

const int buffer_storage_size = 20;
volatile int buffer_storage[20];

const byte expected = 0x41;

// this is a flag for when the loop should print the buffer out
volatile bool ready_to_print = false;
volatile bool has_recived_start = false;
volatile bool has_recived_end = true;

volatile int err_counter = 0;
volatile int packet_counter = 0;
volatile float error_rate = 0;
const int num_of_packets = 200; // corresponds to 1 second

volatile bool error_mode = false;

volatile bool pin_current_state = 1;

// working current byte
void reset_timer(){
    // TIMER 1 for interrupt frequency 2000 Hz:
      cli();    // stop interrupts
      TCCR1A = 0; // set entire TCCR1A register to 0
      TCCR1B = 0; // same for TCCR1B
      TCNT1 = 0;  // initialize counter value to 0
      // set compare match register for 2000 Hz increments
      OCR1A = 7999; // = 16000000 / (1 * 2000) - 1 (must be <65536)
      // turn on CTC mode
      TCCR1B |= (1 << WGM12);
      // Set CS12, CS11 and CS10 bits for 1 prescaler
      TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
      // enable timer compare interrupt
      TIMSK1 |= (1 << OCIE1A);
      sei(); // allow interrupts
}




void detectStart()
{
  
  noInterrupts();
  if (!has_recived_start)
  {
    // counter++;

    // if(counter % 1 == 0){
      Serial.println("          ");



    // }
    // Serial.println("DFDSD");
    has_recived_start = true;
    has_recived_end = false;
    reset_timer();

    // has_recived_start = true;
    // Serial.println("Found Start Bit");
  }
  interrupts();
}


void setup()
{

  // TIMER 1 for interrupt frequency 2000 Hz:
  cli();    // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1 = 0;  // initialize counter value to 0
  // set compare match register for 2000 Hz increments
  OCR1A = 7999; // = 16000000 / (1 * 2000) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts

  // reset_timer();

  pinMode(recPin, INPUT);
  pinMode(errorPin, INPUT_PULLUP);

  
  

  attachInterrupt(digitalPinToInterrupt(recPin), detectStart, RISING);

  Serial.begin(115200);
}

ISR(TIMER1_COMPA_vect)
{


  if (has_recived_start)
  {
    pin_current_state = digitalRead(recPin);

    if (current_buffer_bit == buffer_size)
    {

      bitWrite(buffer, current_buffer_bit, pin_current_state);
      current_buffer_bit++;
      packet_counter++;
      if(packet_counter >= num_of_packets){
        Serial.print("Error rate: ");
        Serial.println(error_rate, 4);
        error_rate = (float)err_counter/((float)num_of_packets);
        packet_counter = 0;
        err_counter = 0;
      }
      bool error_detected = buffer != expected;
      // Serial.print("\tDetected : ");
       Serial.print((char)buffer);
      Serial.print(" :    ");      
      Serial.print(buffer, HEX);
      // Serial.print(" : ");
      // Serial.print(buffer, BIN);
      if(error_mode){
        if( error_detected){
          err_counter++;
          Serial.println(" : Error");
        }
      }else{
        Serial.print(" : ");      

        Serial.println((char)buffer);
      }

      buffer = 0;
      current_buffer_bit = 0;
      has_recived_start = false;

      TCCR1A = 0; // set entire TCCR1A register to 0
        TCCR1B = 0; // same for TCCR1B
      TCNT1 = 0;  // initialize counter value to 0

      // has_recived_end = true;
    }
    else
    {
      bitWrite(buffer, current_buffer_bit, pin_current_state);
      // if (pin_current_state)
      // {
      //  buffer = buffer << 1;

      // }else{
      //  buffer = buffer << 0;
      // }

      current_buffer_bit++;
    }
  }

}
unsigned long previousMillis = 0;
unsigned long err_interval = 1000;
unsigned long err_counter_interval = 5000;
void loop()
{
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= err_interval){
    previousMillis = currentMillis;
    
    error_mode = !digitalRead(errorPin);

    // noInterrupts();

    // interrupts();

  }
  /*
  if (error_mode && currentMillis - previousMillis >= err_counter_interval)
  {
    Serial.print("Errors per 5 sec : ");
    Serial.println(err_counter);
    err_counter = 0;
  }
  */
  
}
