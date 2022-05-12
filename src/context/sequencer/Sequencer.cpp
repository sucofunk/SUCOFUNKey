#include <Arduino.h>
#include <Audio.h>
#include <TeensyThreads.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Sequencer.h"
#include "SongStructure.h"
#include "../sampler/Sampler.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"

Sequencer::Sequencer(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources) {
    _keyboard = keyboard;
    _screen = screen;    
    _sfsio = sfsio;    
    _fsio = fsio;
    _extmemArray = extmemArray;
    _audioResources = audioResources;
    
    zoom = Zoom();
    _sequencerScreen = SequencerScreen(_keyboard, _screen, _sfsio, _audioResources, &zoom);

    _blackKeyMenu = BlackKeyMenu(_keyboard, _screen);
    _blackKeyMenu.setOption(1, "SEL");
    _blackKeyMenu.setOption(2, "MOV");
    _blackKeyMenu.setOption(3, "DBL");
    
    _blackKeyMenu.setOption(6, "SND");
    _blackKeyMenu.setOption(7, "INS");
    _blackKeyMenu.setOption(8, "FX");    

    _blackKeyMenu.setOption(9, "LOD");    
    _blackKeyMenu.setOption(10, "SAV");    
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

 // was:  if (_blinkPosition < _pattern.getPatternResolution()-1) {
  if (_blinkPosition < (_song.getSongResolution()*4)-1) {
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
        
        case Sucofunkey::FN_CURSOR_UP:
              _cursorPosition = zoom.zoomIn(_cursorPosition);
              _sequencerScreen.initializeGrid(&_song, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;
        case Sucofunkey::FN_CURSOR_DOWN:
              _cursorPosition = zoom.zoomOut(_cursorPosition);              
              _sequencerScreen.initializeGrid(&_song, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);              
              break;
                
        // Menu switch will try to open BlackKey menu
        case Sucofunkey::MENU:
              setMenuState(MENU_BLACKKEY);
              break;

        case Sucofunkey::PLAY:
              _calculatePlaybackTickSpeed();
              playSong();
              break;
        case Sucofunkey::PAUSE:
              pausePattern();
              break;
        case Sucofunkey::FN_PLAY:
                // play current sample, if one is selected
              if (!_isPlaying) { playMixedSample(_cursorChannel, _cursorPosition); }                
              break;   
        case Sucofunkey::FN_SET:
              // delete whatever is at the current position
              _song.removePosition(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;                  
        case Sucofunkey::ZOOM:
              // ToDo: change Menu State to PIANO and change state to something like pitch select
              break;    

        // set velocity from fader
        case Sucofunkey::ENCODER_1_PUSH:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                _song.setVelocity(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(1, 127)));
                _sequencerScreen.updateSampleInfoVolume(_cursorChannel, _cursorPosition);              
            }
          break;
        // set panning from fader          
        case Sucofunkey::ENCODER_2_PUSH:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                _song.setStereoPosition(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(1, 127)));
                _sequencerScreen.updateSampleInfoPanning(_cursorChannel, _cursorPosition);
            }
          break;
      }    
      
    }


    if (event.type == Sucofunkey::KEY_NOTE && event.pressed) {

      // check if blackKeys menu is active and pass the key to it, if it is a black key
      if (_currentMenuState == MENU_BLACKKEY && _keyboard->isEventBlackKey(event.index)) {
        _blackKeyMenu.handleEvent(event);
        return;
      }

      byte sampleId1 = _keyboard->getSampleIdByEventKey(event.index);


// ToDo: handle Pitch selection via piano roll as it was before

/*      if (_keyboardMode && _pattern.getSampleAt(_cursorChannel, _cursorPosition).sampleNumber < 253 ) {
        // play pitched note: bankstart(1:29|2:63|3:77)+sampleId1-1 if in keyboard mode
        _pattern.setPitchByMidiNote(_cursorChannel, _cursorPosition, 52+sampleId1);
        if (!_isPlaying) { 
          stopAllChannels();
          _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);
          playMixedSample(_cursorChannel, _cursorPosition); 
        }
      } else {
*/        
        // set sample at cursor position
        loadSetPlay(_keyboard->getBank(), sampleId1, _cursorChannel, _cursorPosition);
//      }     
    }

    // pre-listen samples (in NORMAL STATE) --> ToDo: setBaseNote (in PIANO PITCH WHATEVER STATE)
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
        case Sucofunkey::FN_ENCODER_1:
            if (_isPlaying) {
              _song.setPlayBackSpeed(_song.getPlayBackSpeed() + (event.pressed ? 0.5 : -0.5));
              _calculatePlaybackTickSpeed();
              _sequencerScreen.drawBPM(_song.getPlayBackSpeed());
            } else {
              moveCursor(event.pressed ? DOWN : UP);
            }
          break;
        
        case Sucofunkey::FN_ENCODER_2:
            if (_isPlaying) {
              // what will be the function?
            } else {
              moveCursor(event.pressed ? RIGHT : LEFT);              
            }
          break;

        case Sucofunkey::FN_ENCODER_3:
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);
            
            if (event.pressed && _sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.SCROLL_RIGHT)) _sequencerScreen.drawGrid(_sequencerScreen.SCROLL_RIGHT);
            if (!event.pressed && _sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.SCROLL_LEFT)) _sequencerScreen.drawGrid(_sequencerScreen.SCROLL_LEFT);            
            
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);            
          break;

        case Sucofunkey::FN_ENCODER_4:
            if (!_isPlaying) {
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);              
              _song.changeSongLengthByTick(event.pressed, 4); // 4 is one tick at zoom level NORMAL.. needed to keep everything in shape when moving on different zoom levels

              // ToDo: check if maximum song length is reached!

              // if cursor is at the end, move it one cell left, otherwise it would be out of the grid
              if (_cursorPosition >= _song.getSongLength()) {
                //_cursorPosition = _song.getSongLength()-1;
                _cursorPosition = _song.getSongLength()-zoom.getZoomlevelOffset();
              }

              _sequencerScreen.drawGrid(event.pressed ? _sequencerScreen.APPEND : _sequencerScreen.SHORTEN);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            }
          break;



          case Sucofunkey::ENCODER_1: 
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
              if (event.pressed) {
                _song.increaseVelocity(_cursorChannel, _cursorPosition);
              } else {
                _song.decreaseVelocity(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoVolume(_cursorChannel, _cursorPosition);              
            }
            break;
          case Sucofunkey::ENCODER_2:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
              if (event.pressed) {
                _song.stereoPositionTickRight(_cursorChannel, _cursorPosition);
              } else {
                _song.stereoPositionTickLeft(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoPanning(_cursorChannel, _cursorPosition);              
            }
            break;
          case Sucofunkey::ENCODER_3:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
              if (event.pressed) {
                _song.increasePitchByOne(_cursorChannel, _cursorPosition);
              } else {
                _song.decreasePitchByOne(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);              
            }
            break;
          case Sucofunkey::ENCODER_4:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
              if (event.pressed) {
                _song.increaseProbability(_cursorChannel, _cursorPosition);
              } else {
                _song.decreaseProbability(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoProbability(_cursorChannel, _cursorPosition);              
            }
            break;
      }
    }

    // Application Events from e.g. black key menue
    if (event.type == Sucofunkey::EVENT_APPLICATION) {
      switch (event.index) {
        case Sucofunkey::BLACKKEY_NAV_ITEM1:
          Serial.println("SELECT");
          break;
        case Sucofunkey::BLACKKEY_NAV_ITEM2:
              setSequencerState(MOVE_CELL);
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM3:
              setSequencerState(DOUBLE_CELL);
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM4:
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM5:
          break;
        case Sucofunkey::BLACKKEY_NAV_ITEM6:  
            {
              // Parameter Change for velocity and panning or NOTE OFF
              SongStructure::samplePointerStruct sps = _song.getPosition(_cursorChannel, _cursorPosition);
              switch (sps.type) {
                case SongStructure::SAMPLE:
                  _song.removePosition(_cursorChannel, _cursorPosition);                  
                  _song.setParameterChange(_cursorChannel, _cursorPosition, {});
                  break;
                case SongStructure::NOTE_OFF:
                  _song.removePosition(_cursorChannel, _cursorPosition);
                  break;
                case SongStructure::PARAMETER_CHANGE_SAMPLE:
                    _song.removePosition(_cursorChannel, _cursorPosition);
                    _song.setNoteOff(_cursorChannel, _cursorPosition);
                  break;
                case SongStructure::UNDEFINED:
                    _song.setParameterChange(_cursorChannel, _cursorPosition, {});
                  break;
                default:
                    _song.removePosition(_cursorChannel, _cursorPosition);
                    _song.setParameterChange(_cursorChannel, _cursorPosition, {});
                  break;
              }

              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            }
          break;
        case Sucofunkey::BLACKKEY_NAV_ITEM7:
          Serial.println("INSTRUMENT");
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM8:
          Serial.println("EFFECT");
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM9:
          loadFromSD();
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM10:
          saveToSD();
          break;                    
        default:
        break;
      }      
    }

}



