#include <Arduino.h>
#include <Wire.h>
#include "Sucofunkey.h"
#include "Adafruit_MCP23017.h"
#include <cppQueue.h> // https://github.com/SMFSW/Queue


static volatile bool _keyPressedInterrupt1 = false;
static volatile bool _keyPressedInterrupt2 = false;
static volatile bool _keyPressedInterrupt3 = false;
static volatile bool _keyPressedInterrupt4 = false;
static volatile bool _keyPressedInterrupt5 = false;

static volatile bool _encoderInterrupt = false;

cppQueue keyQueue(sizeof(Sucofunkey::Key), 42, FIFO);


Sucofunkey::Sucofunkey(int intPinMCP1, int intPinMCP2, int intPinMCP3, int intPinMCP4, int intPinMCP5) {
  _intKeyPin1 = intPinMCP1;
  _intKeyPin2 = intPinMCP2;
  _intKeyPin3 = intPinMCP3;
  _intKeyPin4 = intPinMCP4;
  _intKeyPin5 = intPinMCP5;

  // set Fader LED PIN
  pinMode(faderLEDPin, OUTPUT);

  // Is the Function Key held for a key combination?
  _fnKeyHold = false;
  _fnKeyInterrupted = false;

  // Is the Menu Key held for a key combination with rotary encoder push buttons or record button?
  _menuKeyHold = false;
      
  // needs to be true to e.g. not change menu context while recording
  _ignoreKeys = false;

  pinMode(_intKeyPin1, INPUT_PULLUP);
  pinMode(_intKeyPin2, INPUT_PULLUP);
  pinMode(_intKeyPin3, INPUT_PULLUP);
  pinMode(_intKeyPin4, INPUT_PULLUP);
  pinMode(_intKeyPin5, INPUT_PULLUP);

  _setupInterrupts();    


  // Init mcp23017 ICs of SucoKey
  _mcp1.begin(0, &Wire2);
  _mcp2.begin(1, &Wire2);
  _mcp3.begin(2, &Wire2);
  _mcp4.begin(3, &Wire2);
  _mcp5.begin(4, &Wire2);
  
  _mcp1.setupInterrupts(true,false,LOW);
  
  for (int i=0; i<16; i++) {
      if (_input_mcp1[i]) {
        // Input
        _mcp1.pinMode(i, INPUT);
        _mcp1.pullUp (i, HIGH);
        _mcp1.setupInterruptPin(i, CHANGE);
      } else {
        // LED
        _mcp1.pinMode(i, OUTPUT);
        _mcp1.digitalWrite(i, LOW);
      }
  }

  _mcp2.setupInterrupts(true,false,LOW);
  
  for (int i=0; i<16; i++) {
      if (_input_mcp2[i]) {
        // Input
        _mcp2.pinMode(i, INPUT);
        _mcp2.pullUp (i, HIGH);
        _mcp2.setupInterruptPin(i, CHANGE);
      } else {
        // LED
        _mcp2.pinMode(i, OUTPUT);
        _mcp2.digitalWrite(i, LOW);          
      }
  }

  _mcp3.setupInterrupts(true,false,LOW);
  
  for (int i=0; i<16; i++) {
      if (_input_mcp3[i]) {
        // Input
        _mcp3.pinMode(i, INPUT);
        _mcp3.pullUp (i, HIGH);
        _mcp3.setupInterruptPin(i, CHANGE);
      } else {
        // LED
        _mcp3.pinMode(i, OUTPUT);
        _mcp3.digitalWrite(i, LOW);          
      }
  }

  _mcp4.setupInterrupts(true,false,LOW);
  
  for (int i=0; i<16; i++) {
      if (_input_mcp4[i]) {
        // Input
        _mcp4.pinMode(i, INPUT);
        _mcp4.pullUp (i, HIGH);
        _mcp4.setupInterruptPin(i, CHANGE);
      } else {
        // LED
        _mcp4.pinMode(i, OUTPUT);
        _mcp4.digitalWrite(i, LOW);          
      }
  }

  _mcp5.setupInterrupts(true,false,LOW);
  
  for (int i=0; i<16; i++) {
      if (_input_mcp5[i]) {
        // Input
        _mcp5.pinMode(i, INPUT);
        _mcp5.pullUp (i, HIGH);
        _mcp5.setupInterruptPin(i, CHANGE);
      } else {
        // LED
        _mcp5.pinMode(i, OUTPUT);
        _mcp5.digitalWrite(i, LOW);          
      }
  }

  // force a first read and "reset" the MCP23017 IRQ system
  _keyPressedInterrupt1 = false;
  _values_mcp1_current = _mcp1.readGPIOAB();
  _keyPressedInterrupt2 = false;
  _values_mcp2_current = _mcp2.readGPIOAB();
  _keyPressedInterrupt3 = false;
  _values_mcp3_current = _mcp3.readGPIOAB();    
  _keyPressedInterrupt4 = false;
  _values_mcp4_current = _mcp4.readGPIOAB();
  _keyPressedInterrupt5 = false;
  _values_mcp5_current = _mcp5.readGPIOAB();
}


