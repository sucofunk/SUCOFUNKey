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
    if (_blinkPosition == 0) {
      _playLEDon = true;
    } else {
      _playLEDon = false;
    }

    _keyboard->setLEDState(Sucofunkey::LED_PLAY, _playLEDon);
  }

  if (_blinkPosition < _pattern.getPatternResolution()-1) {
    _blinkPosition++;
  } else {
    _blinkPosition = 0;
  }

  return _playbackTickSpeed;
}


void Sequencer::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
      switch(event.index) {
        // move cursor
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
        // change sample bank
        case Sucofunkey::FN_CURSOR_LEFT:
              _keyboard->setBankDown();
              _activeBank = _keyboard->getBank();                          
              break;
        case Sucofunkey::FN_CURSOR_RIGHT:
              _keyboard->setBankUp();
              _activeBank = _keyboard->getBank();                          
              break;
        // change sequence resolution
        case Sucofunkey::FN_CURSOR_UP:
              if (!_isPlaying) {
                _pattern.changePatternResolutionByTick(true);
                _calculatePlaybackTickSpeed();
                _sequencerScreen.drawGrid(_sequencerScreen.RESOLUTION);              
              }
              break;
        case Sucofunkey::FN_CURSOR_DOWN:
              if (!_isPlaying) {
                _pattern.changePatternResolutionByTick(false);
                _calculatePlaybackTickSpeed();              
                _sequencerScreen.drawGrid(_sequencerScreen.RESOLUTION);              
              }
              break;
        // shift samples when changing resolution
        case Sucofunkey::MENU_CURSOR_UP:
              if (!_isPlaying) {
                _pattern.shiftResolution(true);
                _calculatePlaybackTickSpeed();              
                _sequencerScreen.drawGrid(_sequencerScreen.SCALE);              
              }              
              break;
        case Sucofunkey::MENU_CURSOR_DOWN:
              if (!_isPlaying) {
                _pattern.shiftResolution(false);
                _calculatePlaybackTickSpeed();              
                _sequencerScreen.drawGrid(_sequencerScreen.SCALE);             
              }
              break;


        case Sucofunkey::PLAY:
              _calculatePlaybackTickSpeed();
              playPattern();
              break;
        case Sucofunkey::PAUSE:
              pausePattern();
              break;
        case Sucofunkey::FN_PLAY:
                if (!_isPlaying) {
                  Serial.println("Loading Samples to Extmem");
                  _sfsio->loadSamplesToMemory(_pattern.getSamplesUsed());
                  Serial.println("done");
                }
                break;   
        case Sucofunkey::FN_SET:
                _pattern.unsetSampleAt(_cursorChannel, _cursorPosition);
                _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
                break;                                                                       
      }    
    }


    if (event.type == Sucofunkey::KEY_NOTE && event.pressed) {
      byte sampleId1 = _keyboard->getSampleIdByEventKey(event.index);  
      loadSetPlay(_keyboard->getBank(), sampleId1, _cursorChannel, _cursorPosition);
/*      Serial.print("Setting Sample at channel ");
      Serial.print(_cursorChannel);
      Serial.print(", Position ");
      Serial.println(_cursorPosition);
*/      
    }

    // pre-listen samples
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
              _pattern.setPatternSpeed(_pattern.getPatternSpeed() + (event.pressed ? 0.5 : -0.5));
              _calculatePlaybackTickSpeed();
              _sequencerScreen.drawBPM(_pattern.getPatternSpeed());
            } else {
              moveCursor(event.pressed ? DOWN : UP);
            }
          break;
        
        case Sucofunkey::ENCODER_2:
            if (_isPlaying) {
              // what will be the function?
            } else {
              moveCursor(event.pressed ? RIGHT : LEFT);              
            }
          break;

        case Sucofunkey::ENCODER_3:
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);
            
            if (event.pressed && _sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.SCROLL_RIGHT)) _sequencerScreen.drawGrid(_sequencerScreen.SCROLL_RIGHT);
            if (!event.pressed && _sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.SCROLL_LEFT)) _sequencerScreen.drawGrid(_sequencerScreen.SCROLL_LEFT);            
            
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);            
          break;

        case Sucofunkey::ENCODER_4:
            if (!_isPlaying) {
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);
              _pattern.changePatternLengthByTick(event.pressed);

              if (_cursorPosition >= _pattern.getPatternLength()) _cursorPosition = _pattern.getPatternLength()-1;

              _sequencerScreen.drawGrid(event.pressed ? _sequencerScreen.APPEND : _sequencerScreen.SHORTEN);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            }
          break;

          case Sucofunkey::FN_ENCODER_1: 
            if (!_isPlaying) {
              if (event.pressed) {
                _pattern.increaseVelocity(_cursorChannel, _cursorPosition);
              } else {
                _pattern.decreaseVelocity(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoVolume(_cursorChannel, _cursorPosition);              
            }
            break;
          case Sucofunkey::FN_ENCODER_2:
            if (!_isPlaying) {
              if (event.pressed) {
                _pattern.stereoPositionTickRight(_cursorChannel, _cursorPosition);
              } else {
                _pattern.stereoPositionTickLeft(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoPanning(_cursorChannel, _cursorPosition);              
            }
            break;
          case Sucofunkey::FN_ENCODER_3:
            Serial.print("FN ENCODER 3::");
            Serial.println(event.pressed);
            break;
          case Sucofunkey::FN_ENCODER_4:
            if (!_isPlaying) {
              if (event.pressed) {
                _pattern.increaseProbability(_cursorChannel, _cursorPosition);
              } else {
                _pattern.decreaseProbability(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoProbability(_cursorChannel, _cursorPosition);              
            }
            break;
      }
    }
}


void Sequencer::moveCursor(Direction direction) {
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);

  switch(direction) {
    case UP:  _cursorChannel = _cursorChannel > 0 ? _cursorChannel-1 : 0;
              break;
    case DOWN:
              _cursorChannel = _cursorChannel < _pattern.channels-1 ? _cursorChannel+1 : _pattern.channels-1;
              break;
    case LEFT:
              _cursorPosition = _cursorPosition > 0 ? _cursorPosition-1 : 0;
              if (_sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.LEFT)) _sequencerScreen.drawGrid(_sequencerScreen.LEFT);
              break;
    case RIGHT:
              _cursorPosition = _cursorPosition < _pattern.getPatternLength()-1 ? _cursorPosition+1 : _pattern.getPatternLength()-1;
              if (_sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.RIGHT)) _sequencerScreen.drawGrid(_sequencerScreen.RIGHT);
              break;          
    default:
              break;
  }

  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
};




