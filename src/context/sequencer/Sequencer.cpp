#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Sequencer.h"

Sequencer::Sequencer(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;    
    _screen = screen;
}

void Sequencer::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT: 
              break;
        case Sucofunkey::CURSOR_RIGHT:
              break;            
      }
    }
}

void Sequencer::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Sequencer");
    _screen->testBild("Sequencer");
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}