// -------------------------------------------------------------------------------------------------
// SucoKey Interrupt Handling
// -------------------------------------------------------------------------------------------------


void Sucofunkey::_setupInterrupts() {
   attachInterrupt(digitalPinToInterrupt(_intKeyPin1), _intCallBack1, FALLING);
   attachInterrupt(digitalPinToInterrupt(_intKeyPin2), _intCallBack2, FALLING);
   attachInterrupt(digitalPinToInterrupt(_intKeyPin3), _intCallBack3, FALLING);
   attachInterrupt(digitalPinToInterrupt(_intKeyPin4), _intCallBack4, FALLING);
   attachInterrupt(digitalPinToInterrupt(_intKeyPin5), _intCallBack5, FALLING);
}

void Sucofunkey::_intCallBack1() {
  _keyPressedInterrupt1=true;
} 

void Sucofunkey::_intCallBack2() {
  _keyPressedInterrupt2=true;
}

void Sucofunkey::_intCallBack3() {
  _keyPressedInterrupt3=true;
}

void Sucofunkey::_intCallBack4() {
  _keyPressedInterrupt4=true;
}

void Sucofunkey::_intCallBack5() {
  _keyPressedInterrupt5=true;
}


void Sucofunkey::_handleKeyPressed() {

  if (_keyPressedInterrupt1) {        
        _keyPressedInterrupt1 = false;
        _values_mcp1_last = _values_mcp1_current;
        _values_mcp1_current = _mcp1.readGPIOAB();
        _mcpKeysPressedToQueueEvents(_values_mcp1_current, _values_mcp1_last, 0, _input_mcp1, _input_type_mcp1);
  }

  if (_keyPressedInterrupt2) {        
        _keyPressedInterrupt2 = false;
        _values_mcp2_last = _values_mcp2_current;
        _values_mcp2_current = _mcp2.readGPIOAB();
        _mcpKeysPressedToQueueEvents(_values_mcp2_current, _values_mcp2_last, 16, _input_mcp2, _input_type_mcp2);        
  }

  if (_keyPressedInterrupt3) {        
        _keyPressedInterrupt3 = false;
        _values_mcp3_last = _values_mcp3_current;
        _values_mcp3_current = _mcp3.readGPIOAB();
        _mcpKeysPressedToQueueEvents(_values_mcp3_current, _values_mcp3_last, 32, _input_mcp3, _input_type_mcp3);
  }

  if (_keyPressedInterrupt4) {        
        _keyPressedInterrupt4 = false;
        _values_mcp4_last = _values_mcp4_current;
        _values_mcp4_current = _mcp4.readGPIOAB();
        _mcpKeysPressedToQueueEvents(_values_mcp4_current, _values_mcp4_last, 48, _input_mcp4, _input_type_mcp4);
  }

  if (_keyPressedInterrupt5) {
        _keyPressedInterrupt5 = false;
        _values_mcp5_last = _values_mcp5_current;
        _values_mcp5_current = _mcp5.readGPIOAB();
        _mcpKeysPressedToQueueEvents(_values_mcp5_current, _values_mcp5_last, 64, _input_mcp5, _input_type_mcp5);        
  }
}