void Sequencer::loadSetPlay(byte bank1, byte sample1, byte channel, int position) {
  if (_sfsio->sampleBanksStatus[bank1-1][sample1-1]) {
    _audioResources->playMem.stop();
    _pattern.setSampleAt(channel, position, (bank1-1)*24+sample1, 64, 64);

    // load Sample if not in extmem yet
    if(_sfsio->addSampleToMemory(bank1, sample1, false)) {
      // success.. there was enough memory ;)
      _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
      _audioResources->playMem.play(_extmemArray + _sfsio->getExtmemOffset((bank1-1)*24+sample1));
      _sequencerScreen.drawSample(channel, position, false);
    } else {
      // failed, because there is not enough memory left
      _pattern.unsetSampleAt(channel, position);
      Serial.println("out of memory");
    }   
  }
}


long Sequencer::bpmToMicroseconds(float bpm, int res) {
  return floor((60000000.0/bpm/res));
}


void Sequencer::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);     
    
    _sequencerScreen.initializeGrid(&_pattern);
    _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
//    _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
 }
 else {
    _isActive = false;
    _keyboard->setBank(0);
    // ToDo: turn off any Note LEDs
  }
}

void Sequencer::playPattern() {
   _isPlaying = !_isPlaying;
  _blinkPosition = 0;

  if (!_isPlaying) { 
    stopAllChannels();
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, false); 
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
  } else {
    _playerPosition = 0;
    _blinkPosition = 0;
  }
}

void Sequencer::pausePattern() {
  Serial.println("PAUSE");
  if (_isPlaying) {
    _isPlaying = false;
    stopAllChannels();
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, false);
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
  } else {
    // ToDo: draw cursor somewhere ;)
  }
}

void Sequencer::_playNext() {
    _sequencerScreen.drawPlayStepIndicator(_playerPosition == 0 ? _pattern.getPatternLength()-1 : _playerPosition-1, false);
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, true);

    for (byte c=0; c < _pattern.channels; c++) {
      if (_pattern.getSampleAt(c, _playerPosition).sampleNumber < 255 && _sfsio->getExtmemOffset(_pattern.getSampleAt(c, _playerPosition).sampleNumber) != -1) {
        playMixedSample(c, _playerPosition);
      }      
    }

    if (_playerPosition == _pattern.getPatternLength()-1) { 
      _playerPosition = 0;
    } else {
      _playerPosition++;
    }
//    Serial.print("PlayerPosition::");
//    Serial.println(_playerPosition);
}