void Sequencer::moveCursor(Direction direction) {
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);
  _keyboardMode = false;

  byte zoomlevelOffset = zoom.getZoomlevelOffset();

  switch(direction) {
    case UP:  _cursorChannel = _cursorChannel > 0 ? _cursorChannel-1 : 0;
              break;
    case DOWN:
              _cursorChannel = _cursorChannel < 7 ? _cursorChannel+1 : 7;
              break;
    case LEFT:
              _cursorPosition = _cursorPosition >= zoomlevelOffset ? _cursorPosition-zoomlevelOffset : 0;
              if (_sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.LEFT)) _sequencerScreen.drawGrid(_sequencerScreen.LEFT);
              break;
    case RIGHT:
              _cursorPosition = _cursorPosition < _song.getSongLength()-zoomlevelOffset ? _cursorPosition+zoomlevelOffset : _song.getSongLength()-zoomlevelOffset;
              if (_sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.RIGHT)) _sequencerScreen.drawGrid(_sequencerScreen.RIGHT);
              break;          
    default:
              break;
  }

  _song.setCurrentPosition(_cursorChannel, _cursorPosition, false);
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
};


boolean Sequencer::setMenuState(MenuState state) {
  switch (state) {
    case MENU_NONE:
      if (_currentMenuState == MENU_BLACKKEY && _currentSequencerState == NORMAL) {
        _blackKeyMenu.hideMenu();
        setSequencerState(NORMAL);
      }
      _currentMenuState = MENU_NONE;
      break;
    case MENU_BLACKKEY:
      if (_currentMenuState == MENU_BLACKKEY) {
        _blackKeyMenu.hideMenu();
        _currentMenuState = MENU_NONE;
        setSequencerState(NORMAL);
      } else {
        _blackKeyMenu.showMenu();
        _currentMenuState = MENU_BLACKKEY;
        setSequencerState(NORMAL);
      }
      break;
    case MENU_PIANO:
      // ToDo: do something!
      break;
  }

  return false;
};


