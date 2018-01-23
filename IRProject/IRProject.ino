#define PRINT_DEBUG

#include <IRremote.h>

const int RECV_PIN = 11;

// Arduino mini pro
// const int BUTTON_PIN = 9; 
// Arduino mega
const int BUTTON_PIN = 10; 

const int LED_PIN = 12;

// IR receiver pin = 11
// IR send pin arduino mini pro = 3
// IR send pin mega mini pro = 9

const uint16_t HEADER_LASER = 0xf0f;
const uint16_t HEADER_REMOTE = 0x20df;


bool gShotEnabled = true;
uint32_t gShotIdHex = 0xf0f00000;
uint16_t gHitTimeout = 2000;


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

  delay(2000);
  Serial.println("AT+NAMELaserTag2");
  delay(1000);
  Serial.println("AT+PASS1234");
}

void pollForButtonPress() {
  if (gShotEnabled) {
    int state = digitalRead(BUTTON_PIN);
    
    if(state != curButtonState) {
      if(state == HIGH) {
        Serial.println("FIRE");
        irsend.sendLG(gShotIdHex, 32);
        irrecv.enableIRIn();
      }
      curButtonState = state;
    }
  }
}

void showHit() {
  digitalWrite(LED_PIN, HIGH);
  delay(gHitTimeout);
  digitalWrite(LED_PIN, LOW);
}


void readIR() {
  if (irrecv.decode(&results)) {
    // decode the result
    // 3 hex = laser tag id (always f0f)
    // 4 hex = gun type (range from 0 to ffff)
    // 1 hex = weapon id (0-f)
    uint16_t header = (0xffff0000 & results.value) >> 16;
    uint16_t weaponId = (0x000ffff0 & results.value) >> 4;
    uint8_t gunType = (0xf & results.value);

    #ifdef PRINT_DEBUG
    Serial.print("Signal: ");
    Serial.println(results.value);
    Serial.print("Header: ");
    Serial.println(header);
    #endif

    // check that it is our laser tag system
    if(header == HEADER_LASER || header == HEADER_REMOTE) {
      // send the hit command + who hit you + gun type
      Serial.print("HIT,");
      Serial.print(weaponId);
      Serial.print(",");
      Serial.println(gunType);
      showHit();
    }
    irrecv.resume();
  }
}

void loop() {
  pollForButtonPress();
  readIR();
  readInput();
}