void Sequencer::playMixedSample(byte channel, uint16_t position) {
  switch (channel) {
    case 0:
      _playMemory = &_audioResources->playMem1;
      _playMemoryMixerL = &_audioResources->mixerMem1L;
      _playMemoryMixerR = &_audioResources->mixerMem1R;
      _playMemoryMixerGain = 0;
      break;
    case 1:
      _playMemory = &_audioResources->playMem2;
      _playMemoryMixerL = &_audioResources->mixerMem1L;
      _playMemoryMixerR = &_audioResources->mixerMem1R;
      _playMemoryMixerGain = 1;    
      break;
    case 2:
      _playMemory = &_audioResources->playMem3;
      _playMemoryMixerL = &_audioResources->mixerMem1L;
      _playMemoryMixerR = &_audioResources->mixerMem1R;
      _playMemoryMixerGain = 2;    
      break;
    case 3:
      _playMemory = &_audioResources->playMem4;
      _playMemoryMixerL = &_audioResources->mixerMem1L;
      _playMemoryMixerR = &_audioResources->mixerMem1R;
      _playMemoryMixerGain = 3;    
      break;
    case 4:
      _playMemory = &_audioResources->playMem5;
      _playMemoryMixerL = &_audioResources->mixerMem2L;
      _playMemoryMixerR = &_audioResources->mixerMem2R;
      _playMemoryMixerGain = 0;
      break;
    case 5:
      _playMemory = &_audioResources->playMem6;
      _playMemoryMixerL = &_audioResources->mixerMem2L;
      _playMemoryMixerR = &_audioResources->mixerMem2R;
      _playMemoryMixerGain = 1;
      break;
    case 6:
      _playMemory = &_audioResources->playMem7;
      _playMemoryMixerL = &_audioResources->mixerMem2L;
      _playMemoryMixerR = &_audioResources->mixerMem2R;
      _playMemoryMixerGain = 2;
      break;
    case 7:
      _playMemory = &_audioResources->playMem8;
      _playMemoryMixerL = &_audioResources->mixerMem2L;
      _playMemoryMixerR = &_audioResources->mixerMem2R;
      _playMemoryMixerGain = 3;
      break;
  }

  if (_pattern.getSampleAt(channel, _playerPosition).sampleNumber == 254) {
    _playMemory->stop();
  } else {
    if (_pattern.getSampleAt(channel, _playerPosition).probability == 100 || random(100) <= _pattern.getSampleAt(channel, _playerPosition).probability) {
      if (_pattern.getSampleAt(channel, _playerPosition).stereoPosition < 64) {
        _playMemoryMixerL->gain(_playMemoryMixerGain, (_pattern.getSampleAt(channel, _playerPosition).velocity/127.0)*1.0 );
        _playMemoryMixerR->gain(_playMemoryMixerGain, (_pattern.getSampleAt(channel, _playerPosition).velocity/127.0)*(_pattern.getSampleAt(channel, _playerPosition).stereoPosition/64.0));
      } else {
        _playMemoryMixerL->gain(_playMemoryMixerGain, (_pattern.getSampleAt(channel, _playerPosition).velocity/127.0)*((127-_pattern.getSampleAt(channel, _playerPosition).stereoPosition)/64.0));
        _playMemoryMixerR->gain(_playMemoryMixerGain, (_pattern.getSampleAt(channel, _playerPosition).velocity/127.0)*1.0);
      }
  
      _playMemory->play(_extmemArray + _sfsio->getExtmemOffset(_pattern.getSampleAt(channel, _playerPosition).sampleNumber));
    }
  }
}


void Sequencer::stopAllChannels() {
  _playLEDon = false;
  _audioResources->playMem.stop();
  _audioResources->playMem1.stop();
  _audioResources->playMem2.stop();
  _audioResources->playMem3.stop();
  _audioResources->playMem4.stop();
  _audioResources->playMem5.stop();
  _audioResources->playMem6.stop();
  _audioResources->playMem7.stop();
  _audioResources->playMem8.stop();
}

boolean Sequencer::isPlaying() {
  return _isPlaying;
}



long Sequencer::_calculatePlaybackTickSpeed() {
  _playbackTickSpeed = bpmToMicroseconds(_pattern.getPatternSpeed(), _pattern.getPatternResolution());
  return _playbackTickSpeed;
}


void Sequencer::_printPatternData() {
  for (int i=0; i<_pattern.getPatternLength(); i++) {
    Serial.print(i);
    Serial.print(" \t ");
    for (int c=0; c<_pattern.channels; c++) {
      Serial.print(_pattern.getSampleAt(c, i).sampleNumber);
      Serial.print(" \t ");                  
    }
    Serial.println();
  }
  Serial.println("-----");
}
