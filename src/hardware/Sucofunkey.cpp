#include <Arduino.h>
#include <Wire.h>
#include "Sucofunkey.h"
#include "Adafruit_MCP23017.h"
#include <cppQueue.h> // https://github.com/SMFSW/Queue


static volatile bool _keyPressedInterrupt1 = false;
static volatile bool _keyPressedInterrupt2 = false;
static volatile bool _keyPressedInterrupt3 = true;
static volatile bool _encoderInterrupt = false;

cppQueue keyQueue(sizeof(Sucofunkey::Key), 42, FIFO);


Sucofunkey::Sucofunkey(int intKeyPin1, int intEncPin, int intKeyPin2, int intKeyPin3)
 {
    _intKeyPin1 = intKeyPin1;
    _intKeyPin2 = intKeyPin2;
    _intKeyPin3 = intKeyPin3;
    _intEncPin = intEncPin;

    _keyPressedInterrupt1 = false;
    _keyPressedInterrupt2 = false;
    _keyPressedInterrupt3 = true;
    _encoderInterrupt = false;


    // Is the Function Key held for a key combination?
    _fnKeyHold = false;
    _fnKeyInterrupted = false;

    // Is the Menu Key held for a key combination with rotary encoder push buttons or record button?
    _menuKeyHold = false;
        
    // needs to be true to e.g. not change menu context while recording
    _ignoreKeys = false;

    // Init mcp23017 ICs of SucoKey
    _mcp1.begin(0, &Wire2);
    _mcp2.begin(1, &Wire2);
    _mcp3.begin(2, &Wire2);
    _mcp4.begin(3, &Wire2);
    _mcp5.begin(4, &Wire2);

    pinMode(_intKeyPin1, INPUT_PULLUP);
    pinMode(_intKeyPin2, INPUT_PULLUP);
//    pinMode(_intKeyPin3, INPUT_PULLUP);    
//    pinMode(_intEncPin, INPUT_PULLUP);

    _setupInterrupts();    

    // enable first mcp for keys: C#1 D1 D#1 E1 F2 F#2 G2 G#2 F1 F#1 G1 G#1 A1 A#1 B1 C1
    _mcp1.setupInterrupts(true,false,LOW);
    for (int i=0; i<16; i++) {
        _mcp1.pinMode(i, INPUT);
        _mcp1.pullUp (i, HIGH);
        _mcp1.setupInterruptPin(i, CHANGE);
    }

    // enable second mcp for keys: Play Pause Record Input Left Up Down Right A2 A#2 B2 C2 C#2 D2 D#2 E2
    _mcp2.setupInterrupts(true,false,LOW);
    for (int i=0; i<16; i++) {
        _mcp2.pinMode(i, INPUT);
        _mcp2.pullUp (i, HIGH);
        _mcp2.setupInterruptPin(i, CHANGE);
    }

    // enable third mcp for keys: ENC1 ENC2 ... ENC 8 (IRQ on _intEncPin) || Function Menu Set GPB3 GPB4 GPB5 GPB6 GPB7 (IRQ on _intKeyPin3)
    // NOTE: 25.01.2021: Removed IRQ for MCP3, as it crashed the i2c communication after a few seconds, when rotating the encoders.. pith polling it works.
    //_mcp3.setupInterrupts(false,false,LOW);
    for (int i=0; i<16; i++) {
        _mcp3.pinMode(i, INPUT);
        _mcp3.pullUp (i, HIGH);
//        _mcp3.setupInterruptPin(i, CHANGE);
    }

    // set mcp4 and mcp5 to output for LEDs
    for (int i=0; i<16; i++) {
      _mcp4.pinMode(i, OUTPUT);
      _mcp5.pinMode(i, OUTPUT);
    }
}

// -------------------------------------------------------------------------------------------------
// SucoKey Interrupt Handling
// -------------------------------------------------------------------------------------------------


void Sucofunkey::_setupInterrupts() {
   attachInterrupt(digitalPinToInterrupt(_intKeyPin1), _intCallBack1, FALLING);
   attachInterrupt(digitalPinToInterrupt(_intKeyPin2), _intCallBack2, FALLING);
   //attachInterrupt(digitalPinToInterrupt(_intKeyPin3), _intCallBack3, FALLING);
   //attachInterrupt(digitalPinToInterrupt(_intEncPin), _intCallBackEnc,FALLING);    
}


void Sucofunkey::_intCallBack1() {
  _keyPressedInterrupt1=true;
}

void Sucofunkey::_intCallBack2() {
  _keyPressedInterrupt2=true;
}

/*
void Sucofunkey::_intCallBack3() {
  _keyPressedInterrupt3=true;
}

void Sucofunkey::_intCallBackEnc() {
  _encoderInterrupt=true; 
}
*/


