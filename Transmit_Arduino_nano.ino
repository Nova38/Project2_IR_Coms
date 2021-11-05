// maximum message length in bytes (length of serial read buffer)
const long MAX_MSG_LEN = 64;

// Pin attached to the led driver transistor:
const int PIN = 2;

// time each bit takes to send:
const long toggle_time = 500; // 2000 baud
//const long toggle_time = 1000000; // 1 baud

// reading new msg times out after this many microseconds:
const long msg_timeout = 2042; // at 9600 baud, no reason for reading a byte to take longer than 20/9600 = 1042 microseconds

// last time the transmit bit was set:
long old_time = 0;
// Time the message was started to be received:
long old_time_msg = 0;
// time at the beginning of the loop:
long new_time = 0;

class Message {
public:
  int bytes[MAX_MSG_LEN];
  int len;
  int cur_ind;
  int byte_ind;

  bool get_next() {
    if (byte_ind >= 8) {
      // need to swtich to next byte:
      cur_ind++;
      byte_ind = 0;
      
      // if we have exceeded message length, repeat message:
      if (cur_ind >= len) {
        cur_ind = 0;
      }
    }
    // return byte in message and post-increment byte index:
    return (bytes[cur_ind] & (1 << byte_ind++));
  }
  void clear() {
    len = 0;
    cur_ind = 0;
    byte_ind = 0;
  }
  void add_byte(int next) {
    if (len >= MAX_MSG_LEN) {
      // PROBLEM!
      clear();
      Serial.println("Message is too long!");
    }
    bytes[len++] = next;
  }
};

Message* msg = new Message();
Message* new_msg = new Message();
bool reading_msg = false;

void setup() {
  // initialize serial communications (to PC):
  Serial.begin(9600);

  msg->clear();
  msg->add_byte(0x55); // 0x55 = 01010101
//  msg->add_byte(0x09); // = 00001001
//  msg->add_byte(0x90); // = 10010000
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PIN, OUTPUT);
  old_time = micros();
}


void loop() {
  new_time = micros();

  // update transmit bit:
  if (new_time - old_time >= toggle_time) {
    bool state = msg->get_next();
    // send inverted signal (receiver inverts again via hardware)
    if (state) {
      digitalWrite(PIN, LOW);    // turn the LED off by making the voltage LOW
    } else {
      digitalWrite(PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
    old_time = new_time;
  }

  // read msg to repeat from serial:
  if (!reading_msg) {
    if (Serial.available()) {
      // read serial next loop:
      reading_msg = true;
    }
  } else {
    if (Serial.available()) {
      // add next byte onto the message:
      old_time_msg = new_time;
      int tmp = Serial.read();
      new_msg->add_byte(tmp);
    } else if (new_time - old_time_msg >= msg_timeout) {
      // message has ended
      reading_msg = false;

      // remove last byte in message (always seems to be 10 due to serial?)
      new_msg->len = new_msg->len-1;

      if (new_msg->len != 0) {
        // swap old message with new message:
        Message* tmp = msg;
        msg = new_msg;
        // clean up new message:
        tmp->clear();
        new_msg = tmp;

        // print that the message was received:
        Serial.print("msg len: ");
        Serial.print(msg->len, DEC);
        Serial.print(", bytes: ");
        for (int i = 0; i < msg->len; i++) {
          Serial.print(msg->bytes[i], DEC);
          if (i != msg->len-1)
            Serial.print(", ");
        }
        Serial.println();
      } else {
        // if msg contains just the stop byte
        new_msg->clear();
      }
    }
  }
}
