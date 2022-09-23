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
#include <Audio.h>
#include <TeensyThreads.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Sequencer.h"
#include "SongStructure.h"
#include "../sampler/Sampler.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"
#include "Swing.h"

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
    //_blackKeyMenu.setOption(1, "SEL");
    _blackKeyMenu.setOption(2, "MOV");
    _blackKeyMenu.setOption(3, "DBL");


    _blackKeyMenu.setOption(6, "SND");
    _blackKeyMenu.setOption(7, "INS");
    _blackKeyMenu.setOption(8, "REV");

    //_blackKeyMenu.setOption(9, "LOD");    
    _blackKeyMenu.setOption(10, "CLS");    

    _pianoKeyboard = PianoKeyboard(_keyboard, _screen);
    _swing = Swing();
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
              
        case Sucofunkey::SET_CURSOR_LEFT:              
              _song.swingLevelDown(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;
        case Sucofunkey::SET_CURSOR_RIGHT:
              _song.swingLevelUp(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);              
              break;
        case Sucofunkey::SET_CURSOR_UP:
              _song.swingGroupUp(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;
        case Sucofunkey::SET_CURSOR_DOWN:
              _song.swingGroupDown(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);              
              break;

        case Sucofunkey::FN_CURSOR_UP:
              _cursorPosition = zoom.zoomIn(_cursorPosition);
              _sequencerScreen.initializeGrid(&_song, _cursorPosition, &_swing);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;
        case Sucofunkey::FN_CURSOR_DOWN:
              _cursorPosition = zoom.zoomOut(_cursorPosition);              
              _sequencerScreen.initializeGrid(&_song, _cursorPosition, &_swing);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);              
              break;
                
        // Menu switch will try to open BlackKey menu
        case Sucofunkey::MENU:
              setMenuState(MENU_BLACKKEY);
              break;

        case Sucofunkey::PLAY:
              _calculatePlaybackTickSpeed();
              _checkIfAllSamplesAreLoaded();
              playSong();
              break;
        case Sucofunkey::PAUSE:
              stopSong();
              // autosave
              saveToSD();
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
              setMenuState(MENU_PIANO);
              break;    

        // set velocity from fader
        case Sucofunkey::ENCODER_1_PUSH:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                _song.setVelocity(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(1, 127)));
                _sequencerScreen.updateSampleInfoVolume(_cursorChannel, _cursorPosition);
                
                if (!_isPlaying) { playMixedSample(_cursorChannel, _cursorPosition); }   
            }
          break;
        // set panning from fader          
        case Sucofunkey::ENCODER_2_PUSH:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                _song.setStereoPosition(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(1, 127)));
                _sequencerScreen.updateSampleInfoPanning(_cursorChannel, _cursorPosition);

                if (!_isPlaying) { playMixedSample(_cursorChannel, _cursorPosition); }   
            }
          break;
        // set pitch from fader          
        case Sucofunkey::ENCODER_3_PUSH:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
                _song.setPitchByMidiNote(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(35, 85)));                
                _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);

                if (!_isPlaying) { playMixedSample(_cursorChannel, _cursorPosition); }
            }
            break;            
        // set probability from fader          
        case Sucofunkey::ENCODER_4_PUSH:
            if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
                _song.setProbability(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(0, 100)));
                _sequencerScreen.updateSampleInfoProbability(_cursorChannel, _cursorPosition);
            }
          break;
      }          
    }


    if (event.type == Sucofunkey::KEY_NOTE && event.pressed) {
      // check if blackKeys menu is active and pass the key to it
      if (_currentMenuState == MENU_BLACKKEY) {
        _blackKeyMenu.handleEvent(event);
        return;
      }

      byte sampleId1 = _keyboard->getSampleIdByEventKey(event.index);
        
      if (_currentMenuState == MENU_PIANO) {
        
        // ToDo: basenote setzen FP+NoteKey? -> eher in sampler??? -> da wird dann ja auch ADSR Hüllkurve gesetzt..

        if (_song.isSomethingAt(_cursorChannel, _cursorPosition)) {
          // is it a sample? -> set reference as origin in piano roll
          if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
            _pianoKeyboard.setOriginReference(_cursorChannel, _cursorPosition);
            _song.setPitchByMidiNote(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition(), (_keyboard->getBank()*24)+sampleId1+4);          
            playMixedSample(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition());
            _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);
          }
          // ToDo: add for midi 

        } else {
          // empty cell -> copy piano origin, if available
          if (_pianoKeyboard.hasOrigin()) {
            _song.copyPosition(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition(), _cursorChannel, _cursorPosition, false);
            _song.setPitchByMidiNote(_cursorChannel, _cursorPosition, (_keyboard->getBank()*24)+sampleId1+4);
            playMixedSample(_cursorChannel, _cursorPosition);
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
          }
        }

      } else {
        // set sample at cursor position
        loadSetPlay(_keyboard->getBank(), sampleId1, _cursorChannel, _cursorPosition);
      }
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
      SongStructure::pointerType t = _song.getPosition(_cursorChannel, _cursorPosition).type;
      
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

              // if cursor is at the end, move it one cell left, otherwise it would be out off the grid
              if (_cursorPosition >= _song.getSongLength()) {
                _cursorPosition = _song.getSongLength()-zoom.getZoomlevelOffset();
              }

              _sequencerScreen.drawGrid(event.pressed ? _sequencerScreen.APPEND : _sequencerScreen.SHORTEN);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            }
          break;



          case Sucofunkey::ENCODER_1:             
            if ( t == SongStructure::SAMPLE || t == SongStructure::PARAMETER_CHANGE_SAMPLE || t == SongStructure::MIDINOTE) {
              if (event.pressed) {
                _song.increaseVelocity(_cursorChannel, _cursorPosition);
              } else {
                _song.decreaseVelocity(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoVolume(_cursorChannel, _cursorPosition);              
            } else {
              // change playback speed (BPM) when nothing is selected.. same as FN+ENCODER_1 while playing
              _song.setPlayBackSpeed(_song.getPlayBackSpeed() + (event.pressed ? 0.5 : -0.5));
              _calculatePlaybackTickSpeed();
              _sequencerScreen.drawBPM(_song.getPlayBackSpeed());
            }
            break;
          case Sucofunkey::ENCODER_2:
              if (event.pressed) {
                if ( t == SongStructure::SAMPLE || t == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                  _song.stereoPositionTickRight(_cursorChannel, _cursorPosition);
                }
                if ( t == SongStructure::MIDINOTE) {
                  _song.increaseMidiChannel(_cursorChannel, _cursorPosition);
                }                
              } else {
                if ( t == SongStructure::SAMPLE || t == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                  _song.stereoPositionTickLeft(_cursorChannel, _cursorPosition);
                }
                if ( t == SongStructure::MIDINOTE) {
                  _song.decreaseMidiChannel(_cursorChannel, _cursorPosition);
                }            
              }              

              if ( t == SongStructure::SAMPLE || t == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                _sequencerScreen.updateSampleInfoPanning(_cursorChannel, _cursorPosition);              
              }
              if ( t == SongStructure::MIDINOTE) {
                _sequencerScreen.updateMidiChannel(_cursorChannel, _cursorPosition);              
              }            

            break;
          case Sucofunkey::ENCODER_3:
            if ( t == SongStructure::SAMPLE || t == SongStructure::MIDINOTE || t == SongStructure::PARAMETER_CHANGE_SAMPLE) {
              if (event.pressed) {
                _song.increasePitchByOne(_cursorChannel, _cursorPosition);
              } else {
                _song.decreasePitchByOne(_cursorChannel, _cursorPosition);
              }

              _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);              
            }
            break;
          case Sucofunkey::ENCODER_4:
            if ( t == SongStructure::SAMPLE || t == SongStructure::MIDINOTE) {
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
          //Serial.println("SELECT");
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
          _song.setMidiNote(_cursorChannel, _cursorPosition, {});
          _sequencerScreen.drawSample(_cursorChannel, _cursorPosition, false);
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM8:
          _song.reverseSamplePlayback(_cursorChannel, _cursorPosition);
          _sequencerScreen.drawSample(_cursorChannel, _cursorPosition, false);
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM9:
          // loadFromSD(true);
          break;          
        case Sucofunkey::BLACKKEY_NAV_ITEM10:
          // CLS -> deletes all notes from song
          setSequencerState(CONFIRM_CLS); 
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
      if (_currentMenuState == MENU_PIANO) {
        _pianoKeyboard.hide();
      }
      _currentMenuState = MENU_NONE;
      break;
    case MENU_BLACKKEY:
      if (_currentMenuState == MENU_BLACKKEY) {
        _blackKeyMenu.hideMenu();
        _currentMenuState = MENU_NONE;
        setSequencerState(NORMAL);
      } else {
        if (_currentMenuState == MENU_PIANO) {
          _pianoKeyboard.hide();
        }
        _blackKeyMenu.showMenu();
        _currentMenuState = MENU_BLACKKEY;
        setSequencerState(NORMAL);
      }
      break;
    case MENU_PIANO:
      if (_currentMenuState == MENU_PIANO) {
        _pianoKeyboard.hide();
        _currentMenuState = MENU_BLACKKEY;
        _blackKeyMenu.showMenu();
      } else {
        // only switch to piano, if a sample ist selected -> otherwise there would be nothing to play
        // needs to be extended, when synth is implemented
        // ToDo: add for midi, too

        if (_song.getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
          _blackKeyMenu.hideMenu();
          _pianoKeyboard.show();
          _pianoKeyboard.setOriginReference(_cursorChannel, _cursorPosition);
          _currentMenuState = MENU_PIANO;
          setSequencerState(NORMAL);
        }
      }
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
          break;
      case CONFIRM_CLS:
          _clearSong();
          _blackKeyMenu.setExclusiveAction(10, false);
          _currentSequencerState = NORMAL;            
          break;
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
      case CONFIRM_CLS:
          _blackKeyMenu.setExclusiveAction(10, true);
          _currentSequencerState = CONFIRM_CLS;
        break;
      default:
        _currentSequencerState = NORMAL;
        break;
    }    
  }
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
    s.baseMidiNote = 60;
    s.pitchedNote = 60;
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


