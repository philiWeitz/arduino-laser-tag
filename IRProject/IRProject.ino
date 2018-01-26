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
const uint16_t HEADER_REMOTE = 0x20d;

// generate a random weapon id
uint32_t gWeaponId = 0;

bool gShotEnabled = true;
bool gHitEnabled = true;
uint32_t gShotIdHex = 0xf0f00000;
uint16_t gHitTimeout = 2000; 
uint16_t gLongPressTime = 2000;

IRsend irsend;
IRrecv irrecv(RECV_PIN);

decode_results results;
int curButtonState = LOW;

uint32_t MAX_UINT32 = 0xffffffff;
uint32_t longPressTimestamp = MAX_UINT32;


void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();

  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  randomSeed(analogRead(A0));
  gWeaponId = (random(0,0xffff));
  gShotIdHex = 0xf0f00000 | gWeaponId << 4;
    
  delay(2000);
  Serial.println("AT+NAMELaserTag");
  delay(1000);
  Serial.println("AT+PASS1234");
  delay(1000);

  #ifdef PRINT_DEBUG
  Serial.println("--------------------");
  Serial.print("Weapon ID: ");
  Serial.println(gWeaponId, HEX);
  Serial.print("Shot ID: ");
  Serial.println(gShotIdHex, HEX);  
  Serial.print("Shot enabled: ");
  Serial.println(gShotEnabled); 
  Serial.print("Hit enabled: ");
  Serial.println(gHitEnabled);
  Serial.println("--------------------");  
  #endif
}


void pollForButtonPress() {
  int state = digitalRead(BUTTON_PIN);
    
  if(state != curButtonState) {

    if(gShotEnabled && state == HIGH) {
      // fire the gun
      Serial.println("FIRE"); 
      irsend.sendLG(gShotIdHex, 32);
      irrecv.enableIRIn();
    
    } else if(!gShotEnabled && state == HIGH) {
      // send fire disabled
      Serial.println("FIRE_DISABLED");
      delay(10);

      // ENABLE long press if shot is disabled
      longPressTimestamp = millis() + gLongPressTime;
    
    } else if(state == LOW) {
      longPressTimestamp = MAX_UINT32;
    }
    curButtonState = state;
  }

  // check if long press detected
  if(longPressTimestamp < millis()) {
    longPressTimestamp = MAX_UINT32;
    Serial.println("LONG_PRESS"); 
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
    // 3 hex = laser tag id (always 0xf0f)
    // 4 hex = weapon id (range from 0x0 to 0xffff)
    // 1 hex = gun type (0x0-0xf)
    uint16_t header = (0xfff00000 & results.value) >> 20;
    uint16_t weaponId = (0x000ffff0 & results.value) >> 4;
    uint8_t gunType = (0xf & results.value);

    #ifdef PRINT_DEBUG
    Serial.print("Signal: ");
    Serial.println(results.value, HEX);
    Serial.print("Header: ");
    Serial.println(header, HEX);
    #endif

    // check that it is our laser tag system
    // also check that you didn't shot yourself
    if(gHitEnabled && gWeaponId != weaponId && (header == HEADER_LASER || header == HEADER_REMOTE)) {
      // send the hit command + who hit you + gun type
      Serial.print("HIT,");
      Serial.print(weaponId, HEX);
      Serial.print(",");
      Serial.println(gunType, HEX);
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