void Sucofunkey::_mcpKeysPressedToQueueEvents(uint16_t mcpValuesCurrent, uint16_t mcpValuesOld, uint16_t mcpOffset, bool* mcpInput, byte* mcpInputTypes) {  
  // get differing values -> XOR
  uint16_t diffVals = mcpValuesCurrent ^ mcpValuesOld;
  bool keyStatus;
  int offset = 0;
  bool sendKey = true;
  byte inputType;   
  bool enc1Handled = false;
  bool enc2Handled = false;
  bool enc3Handled = false;
  bool enc4Handled = false;


  for (int i=0; i<16; i++) {
    // check if value corresponds to an input on the mcp and if a change happened (bit switched)
    if (mcpInput[i] == true && getBooleanValueFrom16BitInt(diffVals, i) == true) {      

        // 100 = Function Key hold, 200 = Menu Key hold
        offset = 0;
        sendKey = true;

        // read encoders
        if (mcpInputTypes[i] == ENCODER) {
          sendKey = false;

          switch(i+offset) {
            case ENCODER_4_2:
            case ENCODER_4_1:
              if (!enc4Handled) _readRotaryEncoder(3, getBooleanValueFrom16BitInt(mcpValuesCurrent, 1), getBooleanValueFrom16BitInt(mcpValuesCurrent, 2),  _fnKeyHold ? FN_ENCODER_4 : ENCODER_4);            
              enc4Handled = true;
              break;
            case ENCODER_3_2:
            case ENCODER_3_1:
              if (!enc3Handled) _readRotaryEncoder(2, getBooleanValueFrom16BitInt(mcpValuesCurrent, 4), getBooleanValueFrom16BitInt(mcpValuesCurrent, 5),  _fnKeyHold ? FN_ENCODER_3 : ENCODER_3);
              enc3Handled = true;
              break;
            case ENCODER_2_2:
            case ENCODER_2_1:
              if (!enc2Handled) _readRotaryEncoder(1, getBooleanValueFrom16BitInt(mcpValuesCurrent, 8), getBooleanValueFrom16BitInt(mcpValuesCurrent, 9),  _fnKeyHold ? FN_ENCODER_2 : ENCODER_2);
              enc2Handled = true;            
              break;
            case ENCODER_1_2:
            case ENCODER_1_1:
              if (!enc1Handled) _readRotaryEncoder(0, getBooleanValueFrom16BitInt(mcpValuesCurrent, 11), getBooleanValueFrom16BitInt(mcpValuesCurrent, 12),  _fnKeyHold ? FN_ENCODER_1 : ENCODER_1);
              enc1Handled = true;            
              break;
            default:
              break;
          }
        }

        // inverting the boolean value because the mcp signals LOW when a button is pressed!
        keyStatus = !getBooleanValueFrom16BitInt(mcpValuesCurrent, i);

        // Function Key?
        if (i+mcpOffset == FUNCTION) {
          if (keyStatus) {
            _fnKeyHold = true;
            _fnKeyMillis = millis();
            _fnKeyInterrupted = false;
            sendKey = false;
          } else {
            // Fuction key pressed for at least 3 seconds? send event FN_FUNCTION -> FUNCTION + 100
            if (_fnKeyHold && !_fnKeyInterrupted && (millis() - _fnKeyMillis >= 3000)) {
              offset = 100;
              keyStatus = true;
            } else {
              // function key pressed and released within 1 second, send function key to queue
              if (_fnKeyHold && !_fnKeyInterrupted && (millis() - _fnKeyMillis < 1500)) {
                keyStatus = true;
              } else {
                sendKey = false;
              }
            }
            _fnKeyHold = false;
            _fnKeyInterrupted = false;
          }          
        } else {
          if (_fnKeyHold) _fnKeyInterrupted = true;
        }


        // Menu Key?
        if (i+mcpOffset == MENU) {
          if (keyStatus) {
            _menuKeyHold = true;
            _menuKeyMillis = millis();
            _menuKeyInterrupted = false;
            sendKey = false;
          } else {
            // Fuction key pressed for at least 3 seconds? send event FN_FUNCTION -> FUNCTION + 100
            if (_menuKeyHold && !_menuKeyInterrupted && (millis() - _menuKeyMillis >= 3000)) {
              offset = 200;
              keyStatus = true;
            } else {
              // function key pressed and released within 1 second, send function key to queue
              if (_menuKeyHold && !_menuKeyInterrupted && (millis() - _menuKeyMillis < 1500)) {
                keyStatus = true;
              } else {
                sendKey = false;
              }
            }
            _menuKeyHold = false;
            _menuKeyInterrupted = false;
          }          
        } else {
          if (_menuKeyHold) _menuKeyInterrupted = true;
        }

        if (_menuKeyHold) offset = 200; 
        if (_fnKeyHold) offset = 100;         

      if (sendKey) {
        inputType = mcpInputTypes[i];
        if (_menuKeyHold && inputType == KEY_NOTE) inputType = KEY_MENU_NOTE;
        if (_menuKeyHold && inputType == KEY_OPERATION) inputType = KEY_MENU_OPERATION;

        if (_fnKeyHold && inputType == KEY_NOTE) inputType = KEY_FN_NOTE;
        if (_fnKeyHold && inputType == KEY_OPERATION) inputType = KEY_FN_OPERATION;

        keyQueueStruct k = {i+mcpOffset+offset, keyStatus, _ignoreKeys, inputType};
        keyQueue.push(&k);
      }
    }
  }
}