void Sequencer::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);     
    
    _screen->clearAreaRTL(_screen->AREA_SCREEN, _screen->C_BLACK, 0);
    _sequencerScreen.initializeGrid(&_song, _cursorPosition, &_swing);
    _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);

    _song.setCurrentPosition(_cursorChannel, _cursorPosition, true);
    _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
    setSequencerState(NORMAL);
    setMenuState(MENU_NONE);
  }
  else {
      _isActive = false;
      _keyboard->setBank(0);
      // ToDo: turn off any Note LEDs
    }
}


void Sequencer::playSong() {
   _isPlaying = !_isPlaying;

  if (!_isPlaying) { 
    stopAllChannels();

    // song was paused (2x play), next play will start at cursor position
    _nextPlayStartAtCursor = true;    
    _playerPosition = _cursorPosition;
    _blinkPosition = _cursorPosition;

    _sequencerScreen.drawPlayStepIndicator(_playerPosition, false); 
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
    // autosave
    saveToSD();
  } else {
    if (_nextPlayStartAtCursor) {
      _playerPosition = _cursorPosition;
      _blinkPosition = _cursorPosition;
    } else {
      _playerPosition = 0;
      _blinkPosition = 0;
    }
  }
}


// stops playing the song and (even if not playing) moves player cursor to start
void Sequencer::stopSong() {
  if (_isPlaying) {
    _isPlaying = false;
    stopAllChannels();
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, false);
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
  } else {
    // ToDo: draw cursor somewhere ;)
    stopAllChannels();
  }
  _nextPlayStartAtCursor = false;
  _playerPosition = 0;
}


