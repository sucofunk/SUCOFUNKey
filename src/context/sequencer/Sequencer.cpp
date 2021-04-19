#include <Arduino.h>
#include <Audio.h>
#include <TeensyThreads.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Sequencer.h"
#include "Pattern.h"
#include "../sampler/Sampler.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"

Sequencer::Sequencer(Sucofunkey *keyboard, Screen *screen, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources) {
    _keyboard = keyboard;
    _screen = screen;    
    _sfsio = sfsio;
    _extmemArray = extmemArray;
    _audioResources = audioResources;
    _sequencerScreen = SequencerScreen(_keyboard, _screen, _sfsio, _audioResources);
}

// returns the current tick speed.. as tempo changes are not handled global
long Sequencer::receiveTimerTick() {
  if (_isPlaying) {
    _playNext();    
    _playLEDon = !_playLEDon;
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, _playLEDon);    
  }
  return _playbackTickSpeed;
}

void Sequencer::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT: 
              moveColumn(-1);
              break;
        case Sucofunkey::FN_CURSOR_LEFT:
              _keyboard->setBankDown();
              _activeBank = _keyboard->getBank();                          
              break;
        case Sucofunkey::CURSOR_RIGHT:
              moveColumn(1);
              break;
        case Sucofunkey::FN_CURSOR_RIGHT:
              _keyboard->setBankUp();
              _activeBank = _keyboard->getBank();                          
              break;
        case Sucofunkey::CURSOR_UP:
              if (!_isPlaying) moveRow(-1);
              break;
        case Sucofunkey::CURSOR_DOWN:
              if (!_isPlaying) moveRow(1);
              break;
        case Sucofunkey::PLAY:
              playPattern();
              break;
        case Sucofunkey::PAUSE:
                pausePattern();
                _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
              break;
        case Sucofunkey::FN_PLAY:
                Serial.println("Loading Samples to Extmem");
                _sfsio->loadSamplesToMemory(_pattern.getSamplesUsed());
                Serial.println("done");
                break;   
        case Sucofunkey::FN_SET:
                _pattern.unsetSampleAt(_cursorColumn, _cursorRow);
                moveRow(1);
                _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery);
                break;                                                                       
      }    
    }


    if (event.type == Sucofunkey::KEY_NOTE && event.pressed) {
      byte sampleId1 = _keyboard->getSampleIdByEventKey(event.index);  
      loadSetPlay(_keyboard->getBank(), sampleId1, _cursorColumn, _cursorRow);
    }

    if (event.type == Sucofunkey::KEY_FN_NOTE) {      
      if (event.pressed) {
        byte sampleId0 = _keyboard->getSampleIdByEventKey(event.index)-1;  
        if (_sfsio->sampleBanksStatus[_keyboard->getBank()-1][sampleId0]) {
          _audioResources->playSdRaw.play(_sfsio->sampleFilename[_keyboard->getBank()-1][sampleId0]);
        }
      } else {
        _audioResources->playSdRaw.stop();
      }
    }

    if (event.type == Sucofunkey::ENCODER) {
      switch (event.index) {
        case Sucofunkey::ENCODER_1:
            if (_isPlaying) {
              _bpm = _bpm + (event.pressed ? 0.5 : -0.5);
              _playbackTickSpeed = bpmToMicroseconds(_bpm, 4);
              _sequencerScreen.drawBPM(_bpm);
            } else {
              moveRow((event.pressed ? 1 : -1));
            }
          break;
        
        case Sucofunkey::ENCODER_2:
            if (_isPlaying) {
              // what will be the function?
            } else {
              moveColumn((event.pressed ? 1 : -1));
            }
          break;

        case Sucofunkey::ENCODER_3:
            if (event.pressed) {
              _pattern.increaseVelocity(_cursorColumn, _cursorRow);
            } else {
              _pattern.decreaseVelocity(_cursorColumn, _cursorRow);
            }
            _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery);

            Serial.print("velocity::");
            Serial.println(_pattern.getSampleAt(_cursorColumn, _cursorRow).velocity);
          break;

        case Sucofunkey::ENCODER_4:
            if (event.pressed) {
              _pattern.stereoPositionTickRight(_cursorColumn, _cursorRow);
            } else {
              _pattern.stereoPositionTickLeft(_cursorColumn, _cursorRow);
            }
            _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery);

            Serial.print("panorama::");
            Serial.println(_pattern.getSampleAt(_cursorColumn, _cursorRow).stereoPosition);            
          break;
      }
    }

    // turn active sample indicator led off
    if (_activeNoteLEDPin != 0) {
      _keyboard->setLEDState(_activeNoteLEDPin, false);
      _activeNoteLEDPin = 0;
    }

    // turn active sample indicator led on
    if (!_isPlaying) {
        byte sample = _pattern.getSampleAt(_cursorColumn, _cursorRow).sampleNumber;
        if (sample != 255) {
          sample -= 1;
          byte b = sample/24;
          byte s = sample%24;
          _keyboard->setBank(b+1);
          _activeNoteLEDPin = _keyboard->getLEDPinBySampleId(s+1);
          _keyboard->setLEDState(_activeNoteLEDPin, true);
        }        
    }    
}


void Sequencer::loadSetPlay(byte bank1, byte sample1, byte column, int row) {
  if (_sfsio->sampleBanksStatus[bank1-1][sample1-1]) {
    _audioResources->playMem.stop();
    _pattern.setSampleAt(column, row, (bank1-1)*24+sample1, 64, 64);

    // load Sample if not in extmem yet
    if(_sfsio->addSampleToMemory(bank1, sample1, false)) {
      // success.. there was enough memory ;)
      _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
      moveRow(1);
      _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery);
      _audioResources->playMem.play(_extmemArray + _sfsio->getExtmemOffset((bank1-1)*24+sample1));
    } else {
      // failed, because there is not enough memory left
      _pattern.unsetSampleAt(column, row);
      Serial.println("out of memory");
    }   
  }
}

