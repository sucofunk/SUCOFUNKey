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
#include "../../helper/SampleFSIO.h"
#include "../../gui/Screen.h"
#include "SynthCopy.h"
#include "MIDI.h"

SynthCopy::SynthCopy(Sucofunkey* keyboard, Screen* screen, FSIO* fsio, SampleFSIO* sfsio, Play* play) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _play = play;
    _synthCopyScreen = SynthCopyScreen(keyboard, screen);
}

// returns the current tick speed.. as tempo changes are not handled global
long SynthCopy::receiveTimerTick() {
  //return _releaseMS;  
  return _synthCopyScreen.receiveTimerTick();
}


void SynthCopy::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
        switch(event.index) {
            case Sucofunkey::CURSOR_LEFT:
                _keyboard->setBankDown();                
                break;
            case Sucofunkey::CURSOR_RIGHT:
                _keyboard->setBankUp();
                break;
            case Sucofunkey::CURSOR_UP:
                _channel++;
                break;
            case Sucofunkey::CURSOR_DOWN:
                _channel--;
                break;
            case Sucofunkey::RECORD:
                _startRecording();
                break;
            case Sucofunkey::PAUSE:
                _keyboard->addApplicationEventToQueue(Sucofunkey::SYNTHCOPY_STOP_RECORDING);
                break;
            default: 
                break;
        };

    }

    if (event.type == Sucofunkey::KEY_NOTE) {
        int slot = _keyboard->getSampleIdByEventKey(event.index);
        int note = (_keyboard->getBank()-1)*24 + slot + 28;

          if (event.pressed) {
            _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_ON, note, 100, _channel);
          } else {
            _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_OFF, note, 0, _channel);
          }              
    }

    if (event.type == Sucofunkey::EVENT_APPLICATION) {
        switch(event.index) {
            case Sucofunkey::SYNTHCOPY_STOP_NOTE:
                _stopNote();
                break;
            case Sucofunkey::SYNTHCOPY_NEXT_NOTE_REQUEST:
                _nextNote();
                break;
            case Sucofunkey::SYNTHCOPY_NOTE_MARKER:
                Serial.print("Marker received: ");
                Serial.print(_currentNote);
                Serial.print(" :: ");
                Serial.println(event.value);

                break;
            default:
                break;
        }
    }    

}

void SynthCopy::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(2);
    
    _synthCopyScreen.show();
    _keyboard->addApplicationEventToQueue(Sucofunkey::ROUTE_LINE_IN_THROUGH);

  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}

void SynthCopy::_startRecording() {
    _currentNote = _startNote-1;
    _keyboard->addApplicationEventToQueue(Sucofunkey::SYNTHCOPY_START_RECORDING);
}


void SynthCopy::_nextNote() { 
    _currentNote++;

    Serial.print("increasing to:: ");
    Serial.println(_currentNote);
    
    if (_currentNote < _endNote) {        
        _keyboard->addApplicationEventWithValueDataToQueue(Sucofunkey::SYNTHCOPY_START_NOTE, _releaseMS, _currentNote, 100, _channel);        
    } else {
        _keyboard->addApplicationEventWithValueDataToQueue(Sucofunkey::SYNTHCOPY_START_NOTE, _releaseMS, _currentNote, 100, _channel);
        _keyboard->addApplicationEventToQueue(Sucofunkey::SYNTHCOPY_STOP_RECORDING);
    }
}

void SynthCopy::_stopNote() {
    _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_OFF, _currentNote, 0, _channel);
    Serial.println("_stopNote");
}