void Sucofunkey::_handleKeyPressed() {
  byte eventNumber;

  if (_keyPressedInterrupt1) { // must be a normal NOTE, as there are no other Keys connected to that MCP
    _keyPressedInterrupt1 = false;

    for (byte i=0; i<16; i++) {

      eventNumber = i;
      // if function key is pressed too, add 50 as offset
      if (_fnKeyHold) { 
        eventNumber += 50; 
        _fnKeyInterrupted = true;
      }
      
      // if menu key is pressed too, but function (rated higher) not, add 100 as offset
      if (_menuKeyHold && !_fnKeyHold) { 
        eventNumber += 100; 
        _menuKeyInterrupted = true;
      }

      if (_mcp1.digitalRead(i) == 0) {
        if (_values_mcp1[i] == false) {
          _values_mcp1[i] = true;          
          keyQueueStruct k = {eventNumber, true, _ignoreKeys, (_fnKeyHold ? KEY_FN_NOTE : (_menuKeyHold ? KEY_MENU_NOTE : KEY_NOTE))};
          keyQueue.push(&k);
        }        
      } else {
        if (_values_mcp1[i] == true) {
          _values_mcp1[i] = false;
          keyQueueStruct k = {eventNumber, false, _ignoreKeys, (_fnKeyHold ? KEY_FN_NOTE : (_menuKeyHold ? KEY_MENU_NOTE : KEY_NOTE))};
          keyQueue.push(&k);
        }        
      }
    }
  } 

  if (_keyPressedInterrupt2) { 
    _keyPressedInterrupt2 = false;
    for (byte i=0; i<16; i++) {

      eventNumber = i+16;
      // if function key is pressed too, add 50 as offset
      if (_fnKeyHold) { 
        eventNumber += 50; 
        _fnKeyInterrupted = true;
      }
      
      // if menu key is pressed too, but function (rated higher) not, add 100 as offset
      if (_menuKeyHold && !_fnKeyHold) { 
        eventNumber += 100; 
        _menuKeyInterrupted = true;
      }

      byte keyType = 0;

      if (i < 8) {
        keyType = (_fnKeyHold ? KEY_FN_OPERATION : (_menuKeyHold ? KEY_MENU_SELECTOR : KEY_OPERATION));
      } else {
        keyType = (_fnKeyHold ? KEY_FN_NOTE : (_menuKeyHold ? KEY_MENU_NOTE : KEY_NOTE));
      }

      if (_mcp2.digitalRead(i) == 0) {
        if (_values_mcp2[i] == false) {
          _values_mcp2[i] = true;          
          keyQueueStruct k = {eventNumber, true, _ignoreKeys, keyType};
          keyQueue.push(&k);
        }        
      } else {
        if (_values_mcp2[i] == true) {
          _values_mcp2[i] = false;
          keyQueueStruct k = {eventNumber, false, _ignoreKeys, keyType};
          keyQueue.push(&k);
        }        
      }
    }
  } 


//  if (_keyPressedInterrupt3) {    
    for (byte i=8; i<16; i++) {
      
      eventNumber = i+32;

      byte keyType = 0;

      if (i < 15) {
        keyType = (_fnKeyHold ? KEY_FN_OPERATION : ( _menuKeyHold ? KEY_MENU_SELECTOR : KEY_OPERATION ));
      } else {
        keyType = (_fnKeyHold ? KEY_FN_FREE : ( _menuKeyHold ? KEY_MENU_SELECTOR : KEY_FREE ));
      }

      // key pressed..
      if (_mcp3.digitalRead(i) == 0) {
        if (_values_mcp3[i] == false) {
          _values_mcp3[i] = true;

          // Key combination with Function + X
          if (_fnKeyHold) {
            // offset to const values with FN_
            eventNumber += 50;
            _fnKeyInterrupted = true;
          } 

          // Key combination with Menu + X
          if (_menuKeyHold) {
            // offset to const values with MENU_
            eventNumber += 100;
            _menuKeyInterrupted = true;
          } 

          // handle Function Key
          if (i == 8) {
//            Serial.println("FN 1");
            _fnKeyHold = true;
            _fnKeyInterrupted = false;
            _fnKeyMillis = millis();
          } else {

            if (i == 9) {
              _menuKeyHold = true;
              _menuKeyInterrupted = false;
              _menuKeyMillis = millis();              
            } else {
              // add every other key to queue, but not the function or menu key, as they are handled separate                       
              keyQueueStruct k = {eventNumber, true, _ignoreKeys, keyType};
              keyQueue.push(&k);
            }
          }
          
        }        
      } else { // key released..
                        
        if (_values_mcp3[i] == true) {
          _values_mcp3[i] = false;

          // handle Function Key
          if (i == 8) {
            // Fuction key pressed for at least 3 seconds? send event FN_FUNCTION (go to home menu)
            if (_fnKeyHold && !_fnKeyInterrupted && (millis() - _fnKeyMillis >= 3000)) {
              //_fnKeyMillis = 0;
              keyQueueStruct k = {FN_FUNCTION, true, _ignoreKeys, KEY_OPERATION};
              keyQueue.push(&k);
            } else {
              // Fuction key released and hold time was less than 1 seconds -> FUCNTION (2) event
              if (_fnKeyHold && !_fnKeyInterrupted && (millis() - _fnKeyMillis < 1500)) {
                keyQueueStruct k = {FUNCTION, true, _ignoreKeys, KEY_OPERATION};
                keyQueue.push(&k);
              }
            }
            
//            Serial.println("FN 2");

            _fnKeyHold = false;
            _fnKeyInterrupted = false;
          
          } else {
            // handle menu key
            if (i == 9) {
              if (_menuKeyHold && !_menuKeyInterrupted && (millis() - _menuKeyMillis >= 3000)) {
                keyQueueStruct k = {MENU_MENU, true, _ignoreKeys, KEY_OPERATION};
                keyQueue.push(&k);                
              } else {
                if (_menuKeyHold && !_menuKeyInterrupted && (millis() - _menuKeyMillis < 1500)) {
                  keyQueueStruct k = {MENU, true, _ignoreKeys, KEY_OPERATION};
                  keyQueue.push(&k);                
                }
              } 

              _menuKeyHold = false;
              _menuKeyInterrupted = false;
            } else {
              // handle all other keys on the first mcp
              // add event offset, if function key is pressed too
              if (_fnKeyHold) { 
                eventNumber += 50; 
                } else {
                  if (_menuKeyHold) { eventNumber += 100; };
                }

              keyQueueStruct k = {eventNumber, false, _ignoreKeys, keyType};
              keyQueue.push(&k);
            }
          }                    
        }
      }
    }
//  }

  //_keyPressedInterrupt3 = false; 
}