void Sequencer::setSequencerState(SequencerState state) {
   if (state == _currentSequencerState) {
    // use this to do something, if the same state is set again (e.g. toggle something)
    switch(state) {
      case MOVE_CELL:          
          // move cell to here..
          if (!_savedCursorEqualsCurrent()) {
            _song.movePosition(_savedCursorChannel, _savedCursorPosition, _cursorChannel, _cursorPosition);
            _sequencerScreen.drawSample(_savedCursorChannel, _savedCursorPosition, true);
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
          }
          _blackKeyMenu.setExclusiveAction(2, false);
          _currentSequencerState = NORMAL;
        break;
      case DOUBLE_CELL:
          // drop it...
          if (_savedCursorEqualsCurrent()) {
            // drop a copy right beside the current position (double click)
            // check if song end not reached and the cell is empty
            if (_cursorPosition+zoom.getZoomlevelOffset() < _song.getSongLength() && !_song.isSomethingAt(_cursorChannel, _cursorPosition + zoom.getZoomlevelOffset())) {
              
              if(_song.copyPosition(_savedCursorChannel, _savedCursorPosition, _savedCursorChannel, _savedCursorPosition + zoom.getZoomlevelOffset(), false)) {
              moveCursor(RIGHT);
                _sequencerScreen.drawSample(_savedCursorChannel, _savedCursorPosition, true);
                _sequencerScreen.drawSample(_cursorChannel, _cursorPosition, true);
              };
            }            
          } else {
            // drop to the cursor position
            _song.copyPosition(_savedCursorChannel, _savedCursorPosition, _cursorChannel, _cursorPosition, false);
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
          }            

          _blackKeyMenu.setExclusiveAction(3, false);
          _currentSequencerState = NORMAL;      
      default:
        break;
    }
  } else {
    // time for some kind of constructor for the new state
    switch(state) {
      case NORMAL:
        _currentSequencerState = NORMAL; 
        break;
      case MOVE_CELL:
        if (_song.isSomethingAt(_cursorChannel, _cursorPosition)) {
          _saveCurrentCursorPosition();
          _blackKeyMenu.setExclusiveAction(2, true);
          _currentSequencerState = MOVE_CELL;
        } else {
          _currentSequencerState = NORMAL;
        }
        break;
      case DOUBLE_CELL:
        if (_song.isSomethingAt(_cursorChannel, _cursorPosition)) {
          _saveCurrentCursorPosition();
          _blackKeyMenu.setExclusiveAction(3, true);
          _currentSequencerState = DOUBLE_CELL;
        } else {
          _currentSequencerState = NORMAL;
        }
        break;
      default:
        _currentSequencerState = NORMAL;
        break;
    }    
  }

//  Serial.print("Sequencer State :: ");
//  Serial.println(_currentSequencerState);
};

