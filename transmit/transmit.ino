unsigned long last_input_time = 0;
const int serial_rate = 9600; // in bits per second
const int buf_size = 8; // length of buffer
const int pinRec = 2; // output pin
volatile int buf = 0x00AA;  // 16-bit buffer
volatile bool end_trans = false; // end of transmission
const int bit_rate = 500; // 500 us per bit transmission
const int tmp_buf = 0x0041; // constant bit transmission mode
volatile bool start_const_trans = false;  //
const int tmp_buf_value = 126; // tilde is used to start and end constant bit transmission mode
volatile int end_t = 3;
volatile int start_t = 1;
/* used for looping buffer value */
volatile int i = 0; // start loop at i == 0
volatile bool start = true; // initalize with start as true
const int error_packet = 0x0042;
const int ber_packet = 3;
volatile int ber_counter = 0;
volatile bool error = false;

void resetTimer(){
   // TIMER 1 for interrupt frequency 2000 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
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

ISR(TIMER1_COMPA_vect){
   //interrupt commands for TIMER 1 here
    // get data from user
    if(Serial.available() > 0){
      buf = Serial.read(); // read fom serial monitor
      Serial.println(buf, BIN); // print out the buffer as hex 
      // if tilde, send continous
      if(buf == tmp_buf_value){
        buf = tmp_buf;
        start_const_trans = !start_const_trans;
        error = !error;
      }
      end_trans = false;  // flag end of transmission to be false, start should already be true from end of transmission
    }
    else if(start_const_trans && end_trans && start == true){
      end_trans = false;
    }
    if(start && !end_trans){
      digitalWrite(pinRec, LOW);
      resetTimer();
      start = false;
    }
    else if(!end_trans){
       bool K = bitRead(buf, i);
       if(K){
         digitalWrite(pinRec, LOW);
       }
       else{
         digitalWrite(pinRec, HIGH);
       }
       i++;
       if(i >= buf_size){
        end_trans = true;
        i = 0;
        end_t = 0; // how many buffer bits (0 is 1 bit, 1 is 2 bits, ...)
      }
    }
    else if(end_trans){
      if(end_t <= 0){
        start = true;
      }
      else{
        end_t--;
      }
      digitalWrite(pinRec, HIGH);

      if(error && ber_counter > ber_packet){
        buf = error_packet;
        ber_counter = 0;
      }
      else{
        buf = tmp_buf;
        ber_counter++;
      }
    }
}

void setup() {
  // start serial monitor at rate serial_rate
  Serial.begin(serial_rate);
  // put your setup code here, to run once:
  // set pin to OUTPUT mode
  pinMode(pinRec, OUTPUT);
  digitalWrite(pinRec, HIGH);
   // TIMER 1 for interrupt frequency 2000 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
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

void loop() {

}