void Sucofunkey::_handleEncoderUpdate() {
  _readRotaryEncoder(0, _mcp3.digitalRead(0), _mcp3.digitalRead(1), ENCODER_1);
  _readRotaryEncoder(1, _mcp3.digitalRead(2), _mcp3.digitalRead(3), ENCODER_2);
  _readRotaryEncoder(2, _mcp3.digitalRead(4), _mcp3.digitalRead(5), ENCODER_3);
  _readRotaryEncoder(3, _mcp3.digitalRead(6), _mcp3.digitalRead(7), ENCODER_4);
  //_encoderInterrupt = false;
}

int Sucofunkey::_readRotaryEncoder(byte encoderNr, uint8_t valuePinL, uint8_t valuePinR, byte eventId) {
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
  if (_keyPressedInterrupt1 || _keyPressedInterrupt2 || _keyPressedInterrupt3) {
    _handleKeyPressed();
    return true;
  } else {
    return false;
  }
}

// check if an interrupt was fired for rotary encoders
boolean Sucofunkey::hasEncoderChange() {

//  if (_encoderInterrupt) {
    _handleEncoderUpdate();
    return true;
//  } else {
//    return false;
//  }
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

void Sucofunkey::addApplicationEventToQueue(byte eventId) {
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

void Sucofunkey::setLEDState(byte led, bool state) {
  if (state) {
    if (led <= 15) {
      _mcp5.digitalWrite(led, HIGH);  
    } else {
      _mcp4.digitalWrite(led-16, HIGH);  
    }    
  } else {
    if (led <= 15) {
      _mcp5.digitalWrite(led, LOW);  
    } else {
      _mcp4.digitalWrite(led-16, LOW);  
    }  
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
int Sucofunkey::getFaderValue(uint8_t pin, int scaleMin, int scaleMax) {
  int faderIn = analogRead(pin);

  if (abs(faderIn - _lastFaderReading) > 3) {
    _lastFaderReading = faderIn;
  }
  
  return static_cast<int>(floor(scaleMin+(((scaleMax-scaleMin)/1023.0)*_lastFaderReading)));
};



void Sucofunkey::switchLEDsOff() {
    for (int i=0; i<32; i++) {
      setLEDState(i, false);
    }
}

byte Sucofunkey::getSampleIdByEventKey(byte eventKey) {
  if (eventKey > 50) {
    return _eventKeyToSampleId[eventKey-50];
  }
  return _eventKeyToSampleId[eventKey];
};

byte Sucofunkey::getLEDPinBySampleId(byte sampleId1) {
  return _sampleIdToLEDPIN[sampleId1];
}

byte Sucofunkey::getLEDPinByEventKey(byte eventKey) {
  return _sampleIdToLEDPIN[_eventKeyToSampleId[eventKey]];
}

char Sucofunkey::getCharByEventKey(byte eventKey, byte index) {
  if (eventKey > 50) {
    return _eventKeyToChar[eventKey-50][index];
  }
  return _eventKeyToChar[eventKey][index];
};

char Sucofunkey::getFilenameCharByEventKey(byte eventKey, byte index) {
  if (eventKey > 50) {
    return _eventKeyToCharFilename[eventKey-50][index];
  }
  return _eventKeyToCharFilename[eventKey][index];
};
