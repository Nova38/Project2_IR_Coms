
unsigned long last_input_time = 0;

const int serial_rate = 9600; // in bits per second
const int buf_size = 8; // length of buffer

const int pinRec = 2; // output pin

volatile int buf = 0x00AA;  // 16-bit buffer
volatile bool end_trans = false; // end of transmission
const int bit_rate = 500; // 500 us per bit transmission

const int tmp_buf = 0x00B7; // constant bit transmission mode
volatile bool start_const_trans = false;  //
const int tmp_buf_value = 126; // tilde is used to start and end constant bit transmission mode

const int big_buf_size = 12; // size of big buffer
volatile int current_big_buf_size = 0; // number of items in big_buff
volatile int big_buf[big_buf_size]; // large buffer for storing values
volatile bool ready_to_transmit_big_buf = false; // boolean for checking if big_buffer is ready_to_transmit
const int big_buf_fill_code = 126; // tilde is used to begin big buffer transmit cycle
volatile bool fill_proc = false;  // set true if filling is init.

volatile int end_t = 3;
volatile int start_t = 1;

/* used for looping buffer value */
volatile int i = 0; // start loop at i == 0
volatile bool start = true; // initalize with start as true

/**
 * Transmit Data frame:
 * In HEX
 * [1 HIGH|DATA N]
 */

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
      }

/*
      //big buffer code
      // check and see if buffer is same as fill code and check if fill process is not serviced
      if(buf == big_buf_fill_code && fill_proc == false){
        fill_proc = true;
        
      }
      // if fill process is servied and current buffer size is less than max buffer size
      else if(fill_proc && current_big_buf_size <= big_buf_size && buf != big_buf_fill_code){
        // begin storing values from buf
        big_buf[current_big_buf_size] = buf;
        current_big_buf_size++;
      }
      //if at size, transmit when ready, or when user determines if buffer is ready to transmit
      if((current_big_buf_size >= big_buf_size && fill_proc) || (buf == big_buf_fill_code && fill_proc)){
        ready_to_transmit_big_buf = true;
        fill_proc = false;
      }
*/

      end_trans = false;  // flag end of transmission to be false, start should already be true from end of transmission
    }
    else if(start_const_trans && end_trans && start == true){
      end_trans = false;
    }

/*
    // fill buf with big_buf values when needed
    if(ready_to_transmit_big_buf && start && current_big_buf_size > 0){
      current_big_buf_size--;
      buf = big_buf[current_big_buf_size];
    }
  
    if(current_big_buf_size <= 0 && ready_to_transmit_big_buf){
      ready_to_transmit_big_buf = false;
      fill_proc = false;
      current_big_buf_size = 0;
    }
  */
    
    if(start && !end_trans){
      digitalWrite(pinRec, LOW);
      resetTimer();
      /*
      if(start_t == 1){
        resetTimer();
      }
      */
      /*
      if(start_t <= 0){
        start = false;
      }
      else{
        start_t--;
      }
      */
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
        //start_t = 1;
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
    }
}

void setup() {
  // start serial monitor at rate serial_rate
  Serial.begin(serial_rate);
  
  // put your setup code here, to run once:

  // set pin to OUTPUT mode
  pinMode(pinRec, OUTPUT);

  // generate big_buf_tmp
  //genBigBuf_tmp();

  //initalize with LOW
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
/*
  // put your main code here, to run repeatedly:
    noInterrupts();
    if(Serial.available() > 0){
      buf = Serial.read(); // read fom serial monitor
      Serial.println(buf, BIN); // print out the buffer as hex
  
    
      // if tilde, send continous
      if(buf == tmp_buf_value){
        buf = tmp_buf;
        start_const_trans = !start_const_trans;
      }


      //big buffer code
      // check and see if buffer is same as fill code and check if fill process is not serviced
      if(buf == big_buf_fill_code && fill_proc == false){
        fill_proc = true;
        
      }
      // if fill process is servied and current buffer size is less than max buffer size
      else if(fill_proc && current_big_buf_size <= big_buf_size && buf != big_buf_fill_code){
        // begin storing values from buf
        big_buf[current_big_buf_size] = buf;
        current_big_buf_size++;
        for(int i_p; i_p < current_big_buf_size; i_p++){
            Serial.println(big_buf[i_p], BIN);
        }
      }
      //if at size, transmit when ready, or when user determines if buffer is ready to transmit
      if((current_big_buf_size >= big_buf_size && fill_proc) || (buf == big_buf_fill_code && fill_proc)){
        ready_to_transmit_big_buf = true;
        fill_proc = false;
      }

      end_trans = false;  // flag end of transmission to be false, start should already be true from end of transmission
    }
    else if(start_const_trans && end_trans && start == true){
      end_trans = false;
    }

        // fill buf with big_buf values when needed
    if(ready_to_transmit_big_buf && start && current_big_buf_size > 0){
      current_big_buf_size--;
      buf = big_buf[current_big_buf_size];
    }
  
    if(current_big_buf_size <= 0 && ready_to_transmit_big_buf){
      ready_to_transmit_big_buf = false;
      fill_proc = false;
      current_big_buf_size = 0;
    }
    interrupts();
    */
}