void Sequencer::_saveCurrentCursorPosition() {
  _savedCursorChannel = _cursorChannel;
  _savedCursorPosition = _cursorPosition;
}

boolean Sequencer::_savedCursorEqualsCurrent() {
    if (_savedCursorChannel == _cursorChannel && _savedCursorPosition == _cursorPosition) {
      return true;
    } else {
      return false;
    }
}

void Sequencer::loadSetPlay(byte bank1, byte sample1, byte channel, int position) {
  if (_sfsio->sampleBanksStatus[bank1-1][sample1-1]) {
    _audioResources->playMem.stop();

    SongStructure::sampleStruct s;
    s.baseMidiNote = 64;
    s.pitchedNote = 64;
    s.probability = 100;
    s.sampleNumber = (bank1-1)*24+sample1;
    s.stereoPosition = 64;
    s.velocity = 80;

    _song.setSample(channel, position, s);

    // load Sample if not in extmem yet
    if(_sfsio->addSampleToMemory(bank1, sample1, false)) {
      // success.. there was enough memory ;)
      _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
      _audioResources->playMem.play(_extmemArray + _sfsio->getExtmemOffset((bank1-1)*24+sample1));
      _sequencerScreen.drawSample(channel, position, false);
    } else {
      // failed, because there is not enough memory left
      _song.removePosition(channel, position);
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
    
    _sequencerScreen.initializeGrid(&_song, _cursorPosition);
    _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);

    _song.setCurrentPosition(_cursorChannel, _cursorPosition, true);
    _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
  }
  else {
      _isActive = false;
      _keyboard->setBank(0);
      // ToDo: turn off any Note LEDs
    }
}

void Sequencer::playSong() {
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
  if (_isPlaying) {
    _isPlaying = false;
    stopAllChannels();
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, false);
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
  } else {
    // ToDo: draw cursor somewhere ;)
    stopAllChannels();
  }
}