void Sequencer::_playNext() {
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, true);

    SongStructure::samplePointerStruct sps;

    for (byte c=0; c < 8; c++) {
      sps = _song.getPosition(c, _playerPosition); 
      if (sps.type == SongStructure::MIDINOTE) {
        
        if (random(100) <= _song.getMidiNoteFromBucketId(sps.typeIndex).probability) {
          if (_song.getMidiNoteFromBucketId(sps.typeIndex).velocity > 0) {
            _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_ON, _song.getMidiNoteFromBucketId(sps.typeIndex).note, _song.getMidiNoteFromBucketId(sps.typeIndex).velocity, _song.getMidiNoteFromBucketId(sps.typeIndex).channel);
          } else {
            _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_OFF, _song.getMidiNoteFromBucketId(sps.typeIndex).note, 0, _song.getMidiNoteFromBucketId(sps.typeIndex).channel);
          }      
        }        
      } else {
        if (sps.type != SongStructure::UNDEFINED) {
          playMixedSample(c, _playerPosition);    
        }      
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
      _playMemoryMixerGain = channel;
  } else {
      _playMemoryMixerL = &_audioResources->mixerMem2L;
      _playMemoryMixerR = &_audioResources->mixerMem2R;
      _playMemoryMixerGain = channel-4;
  }

  switch (channel) {
    case 0:
      _playMemory = &_audioResources->playMem1;
      break;
    case 1:
      _playMemory = &_audioResources->playMem2;
      break;
    case 2:
      _playMemory = &_audioResources->playMem3;
      break;
    case 3:
      _playMemory = &_audioResources->playMem4;
      break;
    case 4:
      _playMemory = &_audioResources->playMem5;
      break;
    case 5:
      _playMemory = &_audioResources->playMem6;
      break;
    case 6:
      _playMemory = &_audioResources->playMem7;
      break;
    case 7:
      _playMemory = &_audioResources->playMem8;
      break;
  }
}


