
#define INPUT_BUFFER_SIZE 40

#define INPUT_END_CHAR '$'
#define INPUT_DELIMITER ","

/** Configuration options:
- sets a 4 string weapon id + team id + weapon type
GUN,1111,0,0$

-enables and disables friendly fire
FRIEND,ENABLED$ | FRIEND,DISABLED$

- sets the timeout after hit was received
HIT,4000$

- enables and disables receive hit
HIT,ENABLED$ | HIT,DISABLED$

- enables and disables shooting
SHOT,ENABLED$ | SHOT,DISABLED$

- sets the press time for a long press
LONG,3000$

- shows connected signal (blink LED)
CONNECTED$

**/

int inputBufferPtr = 0;
char inputBuffer[INPUT_BUFFER_SIZE];


void readInput() {

  while (Serial.available() > 0) {
    char inputChar = Serial.read();

    // input end character found -> execute command
    if(inputChar == INPUT_END_CHAR) {
      inputBuffer[inputBufferPtr] = '\0';
      parseInputBuffer();
      resetBuffer();
      
    // add char to input buffer
    } else {
      inputBuffer[inputBufferPtr] = inputChar;
      ++inputBufferPtr;
    } 

    // buffer overflow -> reset input buffer
    if(inputBufferPtr >= INPUT_BUFFER_SIZE) {
      resetBuffer();
    }   
  }
}


void parseGun() {
  char* subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }
  // get weapon id
  uint16_t weaponId = atoi(subStr);

  subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }
  // get team id
  uint8_t teamId = atoi(subStr) & 0xf;

  subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }
  // get gun type
  uint8_t gunType = atoi(subStr) & 0xf;

  // set global weapon id
  gWeaponId = weaponId;

  // set global team id
  gTeamId = teamId;
  
  // 2 hex = laser tag id (always 0xaa)
  // 4 hex = weapon id (range from 0x0 to 0xffff)
  // 1 hex = team id (range from 0x0 - 0xf)
  // 1 hex = gun type (0x0-0xf)
  gShotIdHex = 0xaa000000 | (gWeaponId << 8) | (gTeamId << 4) | gunType;

  #ifdef PRINT_DEBUG
  Serial.print("Weapon Id: ");
  Serial.println(weaponId, HEX);
  Serial.print("Team Id: ");
  Serial.println(teamId, HEX);
  Serial.print("Gun type: ");
  Serial.println(gunType, HEX);
  Serial.print("Shot ID: ");
  Serial.println(gShotIdHex, HEX);
  #endif
}

void parseFriendlyFire() {
  char* subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }

  if (strcmp(subStr, "ENABLED") == 0) {
    gFriendlyEnabled = true;    
    #ifdef PRINT_DEBUG
    Serial.println("Friendly enabled");
    #endif
    
  } else if (strcmp(subStr, "DISABLED") == 0) {
    gFriendlyEnabled = false;
    #ifdef PRINT_DEBUG
    Serial.println("Friendly disabled");
    #endif
    
  }
}

void parseHitTimeout() {
  char* subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }

  if (strcmp(subStr, "ENABLED") == 0) {
    gHitEnabled = true;    
    #ifdef PRINT_DEBUG
    Serial.println("Hit enabled");
    #endif
    
  } else if (strcmp(subStr, "DISABLED") == 0) {
    gHitEnabled = false;
    #ifdef PRINT_DEBUG
    Serial.println("Hit disabled");
    #endif
    
  } else {
    gHitTimeout = atoi(subStr);
    
    #ifdef PRINT_DEBUG
    Serial.print("Hit timeout: ");
    Serial.println(gHitTimeout);
    #endif
  }
}


void parseShotEnabled() {
  char* subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }

  if(strcmp(subStr, "ENABLED") == 0) {
    gShotEnabled = true;
  } else if(strcmp(subStr, "DISABLED") == 0) {
    gShotEnabled = false;
  }

  #ifdef PRINT_DEBUG
  Serial.print("Shot enabled: ");
  Serial.println(gShotEnabled);
  #endif
}


void parseLongPress() {
  char* subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }

  gLongPressTime = atoi(subStr);
  
  #ifdef PRINT_DEBUG
  Serial.print("Long press time: ");
  Serial.println(gLongPressTime);
  #endif
}


void showConnected() {
  for (int i = 0; i < 3; ++i) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}


void parseInputBuffer() {
  char* subStr = strtok(inputBuffer, INPUT_DELIMITER);

  if(!subStr) {
    return;
  }
  if (strcmp(subStr, "GUN") == 0) {
    parseGun();
  } else if (strcmp(subStr, "FRIEND") == 0) {
    parseFriendlyFire();
  } else if (strcmp(subStr, "HIT") == 0) {
    parseHitTimeout();
  } else if (strcmp(subStr, "SHOT") == 0) {
    parseShotEnabled();
  } else if (strcmp(subStr, "LONG") == 0) {
    parseLongPress();
  } else if (strcmp(subStr, "CONNECTED") == 0) {
    showConnected();
  }
}


void resetBuffer() {
  inputBufferPtr = 0;
  memcpy(inputBuffer, "0", INPUT_BUFFER_SIZE);
}
