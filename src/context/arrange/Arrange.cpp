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

Arrange::Arrange(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _extmemArray = extmemArray;
    _audioResources = audioResources;    
    _bottomMenu = BottomMenu(_keyboard, _screen);
}


void Arrange::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT:
              _keyboard->setBankDown();
              break;
        case Sucofunkey::CURSOR_RIGHT:
              _keyboard->setBankUp();
              break;
        case Sucofunkey::SET:
              _loop = !_loop;
              break;
      }
    }

    if (event.type == Sucofunkey::KEY_FN_NOTE) {      
      if (event.pressed) {
        byte sampleId0 = _keyboard->getSampleIdByEventKey(event.index)-1;
        
        if (_sfsio->sampleBanksStatus[_keyboard->getBank()-1][sampleId0]) {
          _selectSample(_keyboard->getBank(), sampleId0+1);
        }
      } else {

      }
    }

    if (event.type == Sucofunkey::KEY_NOTE) {      
      byte note = 29+((_keyboard->getBank()-1)*24)+_keyboard->getSampleIdByEventKey(event.index)-1;

      if (event.pressed && _currentInstrumentId != 255) {
          _playNextFreeWavetable(note, true);
        }
        else {                
          _playNextFreeWavetable(note, false);
          _audioResources->playMem.stop();
      }
    }   
}

void Arrange::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Arrange mode");
    _screen->testBild("Arranger");
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}


void Arrange::receiveMidiData(midi::MidiType type, int d1, int d2) {
  switch (type) {
    case  midi::MidiType::NoteOn:
     // d2 = velocity
      _playNextFreeWavetable(d1, true);
      break;
    case  midi::MidiType::NoteOff:
      _playNextFreeWavetable(d1, false);    
      break;
    
    default:
      break;
  }
}


void Arrange::_playNextFreeWavetable(byte note, boolean play) {
  //_sfsio->debugInfos();

  // No sample as instrument selected? -> ingnore..
  if (_currentInstrumentId == 255) return;

  if (play) {    
    // search next free wavetable and play..
    for (int i=0; i<6; i++) {
      if (_polyKeyIDs[i] == 0) {
          _polyKeyIDs[i] = note;

        // play pitched note: bankstart(1:29|2:53|3:77)+sampleId1-1 if in keyboard mode      
        switch(i) {
          case 0:
            _audioResources->playMem1.playPitched(_extmemArray + _sfsio->getExtmemOffset(_currentInstrumentId), 60, note, 0, false);
            break;
          case 1:
            _audioResources->playMem2.playPitched(_extmemArray + _sfsio->getExtmemOffset(_currentInstrumentId), 60, note, 0, false);
            break;
          case 2:
            _audioResources->playMem3.playPitched(_extmemArray + _sfsio->getExtmemOffset(_currentInstrumentId), 60, note, 0, false);
            break;
          case 3:
            _audioResources->playMem4.playPitched(_extmemArray + _sfsio->getExtmemOffset(_currentInstrumentId), 60, note, 0, false);
            break;
          case 4:
            _audioResources->playMem5.playPitched(_extmemArray + _sfsio->getExtmemOffset(_currentInstrumentId), 60, note, 0, false);
            break;
          case 5:
            _audioResources->playMem6.playPitched(_extmemArray + _sfsio->getExtmemOffset(_currentInstrumentId), 60, note, 0, false);
            break;                                                
        }

        return;
      }
    }  
  } else {
    // stop wavetable
    for (int i=0; i<6; i++) {
      if (_polyKeyIDs[i] == note) {
          _polyKeyIDs[i] = 0;

          switch(i) {
            case 0:
              _audioResources->playMem1.stop();
              break;
            case 1:
              _audioResources->playMem2.stop();
              break;
            case 2:
              _audioResources->playMem3.stop();
              break;
            case 3:
              _audioResources->playMem4.stop();
              break;
            case 4:
              _audioResources->playMem5.stop();
              break;
            case 5:
              _audioResources->playMem6.stop();
              break;
          }

        return;
      }
    }  
  }
};


void Arrange::_selectSample(byte bank1, byte sampleId1) {
  _sfsio->addSampleToMemory(bank1, sampleId1, false);  
  _currentInstrumentId = (bank1-1)*24+sampleId1;
  _audioResources->playMem.play(_extmemArray + _sfsio->getExtmemOffset(_currentInstrumentId));
}