void Sequencer::playMixedSample(byte channel, uint16_t position) {
  _prepareMixerRouting(channel);

  SongStructure::samplePointerStruct sps = _song.getPosition(channel, position);

  if (sps.type == SongStructure::NOTE_OFF) {
    _playMemory->stop();
  } else {

    byte velocity = 64;
    byte stereoPosition = 64;
    byte probability = 100;
    byte pitchedNote = 0;
    byte baseMidiNote = 0;
    byte sampleNumber = 0;
    byte pitchChange = 0;
    byte swingLevel = 0;
    byte swingGroup = 0;
    int shiftSamples = 0;
    boolean reverse = false;

    byte swingExpression;

    switch (sps.type) {
      case SongStructure::SAMPLE:
        velocity = _song.getSampleFromBucketId(sps.typeIndex).velocity;
        stereoPosition = _song.getSampleFromBucketId(sps.typeIndex).stereoPosition;
        probability = _song.getSampleFromBucketId(sps.typeIndex).probability;
        pitchedNote = _song.getSampleFromBucketId(sps.typeIndex).pitchedNote;
        baseMidiNote = _song.getSampleFromBucketId(sps.typeIndex).baseMidiNote;
        sampleNumber = _song.getSampleFromBucketId(sps.typeIndex).sampleNumber;
        
        swingExpression = _song.getSampleFromBucketId(sps.typeIndex).swing;
        swingLevel = _swing.getLevelFromBinarySwingExpression(swingExpression);
        swingGroup = _swing.getGroupFromBinarySwingExpression(swingExpression);    
        reverse =  _song.getSampleFromBucketId(sps.typeIndex).reverse;   
        break;

      case SongStructure::PARAMETER_CHANGE_SAMPLE:
        velocity = _song.getParameterChangeFromBucketId(sps.typeIndex).velocity;
        stereoPosition = _song.getParameterChangeFromBucketId(sps.typeIndex).stereoPosition;
        pitchChange = _song.getParameterChangeFromBucketId(sps.typeIndex).pitchChange;
        reverse = _song.getParameterChangeFromBucketId(sps.typeIndex).reverse;
        break;

      default:
        break;
    }

    // is there something to do? -> sample probability OK or a parameter change. then go!
    if (random(100) <= probability || sps.type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
      if (stereoPosition < 64) {
        _playMemoryMixerL->gain(_playMemoryMixerGain, (velocity/127.0)*1.0);
        _playMemoryMixerR->gain(_playMemoryMixerGain, (velocity/127.0)*(stereoPosition/64.0));
      } else {
        _playMemoryMixerL->gain(_playMemoryMixerGain, (velocity/127.0)*((127-stereoPosition)/64.0));
        _playMemoryMixerR->gain(_playMemoryMixerGain, (velocity/127.0)*1.0);
      }
  
      if (sps.type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
        _playMemory->adjustFrequencyByTick(pitchChange-127);
        if (reverse) _playMemory->reversePlayback();        
      }

      if (swingGroup != 0) {
        swingLevel = _song.getSwingGroupLevel(swingGroup);
      }
      shiftSamples = _swing.getShiftSamplesForSwingLevel(swingLevel);

      // do not play a sample, if position is a parameter change .. but check if sample is in EXTMEM
      if (_song.getPosition(channel, position).type == SongStructure::SAMPLE && _sfsio->getExtmemOffset(_song.getSampleFromBucketId(_song.getPosition(channel, position).typeIndex).sampleNumber) != -1)  {
        _playMemory->playPitched(_extmemArray + _sfsio->getExtmemOffset(sampleNumber), 60, pitchedNote, shiftSamples, reverse);
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
}

boolean Sequencer::isPlaying() {
  return _isPlaying;
}


void Sequencer::loadFromSD(boolean drawGrid) {
  // load from SD card
  _song.loadFromSD(_sfsio->getSongPath());  
  _cursorPosition = 0;

  // redraw the grid
  if (drawGrid) {
    _sequencerScreen.initializeGrid(&_song, _cursorPosition, &_swing);
    _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);

    // set right position in song structure
    _song.setCurrentPosition(_cursorChannel, _cursorPosition, true);
  }

  // loading samples to extmem
  _sfsio->loadSamplesToMemory(_song.getSamplesUsed());
  
  if (drawGrid) {
    _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
  }
};

void Sequencer::saveToSD() {
  _song.saveToSD(_sfsio->getSongPath());
};

void Sequencer::debugInfos() {
  _song.debugInfos();
};


long Sequencer::bpmToMicroseconds(float bpm, int res) {
  return floor((60000000.0/bpm/res));
}

long Sequencer::_calculatePlaybackTickSpeed() {  
  _playbackTickSpeed = bpmToMicroseconds(_song.getPlayBackSpeed(), _song.getSongResolution()*4);
  // recalculate swing delays
  _swing.setTickMicroseconds(_playbackTickSpeed);
  return _playbackTickSpeed;
}

// checks if the used samples in the song are available on disk and already loaded into extmem. if not, do so..
void Sequencer::_checkIfAllSamplesAreLoaded() {
  boolean *sampleArray = _song.getSamplesUsed();

  for (byte b=0; b<3; b++) {
    for (byte s=0; s<24; s++) {
      if (sampleArray[b*24+s]) {
        if (_sfsio->sampleAvailable(b*24+s) && !_sfsio->sampleInMemory(b*24+s)) {
          _sfsio->addSampleToMemory(b+1, s+1, true);
        }
      }
    }
  }
}


void Sequencer::_clearSong() {
    // 1 empty sample ram
  _sfsio->clearSampleMemory();

  // 2 clear all data structures or initialize it new
  _song.clearSelection(0, 0, 7, _song.getSongLength());

  // 3 set defaults for meta data
  _song.setMetadataToDefault();

  // 4 redraw grid
  _cursorPosition = 0;
  _cursorChannel = 0;

  _sequencerScreen.initializeGrid(&_song, _cursorPosition, &_swing);
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
  _song.setCurrentPosition(_cursorChannel, _cursorPosition, true);       
  _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
}