void Sequencer::moveRow(int count) {
    int oldVal = _cursorRow;
    _cursorRow = _cursorRow + count;

    if (_cursorRow < 0) { 
      _cursorRow = 0;      
      return;
    } 
    if (_cursorRow >= _pattern.getPatternLength()) {
      _cursorRow = _pattern.getPatternLength()-1; 
      return;
    }    
    
    if (oldVal != _cursorRow) {
      _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery);
    }
}


void Sequencer::moveColumn(byte count) {  
  _cursorColumn = _cursorColumn + count;

  if (_cursorColumn < 1) _cursorColumn = 1;
  if (_cursorColumn > 4) _cursorColumn = 4;

  _sequencerScreen.drawCursorAtColumn(_cursorColumn);
}


long Sequencer::bpmToMicroseconds(float bpm, int quant) {
  return floor(60000000 / bpm / quant);
}


void Sequencer::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);     
    Serial.println("Sequencer");
    
    _sequencerScreen.initializeGrid(&_pattern);
    _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery);
    _sequencerScreen.drawCursorAtColumn(_cursorColumn);
    _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
 }
 else {
    _isActive = false;
    _keyboard->setBank(0);
    _keyboard->setLEDState(_activeNoteLEDPin, false);
    _activeNoteLEDPin = 0;
  }
}

void Sequencer::playPattern() {
  _isPlaying = !_isPlaying;

  if (!_isPlaying) stopAllChannels();

  if (_isPlaying) {
//   _cursorRow = 0;
   _playedFirstRow = false;
   _sequencerScreen.drawBPM(_bpm);
  }
  
}

void Sequencer::pausePattern() {
  if (_isPlaying) {
    _isPlaying = false;
    stopAllChannels();
  } else {
    _cursorRow = 0;
    _playedFirstRow = false;
    //_sequencerScreen.drawBPM(_bpm);   
    _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery); 
  }
}

void Sequencer::_playNext() {
    if (_playedFirstRow) { _cursorRow++; } else { _playedFirstRow = true; };
    
    if (_isActive) {
      _sequencerScreen.drawTrackerAtPosition(_cursorRow, &_pattern, true, _highlightEvery);
    }

    for (byte c=0; c < _pattern.channels; c++) {
      if (_pattern.getSampleAt(c+1, _cursorRow).sampleNumber < 255 && _sfsio->getExtmemOffset(_pattern.getSampleAt(c+1, _cursorRow).sampleNumber) != -1) {
        switch(c) {
          case 0:
            if (_pattern.getSampleAt(c+1, _cursorRow).sampleNumber == 254) {
              _audioResources->playMem1.stop();
            } else {

              // ToDo: set mixers to velocity of note --> optimize

              if (_pattern.getSampleAt(1, _cursorRow).stereoPosition < 64) {
                _audioResources->mixerMem1L.gain(0, (_pattern.getSampleAt(1, _cursorRow).velocity/127.0)*1.0 );
                _audioResources->mixerMem1R.gain(0, (_pattern.getSampleAt(1, _cursorRow).velocity/127.0)*(_pattern.getSampleAt(1, _cursorRow).stereoPosition/64.0));
              } else {                
                _audioResources->mixerMem1L.gain(0, (_pattern.getSampleAt(1, _cursorRow).velocity/127.0)*((127-_pattern.getSampleAt(1, _cursorRow).stereoPosition)/64.0));
                _audioResources->mixerMem1R.gain(0, (_pattern.getSampleAt(1, _cursorRow).velocity/127.0)*1.0 );
              }
          
              Serial.println(_pattern.getSampleAt(1, _cursorRow).velocity);
              _audioResources->playMem1.play(_extmemArray + _sfsio->getExtmemOffset(_pattern.getSampleAt(c+1, _cursorRow).sampleNumber));
            }
            break;
          case 1:
            if (_pattern.getSampleAt(c+1, _cursorRow).sampleNumber == 254) {
              _audioResources->playMem2.stop();
            } else {
              _audioResources->playMem2.play(_extmemArray + _sfsio->getExtmemOffset(_pattern.getSampleAt(c+1, _cursorRow).sampleNumber));
            }
            break;
          case 2:
            if (_pattern.getSampleAt(c+1, _cursorRow).sampleNumber == 254) {
              _audioResources->playMem3.stop();
            } else {            
              _audioResources->playMem3.play(_extmemArray + _sfsio->getExtmemOffset(_pattern.getSampleAt(c+1, _cursorRow).sampleNumber));
            }  
            break;
          case 3:
            if (_pattern.getSampleAt(c+1, _cursorRow).sampleNumber == 254) {
              _audioResources->playMem4.stop();
            } else {
              _audioResources->playMem4.play(_extmemArray + _sfsio->getExtmemOffset(_pattern.getSampleAt(c+1, _cursorRow).sampleNumber));
            }
            break;
        }
      }
    }

    if (_cursorRow == _pattern.getPatternLength()-1) { 
      _cursorRow = 0;
      _playedFirstRow = false;
    }    
}

void Sequencer::stopAllChannels() {
  _playLEDon = false;
  _audioResources->playMem.stop();
  _audioResources->playMem1.stop();
  _audioResources->playMem2.stop();
  _audioResources->playMem3.stop();
  _audioResources->playMem4.stop();
}

boolean Sequencer::isPlaying() {
  return _isPlaying;
}
