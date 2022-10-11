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
}

// returns the current tick speed.. as tempo changes are not handled global
long Arrange::receiveTimerTick() {
  if (_isInitialized) {
    _play->snippetsPlayNext();
  }
  
  _playLEDon = !_playLEDon;  
  //_keyboard->setLEDState(Sucofunkey::LED_PLAY, _playLEDon);
  return _playbackTickSpeed;
}


void Arrange::handleEvent(Sucofunkey::keyQueueStruct event) {
  int wk;

  if (event.pressed) {
    switch(event.index) {
      case Sucofunkey::CURSOR_LEFT:
            _keyboard->setBankDown();
            break;
      case Sucofunkey::CURSOR_RIGHT:
            _keyboard->setBankUp();
            break;
      case Sucofunkey::PLAY:
            _play->snippetsPlayNext();
            break;
    }
  }

  if (event.type == Sucofunkey::KEY_NOTE && event.pressed && _keyboard->isEventWhiteKey(event.index)) {
    wk = _keyboard->getWhiteKeyByEventKey(event.index);

    if (_LEDHighlightSlots[wk-1] == true) {
      _LEDHighlightSlots[wk-1] = false;
      _keyboard->setLEDState(_keyboard->getLEDPinByWhiteKey(wk), false);
      _play->unqueueSnippet(wk);
    } else {
      _play->queueSnippet(wk, true, false);
    }
  }

  if (event.type == Sucofunkey::KEY_FN_NOTE && event.pressed && _keyboard->isEventWhiteKey(event.index-100)) {
    wk = _keyboard->getWhiteKeyByEventKey(event.index-100);

    if (_LEDHighlightSlots[wk-1] == false) {
      _play->queueSnippet(wk, false, true);
      _LEDHighlightSlots[wk-1] = true;
      _keyboard->setLEDState(_keyboard->getLEDPinByWhiteKey(wk), true);
    } else {
      _LEDHighlightSlots[wk-1] = false;
      _keyboard->setLEDState(_keyboard->getLEDPinByWhiteKey(wk), false);
      _play->removeLoopFlagFromSnippet(wk);
    }
  }    

}

void Arrange::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Arrange mode");
    _screen->testBild("Arranger");    
    // might have changed in sequencer
    _play->checkIfAllSamplesAreLoaded();
    _isInitialized = true;
    _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
  } else {
    _isActive = false;
    _keyboard->setBank(0);
    _isInitialized = false;
  }
}


