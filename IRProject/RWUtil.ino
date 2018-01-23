
#define INPUT_BUFFER_SIZE 40

#define INPUT_END_CHAR '$'
#define INPUT_DELIMITER ","

// sets a 4 string weapon id + weapon type
// GUN,1111,0$

// HIT,TIME_OUT_IN_MS
// HIT,4000$

// enables and disables shooting
// SHOT,ENABLED$ | SHOT,DISABLED$


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
  // get gun type
  uint8_t gunType = atoi(subStr) & 0xf;
  
  // 3 hex = laser tag id (always f0f)
  // 4 hex = gun type (range from 0 to 9999)
  // 1 hex = weapon id (0-15)
  gShotIdHex = 0xf0f00000 | (weaponId << 4) | gunType;

  #ifdef PRINT_DEBUG
  Serial.print("Weapon Id: ");
  Serial.println(weaponId);
  Serial.print("Gun type: ");
  Serial.println(gunType);
  Serial.print("Shot ID: ");
  Serial.println(gShotIdHex);
  #endif
}


void parseHitTimeout() {
  char* subStr = strtok (NULL, INPUT_DELIMITER);
  if(!subStr) { return; }

  gHitTimeout = atoi(subStr);
  
  #ifdef PRINT_DEBUG
  Serial.print("Hit timeout: ");
  Serial.println(gHitTimeout);
  #endif
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


void parseInputBuffer() {
  char* subStr = strtok(inputBuffer, INPUT_DELIMITER);

  if(!subStr) {
    return;
  }
  if (strcmp(subStr, "GUN") == 0) {
    parseGun();
  } else if  (strcmp(subStr, "HIT") == 0) {
    parseHitTimeout();
  } else if  (strcmp(subStr, "SHOT") == 0) {
    parseShotEnabled();
  }
}


void resetBuffer() {
  inputBufferPtr = 0;
  memcpy(inputBuffer, "0", INPUT_BUFFER_SIZE);
}
