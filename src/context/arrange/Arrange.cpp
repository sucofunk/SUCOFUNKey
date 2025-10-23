/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2025 by Marc Berendes (marc @ sucofunk.com)
    
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
            moveCursor(LEFT);
            break;
      case Sucofunkey::CURSOR_RIGHT:
            moveCursor(RIGHT);
            break;
      case Sucofunkey::CURSOR_UP:
            moveCursor(UP);
            break;
      case Sucofunkey::CURSOR_DOWN:
            moveCursor(DOWN);
            break;

      case Sucofunkey::FN_CURSOR_LEFT:
            _keyboard->setBankDown();
            break;
      case Sucofunkey::FN_CURSOR_RIGHT:
            _keyboard->setBankUp();
            break;

      case Sucofunkey::FN_CURSOR_UP:
            if (_play->getSong()->arrangementGetSheetForPosition(_cursorPosition) != -1) {
              _arrangeScreen.drawCursor(_cursorPosition, true);
              _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition) , _play->getSong()->arrangementChangePositionRepeat(_cursorPosition, true), true);
              saveToSD();
            }            
            break;

      case Sucofunkey::FN_CURSOR_DOWN:            
            if (_play->getSong()->arrangementGetSheetForPosition(_cursorPosition) != -1) {
              _arrangeScreen.drawCursor(_cursorPosition, true);
              _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition) , _play->getSong()->arrangementChangePositionRepeat(_cursorPosition, false), true);
              saveToSD();
            }
            break;

      case Sucofunkey::FN_PLAY:      
            _playbackTickSpeed = _play->calculatePlaybackTickSpeed();            
            if (_play->queueArrangement(0, true)) _blinkPosition = 0;          

            // send MIDI play
            _keyboard->addApplicationEventWithValueDataToQueue(Sucofunkey::MIDI_SEND_START, _play->bpmToMicroseconds(_play->getSong()->getPlayBackSpeed(), 24), 0, 0, 0);  
            break;
      case Sucofunkey::PLAY:
            _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
            if (_play->queueArrangement(_cursorPosition, false)) _blinkPosition = 0;

            // send MIDI play
            _keyboard->addApplicationEventWithValueDataToQueue(Sucofunkey::MIDI_SEND_START, _play->bpmToMicroseconds(_play->getSong()->getPlayBackSpeed(), 24), 0, 0, 0);  
            break;
      case Sucofunkey::PAUSE:
            if (_play->isArrangementPlaying()) {
              _play->stopArrangement();
              _keyboard->addApplicationEventToQueue(Sucofunkey::MIDI_SEND_STOP);
            } else {
              _arrangeScreen.drawCursor(_cursorPosition, false);
              _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition) , _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), false);            
              _cursorPosition = 0;
              _arrangeScreen.drawCursor(_cursorPosition, true);
              _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition) , _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), true);
            }            

            break;
      case Sucofunkey::FN_SET:
            if(_play->getSong()->arrangementRemovePosition(_cursorPosition)) {
              _arrangeScreen.drawCursor(_cursorPosition, true);
              //_arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition), _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), true);
              saveToSD();
            }
            break;

      case Sucofunkey::ARRANGEMENT_PLAY_INDICATOR_CELL:
            _arrangeScreen.drawCursor(_cursorPosition, false);
            _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition) , _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), false);
            _cursorPosition = event.data1;
            _arrangeScreen.drawCursor(_cursorPosition, true);
            _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition) , _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), true);
            break;
    }
  }

  if (event.type == Sucofunkey::KEY_NOTE && event.pressed && _keyboard->isEventWhiteKey(event.index)) {
    sheet = _keyboard->getWhiteKeyByEventKey(event.index) + ((_keyboard->getBank()-1)*14);
    if (_play->getSong()->arrangementInsertSheetAtPosition(_cursorPosition, sheet)) {
      _arrangeScreen.annotateCell(_cursorPosition, sheet, _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), true);
      saveToSD();
    }    
  }

  if (event.type == Sucofunkey::KEY_NOTE && event.pressed && _keyboard->isEventBlackKey(event.index)) {
    
    byte solo = _keyboard->getBlackKey1To10FromEventKey(event.index);
    if (solo > 8) return; // there are just 8 channels..

    if (_soloChannel1 == solo) {
      _play->unMuteAllChannels();
      _soloChannel1 = 0;
      _arrangeScreen.showSoloChannelMessage(0);
    } else {
      for (int c=0; c<8; c++) {
        if (solo-1 != c) {
          _play->muteChannel(c);
        } else {
          _play->unMuteChannel(c);
          _arrangeScreen.showSoloChannelMessage(solo);
        }        
      }
      _soloChannel1 = solo;
    }
  }
}


void Arrange::moveCursor(CursorDirection direction) {
  // delete old cursor
  _arrangeScreen.drawCursor(_cursorPosition, false);

  // redraw text from old cursor
  if (_play->getSong()->arrangementGetSheetForPosition(_cursorPosition) != -1) {
    _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition), _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), false);
  }

  switch(direction) {    
    case UP:
        _cursorPosition = _cursorPosition-_arrangeScreen.columns;
        if (_cursorPosition < 0) _cursorPosition = _cursorPosition+_arrangeScreen.columns;
      break;
    case RIGHT:
        _cursorPosition++;
        if (_cursorPosition > (_arrangeScreen.rows*_arrangeScreen.columns)-1) _cursorPosition = (_arrangeScreen.rows*_arrangeScreen.columns)-1;   
      break;
    case DOWN:
        _cursorPosition = _cursorPosition+_arrangeScreen.columns;
        if (_cursorPosition > (_arrangeScreen.rows*_arrangeScreen.columns)-1) _cursorPosition = _cursorPosition-_arrangeScreen.columns;   
      break;
    case LEFT:
        _cursorPosition--;
        if (_cursorPosition < 0) _cursorPosition = 0;
      break;
    default:
      break;    
  }

  _arrangeScreen.drawCursor(_cursorPosition, true);

  if (_play->getSong()->arrangementGetSheetForPosition(_cursorPosition) != -1) {
    _arrangeScreen.annotateCell(_cursorPosition, _play->getSong()->arrangementGetSheetForPosition(_cursorPosition), _play->getSong()->arrangementGetRepeatForPosition(_cursorPosition), true);
  }

};


void Arrange::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);   
    
    // might have changed in sequencer
    _play->checkIfAllSamplesAreLoaded();
    _isInitialized = true;
    _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
    _arrangeScreen.showEmptyOverview();
    _arrangeScreen.drawCursor(_cursorPosition, true);

    for (int i=0; i<_arrangeScreen.rows*_arrangeScreen.columns; i++) {
      if (_play->getSong()->arrangementGetSheetForPosition(i) != -1) {
        _arrangeScreen.annotateCell(i, _play->getSong()->arrangementGetSheetForPosition(i), _play->getSong()->arrangementGetRepeatForPosition(i), i == _cursorPosition ? true : false);
      }
    }

  } else {
    _isActive = false;
    _keyboard->setBank(0);
    _isInitialized = false;

    if (_play->isArrangementPlaying()) {
      _play->stopArrangement();
      _keyboard->addApplicationEventToQueue(Sucofunkey::MIDI_SEND_STOP);
    }    
    _play->unMuteAllChannels();
  }
}

void Arrange::saveToSD() {
  _play->getSong()->saveMetadataToSD(_sfsio->getSongPath());
};

