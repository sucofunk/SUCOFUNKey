/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2022 by Marc Berendes (marc @ sucofunk.com)
    
   ----------------------------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.    

   ---------------------------------------------------------------------------------------------- */
   
#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Arrange.h"

Arrange::Arrange(Sucofunkey* keyboard, Screen* screen, FSIO* fsio, SampleFSIO* sfsio, unsigned int* extmemArray, AudioResources* audioResources, Play* play) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _extmemArray = extmemArray;
    _audioResources = audioResources;    
    _play = play;
    _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
    _arrangeScreen = ArrangeScreen(keyboard, screen, sfsio);
}


// returns the current tick speed.. as tempo changes are not handled global
long Arrange::receiveTimerTick() {
  if (_isInitialized && _play->isArrangementPlaying()) {
    _play->arrangementPlayNext();  
  
    if (_blinkPosition == 0) {
      _playLEDon = true;
    } else {
      _playLEDon = false;
    }

    _keyboard->setLEDState(Sucofunkey::LED_PLAY, _playLEDon);

    if (_blinkPosition < (_play->getSong()->getSongResolution()*4)-1) {
      _blinkPosition++;
    } else {
      _blinkPosition = 0;
    }
  } else {
    // stopped.. check if play led is still on..
    if (_playLEDon) {
      _playLEDon = false;      
    }    
  }

  _keyboard->setLEDState(Sucofunkey::LED_PLAY, _playLEDon);
  return _playbackTickSpeed;
}


void Arrange::handleEvent(Sucofunkey::keyQueueStruct event) {
  int sheet;

  if (event.pressed) {
    switch(event.index) {
      case Sucofunkey::CURSOR_LEFT:
            _keyboard->setBankDown();
            break;
      case Sucofunkey::CURSOR_RIGHT:
            _keyboard->setBankUp();
            break;
      case Sucofunkey::FN_PLAY:      
            _playbackTickSpeed = _play->calculatePlaybackTickSpeed();            
            if (_play->queueArrangement(0, true)) _blinkPosition = 0;
            break;
      case Sucofunkey::PLAY:
            _playbackTickSpeed = _play->calculatePlaybackTickSpeed();            
            if (_play->queueArrangement(0, false)) _blinkPosition = 0;
            break;
      case Sucofunkey::PAUSE:
            _play->stopArrangement();
            break;
      case Sucofunkey::FN_SET:
            _play->getSong()->removeLastSheetFromArrangement();
            break;

    }
  }

  if (event.type == Sucofunkey::KEY_NOTE && event.pressed && _keyboard->isEventWhiteKey(event.index)) {
    sheet = _keyboard->getWhiteKeyByEventKey(event.index) + ((_keyboard->getBank()-1)*14);
    _play->getSong()->appendSheetToArrangement(sheet);
    _play->getSong()->debugArrangement();
  }   
}

void Arrange::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Arrange mode");  
    
    // might have changed in sequencer
    _play->checkIfAllSamplesAreLoaded();
    _isInitialized = true;
    _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
    _arrangeScreen.showEmptyOverview();
  } else {
    _isActive = false;
    _keyboard->setBank(0);
    _isInitialized = false;
  }
}