void Sequencer::_playNext() {
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, true);

    for (byte c=0; c < 8; c++) {
      if (_song.getPosition(c, _playerPosition).type != SongStructure::UNDEFINED) {
        playMixedSample(c, _playerPosition);    
      }      
    }

    if (_playerPosition == _song.getSongLength()-1) { 
      _playerPosition = 0;
    } else {
      _playerPosition++;
    }
    
}

void Sequencer::_prepareMixerRouting(byte channel) {
  if (channel <= 3) {
      _playMemoryMixerL = &_audioResources->mixerMem1L;
      _playMemoryMixerR = &_audioResources->mixerMem1R;
      _playWavetableMixerL = &_audioResources->mixerWav1L;
      _playWavetableMixerR = &_audioResources->mixerWav1R;
      _playMemoryMixerGain = channel;
      _playWavetableMixerGain = channel;      
  } else {
      _playMemoryMixerL = &_audioResources->mixerMem2L;
      _playMemoryMixerR = &_audioResources->mixerMem2R;
      _playWavetableMixerL = &_audioResources->mixerWav2L;
      _playWavetableMixerR = &_audioResources->mixerWav2R;
      _playMemoryMixerGain = channel-4;
      _playWavetableMixerGain = channel-4;
  }

  switch (channel) {
    case 0:
      _playMemory = &_audioResources->playMem1;
      _playWavetable = &_audioResources->wavetable1;
      break;
    case 1:
      _playMemory = &_audioResources->playMem2;
      _playWavetable = &_audioResources->wavetable2;
      break;
    case 2:
      _playMemory = &_audioResources->playMem3;
      _playWavetable = &_audioResources->wavetable3;
      break;
    case 3:
      _playMemory = &_audioResources->playMem4;
      _playWavetable = &_audioResources->wavetable4;
      break;
    case 4:
      _playMemory = &_audioResources->playMem5;
      _playWavetable = &_audioResources->wavetable5;
      break;
    case 5:
      _playMemory = &_audioResources->playMem6;
      _playWavetable = &_audioResources->wavetable6;
      break;
    case 6:
      _playMemory = &_audioResources->playMem7;
      _playWavetable = &_audioResources->wavetable7;
      break;
    case 7:
      _playMemory = &_audioResources->playMem8;
      _playWavetable = &_audioResources->wavetable8;
      break;
  }
}


