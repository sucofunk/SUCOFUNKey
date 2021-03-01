#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Home.h"

Home::Home(Sucofunkey *keyboard, Screen *screen, char *activeSongPath) {
    _keyboard = keyboard;    
    _screen = screen;
    _activeSongPath = activeSongPath;
}

void Home::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT: 
              break;
        case Sucofunkey::CURSOR_RIGHT:
              break;            
      }
    }
}

void Home::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    _screen->testBild("SUCOFUNKey");
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}