int Sucofunkey::_readRotaryEncoder(byte encoderNr, uint8_t valuePinL, uint8_t valuePinR, int eventId) {
  static unsigned char p;

  switch(p=valuePinL | valuePinR<<1) {
    case 0:
      if(_encoderTempValues[encoderNr]==1) {
        _encoderTempValues[encoderNr]=p;
        keyQueueStruct k = {eventId, true, _ignoreKeys, ENCODER};
        keyQueue.push(&k);        
        return +1;
      } else if(_encoderTempValues[encoderNr]==2) {
        _encoderTempValues[encoderNr]=p;        
        keyQueueStruct k = {eventId, false, _ignoreKeys, ENCODER};
        keyQueue.push(&k);
        return -1;
      }
      break;
    case 1:
    case 2:
      if(_encoderTempValues[encoderNr]==3) _encoderTempValues[encoderNr]=p;
      break;
    case 3:
      _encoderTempValues[encoderNr]=p;
  }
  
  return 0;  
};


// check if an interrupt was fired and add key to queue
boolean Sucofunkey::hasKeyPressed() {
  if (_keyPressedInterrupt1 || _keyPressedInterrupt2 || _keyPressedInterrupt3 || _keyPressedInterrupt4 || _keyPressedInterrupt5) {
    _handleKeyPressed();
    return true;
  } else {
    return false;
  }
}

// -------------------------------------------------------------------------------------------------

void Sucofunkey::printQueue() {
  
  // handle queue
  if (!keyQueue.isEmpty()) {
    while(keyQueue.getCount() > 0) {
      keyQueueStruct k;
      keyQueue.pop(&k);
      //handleEvent(k.index, k.pressed);            
      Serial.print(k.index);
      Serial.print(" --> ");
      Serial.print(k.pressed);
      Serial.println();      
    }
  }
}

// -------------------------------------------------------------------------------------------------

boolean Sucofunkey::hasEvents() { 
  if(!keyQueue.isEmpty()) {
    return true;     
  } else {
    return false;
  }
};

Sucofunkey::Key Sucofunkey::getNextEvent() { 
  keyQueueStruct k;
  keyQueue.pop(&k);
  return k;
};

void Sucofunkey::addApplicationEventToQueue(int eventId) {
  keyQueueStruct k = { eventId, true, false, EVENT_APPLICATION };
  keyQueue.push(&k);
}


// -------------------------------------------------------------------------------------------------
// Keys may be ignored, if a Function is needing full attention e.g. recording
// Key strokes will be added to queue, but are flagged.
// -------------------------------------------------------------------------------------------------

void Sucofunkey::setIgnoreKeys(boolean state) {  
  _ignoreKeys = state;
}

// -------------------------------------------------------------------------------------------------

