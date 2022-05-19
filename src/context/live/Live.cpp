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


void Live::_playNextFreeMemory(byte sample1, byte velocity, boolean play) {

  // check if sample in extmem.. if not, load it now!
  if (!_sfsio->addSampleToMemory((sample1/24)+1, (sample1%24), false)) return;

  // sample still playing?
  if (!_audioResources->playMemLive1.isPlaying()) { _polyMemIDs[0] = 0; }
  if (!_audioResources->playMemLive2.isPlaying()) { _polyMemIDs[1] = 0; }  
  if (!_audioResources->playMemLive3.isPlaying()) { _polyMemIDs[2] = 0; }
  if (!_audioResources->playMemLive4.isPlaying()) { _polyMemIDs[3] = 0; }
  if (!_audioResources->playMemLive5.isPlaying()) { _polyMemIDs[4] = 0; }    
  if (!_audioResources->playMemLive6.isPlaying()) { _polyMemIDs[5] = 0; }
  if (!_audioResources->playMemLive7.isPlaying()) { _polyMemIDs[6] = 0; }
  if (!_audioResources->playMemLive8.isPlaying()) { _polyMemIDs[7] = 0; }

  if (play) {    
    // search next free playmem and play..
    for (int i=0; i<8; i++) {

      if (_polyMemIDs[i] == 0) {
          _polyMemIDs[i] = sample1;
        
        switch(i) {
          case 0:
            _polyChangeVelocity(i, velocity);
            _audioResources->playMemLive1.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;
          case 1:
            _polyChangeVelocity(i, velocity);          
            _audioResources->playMemLive2.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;
          case 2:
            _polyChangeVelocity(i, velocity);          
            _audioResources->playMemLive3.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;
          case 3:
            _polyChangeVelocity(i, velocity);          
            _audioResources->playMemLive4.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;
          case 4:
            _polyChangeVelocity(i, velocity);          
            _audioResources->playMemLive5.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;
          case 5:
            _polyChangeVelocity(i, velocity);          
            _audioResources->playMemLive6.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;
          case 6:
            _polyChangeVelocity(i, velocity);          
            _audioResources->playMemLive7.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;
          case 7:
            _polyChangeVelocity(i, velocity);          
            _audioResources->playMemLive8.play(_extmemArray + _sfsio->getExtmemOffset(sample1));
            break;                                                                        
        }

        return;
      }
    }  
  } else {
    // stop wavetable
    for (int i=0; i<6; i++) {
      if (_polyMemIDs[i] == sample1) {
          _polyMemIDs[i] = 0;

          switch(i) {
            case 0:
              _audioResources->playMemLive1.stop();
              break;
            case 1:
              _audioResources->playMemLive2.stop();
              break;
            case 2:
              _audioResources->playMemLive3.stop();
              break;
            case 3:
              _audioResources->playMemLive4.stop();
              break;
            case 4:
              _audioResources->playMemLive5.stop();
              break;
            case 5:
              _audioResources->playMemLive6.stop();
              break;
            case 6:
              _audioResources->playMemLive7.stop();
              break;
            case 7:
              _audioResources->playMemLive8.stop();
              break;
          }

        return;
      }
    }  
  }
}

void Live::_polyChangeVelocity(byte polymem, byte velocity) {

  _tempVelocity = (velocity/127.0)*1.0;

  switch(polymem) {
    case 0:
      _audioResources->mixerMem3L.gain(0, _tempVelocity);
      _audioResources->mixerMem3R.gain(0, _tempVelocity);
      break;
    case 1:
      _audioResources->mixerMem3L.gain(1, _tempVelocity);
      _audioResources->mixerMem3R.gain(1, _tempVelocity);
      break;
    case 2:
      _audioResources->mixerMem3L.gain(2, _tempVelocity);
      _audioResources->mixerMem3R.gain(2, _tempVelocity);
      break;
    case 3:
      _audioResources->mixerMem3L.gain(3, _tempVelocity);
      _audioResources->mixerMem3R.gain(3, _tempVelocity);
      break;
    case 4:
      _audioResources->mixerMem4L.gain(0, _tempVelocity);
      _audioResources->mixerMem4R.gain(0, _tempVelocity);
      break;
    case 5:
      _audioResources->mixerMem4L.gain(1, _tempVelocity);
      _audioResources->mixerMem4R.gain(1, _tempVelocity);
      break;
    case 6:
      _audioResources->mixerMem4L.gain(2, _tempVelocity);
      _audioResources->mixerMem4R.gain(2, _tempVelocity);
      break;
    case 7:
      _audioResources->mixerMem4L.gain(3, _tempVelocity);
      _audioResources->mixerMem4R.gain(3, _tempVelocity);
      break;
  }
}


void Live::_handlePolyphonicAftertouch(byte sample1, byte value) {
/*  Serial.print("Poly Aftertouch::");
  Serial.print(sample1);
  Serial.print("::");
  Serial.println(value);
*/
  for (int i=0; i<8; i++) {
    if (_polyMemIDs[i] == sample1) {
      _polyChangeVelocity(i, value);
      return;
    }
  }
}


void Live::receiveMidiData(midi::MidiType type, int d1, int d2) {
  switch (type) {
    case  midi::MidiType::NoteOn:
     // d2 = velocity
      _playNextFreeMemory(d1, d2, true);
  
/*  Serial.print("NoteOn::");
  Serial.print(d1);
  Serial.print("::");
  Serial.println(d2);
*/
      break;
    case  midi::MidiType::NoteOff:
      _playNextFreeMemory(d1, d2, false);

/*  Serial.print("NoteOff::");
  Serial.print(d1);
  Serial.print("::");
  Serial.println(d2);
*/
      break;

    case  midi::MidiType::AfterTouchPoly:
      _handlePolyphonicAftertouch(d1, d2);
      break;


    default:
      break;
  }
}
