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
#include "Synth.h"

Synth::Synth(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _extmemArray = extmemArray;
    _audioResources = audioResources;    
    _bottomMenu = BottomMenu(_keyboard, _screen);
}


void Synth::handleEvent(Sucofunkey::keyQueueStruct event) {

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
        Serial.println("start");

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
      }
    }

    if (event.type == Sucofunkey::ENCODER) {
      switch(event.index) {
        case Sucofunkey::ENCODER_1: 
          if (event.pressed) { _val1++; } else { _val1--; }
            Serial.print("_val1::");
            Serial.println(_val1);
            break;
        case Sucofunkey::ENCODER_2: 
          if (event.pressed) { _val2++; } else { _val2--; }
          Serial.print("_val2::");
          Serial.println(_val2);
          break;
        case Sucofunkey::ENCODER_3: 
          if (event.pressed) { _val3++; } else { _val3--; }
          Serial.print("_val3::");
          Serial.println(_val3);
          break;
        case Sucofunkey::ENCODER_4:
          if (event.pressed) { _val4++; } else { _val4--; }
          Serial.print("_val4::");
          Serial.println(_val4);
          break;
        case Sucofunkey::FN_ENCODER_1: 
          if (event.pressed) { _val5++; } else { _val5--; }
            Serial.print("_val5::");
            Serial.println(_val5);
            break;
        case Sucofunkey::FN_ENCODER_2: 
          if (event.pressed) { _val6++; } else { _val6--; }
          Serial.print("_val6::");
          Serial.println(_val6);
          break;
        case Sucofunkey::FN_ENCODER_3: 
          if (event.pressed) { _val7++; } else { _val7--; }
          Serial.print("_val7::");
          Serial.println(_val7);
          break;
        case Sucofunkey::FN_ENCODER_4:
          if (event.pressed) { _val8++; } else { _val8--; }
          Serial.print("_val8::");
          Serial.println(_val8);
          break;
      }
    }
}

void Synth::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    Serial.println("Synth mode");
    _screen->testBild("Synth mode");
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}


void Synth::receiveMidiData(midi::MidiType type, int d1, int d2) {
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


void Synth::_playNextFreeWavetable(byte note, boolean play) {

  // No sample as instrument selected? -> ingnore..
  if (_currentInstrumentId == 255) return;

  if (play) {    
    // search next free wavetable and play..
    for (int i=0; i<6; i++) {
      if (_polyKeyIDs[i] == 0) {
          _polyKeyIDs[i] = note;

        // play pitched note: bankstart(1:29|2:53|3:77)+sampleId1-1 if in keyboard mode
        //_sfsio->changeInstrumentParameters(_currentInstrumentId, _loop, _val1, _val2, _val3, _val4, _val5, _val6, _val7, _val8);
        
        switch(i) {
          case 0:
            _audioResources->wavetableSynth1.setInstrument(_sfsio->getInstrumentDataBySample(_currentInstrumentId));
            _audioResources->wavetableSynth1.playNote(note);
            break;
          case 1:
            _audioResources->wavetableSynth2.setInstrument(_sfsio->getInstrumentDataBySample(_currentInstrumentId));
            _audioResources->wavetableSynth2.playNote(note);
            break;
          case 2:
            _audioResources->wavetableSynth3.setInstrument(_sfsio->getInstrumentDataBySample(_currentInstrumentId));
            _audioResources->wavetableSynth3.playNote(note);
            break;
          case 3:
            _audioResources->wavetableSynth4.setInstrument(_sfsio->getInstrumentDataBySample(_currentInstrumentId));
            _audioResources->wavetableSynth4.playNote(note);
            break;
          case 4:
            _audioResources->wavetableSynth5.setInstrument(_sfsio->getInstrumentDataBySample(_currentInstrumentId));
            _audioResources->wavetableSynth5.playNote(note);
            break;
          case 5:
            _audioResources->wavetableSynth6.setInstrument(_sfsio->getInstrumentDataBySample(_currentInstrumentId));
            _audioResources->wavetableSynth6.playNote(note);
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
              _audioResources->wavetableSynth1.stop();            
              break;
            case 1:
              _audioResources->wavetableSynth2.stop();            
              break;
            case 2:
              _audioResources->wavetableSynth3.stop();            
              break;
            case 3:
              _audioResources->wavetableSynth4.stop();            
              break;
            case 4:
              _audioResources->wavetableSynth5.stop();            
              break;
            case 5:
              _audioResources->wavetableSynth6.stop();            
              break;
          }

        return;
      }
    }  
  }
};


void Synth::_selectSample(byte bank1, byte sampleId1) {
  _sfsio->addSampleToMemory(bank1, sampleId1, false);  
  _currentInstrumentId = (bank1-1)*24+sampleId1;
  _sfsio->generateInstrument(_currentInstrumentId, 60);

  // play for pre-listening
  _audioResources->wavetableSynth1.setInstrument(_sfsio->getInstrumentDataBySample(_currentInstrumentId));
  _audioResources->wavetableSynth1.amplitude(0.3);
  _audioResources->wavetableSynth1.playNote(60);
}