#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Live.h"

Live::Live(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _extmemArray = extmemArray;
    _audioResources = audioResources;    
    _bottomMenu = BottomMenu(_keyboard, _screen);
}


void Live::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT:
              break;
        case Sucofunkey::CURSOR_RIGHT:
              break;
        case Sucofunkey::SET:
              break;
      }
    }

}

void Live::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Live mode");
    _screen->testBild("Live mode");
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}