void Sucofunkey::setLEDState(int led, bool state) {
  int mcp = led/16;

  switch(mcp) {
    case 0:
            _mcp1.digitalWrite(led, state ? HIGH : LOW);
            break;
    case 1:
            _mcp2.digitalWrite(led-16, state ? HIGH : LOW);    
            break;
    case 2:
            _mcp3.digitalWrite(led-32, state ? HIGH : LOW);    
            break;
    case 3:
            _mcp4.digitalWrite(led-48, state ? HIGH : LOW);    
            break;
    case 4:
            _mcp5.digitalWrite(led-64, state ? HIGH : LOW);    
            break;
  }   
}

// -------------------------------------------------------------------------------------------------

// nr = 1..3  0 = all off
void Sucofunkey::setBank(byte nr) {
  _activeBank = nr;
    
  switch (nr) {
    case 1:  setLEDState(LED_BANK_1, true);
             setLEDState(LED_BANK_2, false);
             setLEDState(LED_BANK_3, false);
             break;
    case 2:  setLEDState(LED_BANK_1, false);
             setLEDState(LED_BANK_2, true);
             setLEDState(LED_BANK_3, false);
             break;
    case 3:  setLEDState(LED_BANK_1, false);
             setLEDState(LED_BANK_2, false);
             setLEDState(LED_BANK_3, true);
             break;
    default: setLEDState(LED_BANK_1, false);
             setLEDState(LED_BANK_2, false);
             setLEDState(LED_BANK_3, false);
  }
}

void Sucofunkey::setBankUp() {
  if (_activeBank < 3 && _activeBank > 0) {
    _activeBank++;
  }
  setBank(_activeBank);
}

void Sucofunkey::setBankDown() {
  if (_activeBank > 1) {
    _activeBank--;
  }
  setBank(_activeBank);  
}

byte Sucofunkey::getBank() {
  return _activeBank;
};



void Sucofunkey::setInput(byte nr) {
  _activeInput = nr;
    
  switch (nr) {
    case INPUT_NONE:  
              setLEDState(LED_INPUT_MIC, false);
              setLEDState(LED_INPUT_LINE, false);
              setLEDState(LED_INPUT_RESAMPLE, false);
              break;
    case INPUT_MIC:   
              setLEDState(LED_INPUT_MIC, true);
              setLEDState(LED_INPUT_LINE, false);
              setLEDState(LED_INPUT_RESAMPLE, false);
              break;
    case INPUT_LINE:  
              setLEDState(LED_INPUT_MIC, false);
              setLEDState(LED_INPUT_LINE, true);
              setLEDState(LED_INPUT_RESAMPLE, false);
              break;
    case INPUT_RESAMPLE:  
              setLEDState(LED_INPUT_MIC, false);
              setLEDState(LED_INPUT_LINE, false);
              setLEDState(LED_INPUT_RESAMPLE, true);
              break;
    case INPUT_MIC_RESAMPLE:  
              setLEDState(LED_INPUT_MIC, true);
              setLEDState(LED_INPUT_LINE, false);
              setLEDState(LED_INPUT_RESAMPLE, true);
              break;
    case INPUT_LINE_RESAMPLE:  
              setLEDState(LED_INPUT_MIC, false);
              setLEDState(LED_INPUT_LINE, true);
              setLEDState(LED_INPUT_RESAMPLE, true);
              break;
    default:  setLEDState(LED_INPUT_MIC, false);
              setLEDState(LED_INPUT_LINE, false);
              setLEDState(LED_INPUT_RESAMPLE, false);
              break;
  }
};

void Sucofunkey::toggleInput() {
  if (_activeInput < 5) {
    _activeInput++;
  } else {
    _activeInput = 0;
  }
  setInput(_activeInput);
};

byte Sucofunkey::getInput() {
  return _activeInput;
};

// returns the value from an analog input (fader) and scales it to the defined min max range
// e.g. range is from 100 to 200, input (0..1023) is 512 --> 150
int Sucofunkey::getContinuousFaderValue(int scaleMin, int scaleMax) {
  // ignore small steps, as potentiometers tend to jump a bit.
  if (abs(_faderReading - _lastFaderReading) > 3) {
    _lastFaderReading = _faderReading;
  }
  
  // the "jumping point seems fo be at the middle -> 1024/2 = 512 -> difference seems to be about 75 -> e.g. 486 --> 561
  int retVal = static_cast<int>(floor(scaleMin+(((scaleMax-scaleMin)/948.0)*(_lastFaderReading > 512 ? _lastFaderReading-75 : _lastFaderReading))))-1;
  return retVal >= 0 ? retVal : 0;
};

