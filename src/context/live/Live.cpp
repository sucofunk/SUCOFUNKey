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
        case Sucofunkey::PLAY:
              _test();
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


//getExtmemOffset((bank1-1)*24+sample1)

void Live::_test() {
  _sfsio->addSampleToMemory(1, 1, false);

  _sfsio->generateInstrument(1, 64);

  _audioResources->wavetable1.setInstrument(_sfsio->getInstrumentDataBySample(1));
  _audioResources->wavetable1.amplitude(0.3);
  int note = _keyboard->getFaderValue(_keyboard->faderPin, 0, 127);
  Serial.print("note::");
  Serial.println(note);
  _audioResources->wavetable1.playNote(note);


//  _audioResources->playMem.play(_extmemArray + _sfsio->getExtmemOffset(1));



}
