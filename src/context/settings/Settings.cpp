#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Settings.h"

Settings::Settings(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;    
    _screen = screen;
}

void Settings::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT: 
              break;
        case Sucofunkey::CURSOR_RIGHT:
              break;            
      }
    }
}

void Settings::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Settings mode");
    _screen->testBild("Settings mode");
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}
