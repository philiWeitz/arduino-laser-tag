
#include <IRremote.h>

const int RECV_PIN = 11;
const int BUTTON_PIN = 9;

const int LED_PIN = 12;

const char* ID_STRING = "20df10ef";
const long ID_HEX = 0x20df10ef; 

IRsend irsend;
IRrecv irrecv(RECV_PIN);


decode_results results;
int curButtonState = LOW;


void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();

  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void pollForButtonPress() {
  int state = digitalRead(BUTTON_PIN);
  
  if(state != curButtonState) {
    if(state == HIGH) {
      Serial.println("FIRE");
      irsend.sendLG(ID_HEX, 32);
      irrecv.enableIRIn();
    }
    curButtonState = state;
  }
}

void showHit() {
  digitalWrite(LED_PIN, HIGH);
  delay(2000);
  digitalWrite(LED_PIN, LOW);
}


void readIR() {
  if (irrecv.decode(&results)) {
    String resultString = String(results.value, HEX);
    
    if(resultString.equals(ID_STRING)) {
      // send the hit command + who hit you
      Serial.print("HIT,");
      Serial.println(resultString);
      showHit();
    }
    irrecv.resume();
  }
}

void loop() {
  pollForButtonPress();
  readIR();
}