void Sequencer::playMixedSample(byte channel, uint16_t position) {
  _prepareMixerRouting(channel);

  SongStructure::samplePointerStruct sps = _song.getPosition(channel, position);

  if (sps.type == SongStructure::NOTE_OFF) {
    _playMemory->stop();
    _playWavetable->stop();
  } else {

    byte velocity = 64;
    byte stereoPosition = 64;
    byte probability = 100;
    byte pitchedNote = 0;
    byte baseMidiNote = 0;
    byte sampleNumber = 0;
    
    switch (sps.type) {
      case SongStructure::SAMPLE:
        velocity = _song.getSampleFromBucketId(sps.typeIndex).velocity;
        stereoPosition = _song.getSampleFromBucketId(sps.typeIndex).stereoPosition;
        probability = _song.getSampleFromBucketId(sps.typeIndex).probability;
        pitchedNote = _song.getSampleFromBucketId(sps.typeIndex).pitchedNote;
        baseMidiNote = _song.getSampleFromBucketId(sps.typeIndex).baseMidiNote;
        sampleNumber = _song.getSampleFromBucketId(sps.typeIndex).sampleNumber;
        break;
      case SongStructure::PARAMETER_CHANGE_SAMPLE:
        velocity = _song.getParameterChangeFromBucketId(sps.typeIndex).velocity;
        stereoPosition = _song.getParameterChangeFromBucketId(sps.typeIndex).stereoPosition;
        break;
      default:
        break;
    }

// was: if (_pattern.getSampleAt(channel, position).probability == 100 || random(100) <= _pattern.getSampleAt(channel, _playerPosition).probability) {
    if (random(100) <= probability || _song.getPosition(channel, position).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
      if (stereoPosition < 64) {
        _playMemoryMixerL->gain(_playMemoryMixerGain, (velocity/127.0)*1.0 );
        _playMemoryMixerR->gain(_playMemoryMixerGain, (velocity/127.0)*(stereoPosition/64.0));

        _playWavetableMixerL->gain(_playWavetableMixerGain, (velocity/127.0)*1.0 );
        _playWavetableMixerR->gain(_playWavetableMixerGain, (velocity/127.0)*(stereoPosition/64.0));
      } else {
        _playMemoryMixerL->gain(_playMemoryMixerGain, (velocity/127.0)*((127-stereoPosition)/64.0));
        _playMemoryMixerR->gain(_playMemoryMixerGain, (velocity/127.0)*1.0);

        _playWavetableMixerL->gain(_playWavetableMixerGain, (velocity/127.0)*((127-stereoPosition)/64.0));
        _playWavetableMixerR->gain(_playWavetableMixerGain, (velocity/127.0)*1.0);
      }
  
      // do not play a sample, if position is a parameter change .. but check if sample is in EXTMEM
      if (_song.getPosition(channel, position).type == SongStructure::SAMPLE && _sfsio->getExtmemOffset(_song.getSampleFromBucketId(_song.getPosition(channel, position).typeIndex).sampleNumber) != -1)  {

        //_sfsio->generateInstrument(_pattern.getSampleAt(channel, position).sampleNumber, 60);
        if (pitchedNote != baseMidiNote) {
          // only use wavetable, if pitch is set
          _playWavetable->setInstrument(_sfsio->getInstrumentDataBySample(sampleNumber));
          _playWavetable->playNote(pitchedNote);
        } else {
          // no pitch? just play from memory
          _playMemory->play(_extmemArray + _sfsio->getExtmemOffset(sampleNumber));
        }        
      }
    }
  }
}

void Sequencer::stopAllChannels() {
  _playLEDon = false;
  _audioResources->playMem.stop();
  _audioResources->playSdRaw.stop();
  
  _audioResources->playMem1.stop();
  _audioResources->playMem2.stop();
  _audioResources->playMem3.stop();
  _audioResources->playMem4.stop();
  _audioResources->playMem5.stop();
  _audioResources->playMem6.stop();
  _audioResources->playMem7.stop();
  _audioResources->playMem8.stop();

  _audioResources->wavetable1.stop();
  _audioResources->wavetable2.stop();
  _audioResources->wavetable3.stop();
  _audioResources->wavetable4.stop();
  _audioResources->wavetable5.stop();
  _audioResources->wavetable6.stop();
  _audioResources->wavetable7.stop();
  _audioResources->wavetable8.stop();
}

boolean Sequencer::isPlaying() {
  return _isPlaying;
}


void Sequencer::loadFromSD() {
  // load from SD card
  _song.loadFromSD(_sfsio->getSongPath());  
  _cursorPosition = 0;

  // redraw the grid
  _sequencerScreen.initializeGrid(&_song, _cursorPosition);
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);

  // set right position in song structure
  _song.setCurrentPosition(_cursorChannel, _cursorPosition, true);

  // loading samples to extmem
  _sfsio->loadSamplesToMemory(_song.getSamplesUsed());
  
  _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
};

void Sequencer::saveToSD() {
  _song.saveToSD(_sfsio->getSongPath());
};

void Sequencer::debugInfos() {
  _song.debugInfos();
};



long Sequencer::_calculatePlaybackTickSpeed() {
//  _playbackTickSpeed = bpmToMicroseconds(_song.getPlayBackSpeed(), _song.getSongResolution());
  // scale down to 
  _playbackTickSpeed = bpmToMicroseconds(_song.getPlayBackSpeed(), _song.getSongResolution()*4);

  return _playbackTickSpeed;
}