// will be called from the main loop, adds the current reading to the readings queue and calculates the average value of the queue
void Sucofunkey::updateContinuousFaderValue() {
  _lastFaderReadings[_faderReadingPosition] = analogRead(faderPin);
  _faderReading = 0;

  for (int i=0; i<5; i++) {
    _faderReading = _faderReading + _lastFaderReadings[i];
  }

  _faderReading = static_cast<int>(floor(_faderReading/5.0));

  if (_faderReadingPosition >= 5) {
    _faderReadingPosition = 0;
  } else {
    _faderReadingPosition++;
  }

  // ToDo: build a settings option to find the weak spot of a fader and save the configuration

/*  if (_tempInt-_faderReading > 50) {
    Serial.print(_faderReading);
    Serial.print(";");
    Serial.print(_tempInt);
    Serial.print(";");
    Serial.println(abs(_tempInt-_faderReading));
  }
*/
}


int Sucofunkey::getFaderValue(int scaleMin, int scaleMax) {
  // subtract one, as the left side of the fader very often is not 0, but a little up
  int retVal = static_cast<int>(floor(scaleMin+(((scaleMax-scaleMin)/948.0)*(_faderReading > 512 ? _faderReading-75 : _faderReading))))-1;
  return retVal >= 0 ? retVal : 0;
};



void Sucofunkey::switchFaderLED(bool on) {
  digitalWrite(faderLEDPin, on ? HIGH : LOW);
  Serial.print("faderLED::");
  Serial.println(on);
}


void Sucofunkey::switchLEDsOff() {
    for (int i=0; i<32; i++) {
      setLEDState(i, false);
    }
}

byte Sucofunkey::getSampleIdByEventKey(byte eventKey) {
  if (eventKey > 100) {
    return _noteKeysLookUpTable[_eventKeyToLookUpTable[eventKey-100]].sampleId;
  }
  return _noteKeysLookUpTable[_eventKeyToLookUpTable[eventKey]].sampleId;
};

byte Sucofunkey::getLEDPinBySampleId(byte sampleId1) {
  return _noteKeysLookUpTable[sampleId1-1].LED_PIN;
}

byte Sucofunkey::getLEDPinByEventKey(byte eventKey) {
  return _noteKeysLookUpTable[_eventKeyToLookUpTable[eventKey]].LED_PIN;
}

char Sucofunkey::getCharByEventKey(byte eventKey, byte index) {
  if (eventKey > 100) {
    return _noteKeysLookUpTable[_eventKeyToLookUpTable[eventKey-100]].character[index];
  }
  return _noteKeysLookUpTable[_eventKeyToLookUpTable[eventKey]].character[index];
};

char Sucofunkey::getFilenameCharByEventKey(byte eventKey, byte index) {
  if (eventKey > 100) {
    return _noteKeysLookUpTable[_eventKeyToLookUpTable[eventKey-100]].filenameCharacter[index];
  }  return _noteKeysLookUpTable[_eventKeyToLookUpTable[eventKey]].filenameCharacter[index];

};

// a black key from the piano roll.. false does not mean that it is a white key.. might be anything else
boolean Sucofunkey::isEventBlackKey(byte eventKey) {
  if (eventKey < 22 || eventKey > 78) {
    return false;
  } else {
    if (eventKey == FS_1 || eventKey == GS_1 || eventKey == AS_1 || eventKey == CS_1 || eventKey == DS_1 || eventKey == FS_2 || eventKey == GS_2 || eventKey == AS_2 || eventKey == CS_2 || eventKey == DS_2) {
      return true;
    }
  }
  return false;
};


// reads one bit from an 16 Bit unsigned integer and returns its boolean value
bool Sucofunkey::getBooleanValueFrom16BitInt(uint16_t values, int pos) {
    uint16_t v;
    v = (values & ( 1 << pos )) >> pos;
    return v == 1 ? true : false;
}