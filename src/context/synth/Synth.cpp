#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Synth.h"

Synth::Synth(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;    
    _screen = screen;
}

void Synth::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT: 
              break;
        case Sucofunkey::CURSOR_RIGHT:
              break;            
      }
    }
}

void Synth::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Synthesizer");
    _screen->testBild("Synthesizer");
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}
