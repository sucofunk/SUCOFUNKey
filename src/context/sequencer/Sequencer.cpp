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
#include "Snippets.h"

Sequencer::Sequencer(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, Play *play) {
    _keyboard = keyboard;
    _screen = screen;    
    _sfsio = sfsio;    
    _fsio = fsio;
    _play = play;
    _extmemArray = _play->_extmemArray;
    _audioResources = _play->_audioResources;

    zoom = Zoom();
    
    _selection = Selection();

    _blackKeyMenu = BlackKeyMenu(_keyboard, _screen);
    _blackKeyMenu.setOption(1, "SEL");
    _blackKeyMenu.setOption(2, "MOV");
    _blackKeyMenu.setOption(3, "DBL");

    _blackKeyMenu.setOption(4, "SNI");
//    _blackKeyMenu.setOption(5, "SHE");

    _blackKeyMenu.setOption(6, "SND");
    _blackKeyMenu.setOption(7, "INS");
    _blackKeyMenu.setOption(8, "REV");

    //_blackKeyMenu.setOption(9, "LOD");    
    _blackKeyMenu.setOption(10, "CLS");    

    _pianoKeyboard = PianoKeyboard(_keyboard, _screen);
    
    _song = _play->getSong();
    _snippets = _play->getSnippets();

    _sequencerScreen = SequencerScreen(_keyboard, _screen, _sfsio, _audioResources, &zoom, _play, &_selection);
    
    _playbackTickSpeed = _play->calculatePlaybackTickSpeed(); // microseconds interval to receive Ticks
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

  if (_blinkPosition < (_song->getSongResolution()*4)-1) {
    _blinkPosition++;
  } else {
    _blinkPosition = 0;
  }
  
  return _playbackTickSpeed;
}


void Sequencer::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);     
    
    _screen->clearAreaRTL(_screen->AREA_SCREEN, _screen->C_BLACK, 0);
    _sequencerScreen.initializeGrid(_song, _cursorPosition);
    _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);

    _song->setCurrentPosition(_cursorChannel, _cursorPosition, true);
    _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
    setSequencerState(NORMAL);
    setMenuState(MENU_NONE);
  }
  else {
      _isActive = false;      
      _keyboard->switchLEDsOff();
      _keyboard->setBank(0);
    }
}


void Sequencer::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
      switch(event.index) {
        // move cursor
        case Sucofunkey::CURSOR_LEFT: 
                moveCursor(LEFT);
                
              break;
        case Sucofunkey::MENU_CURSOR_LEFT: 
                _jumpToPreviousSheet();
              break;
        
        case Sucofunkey::CURSOR_RIGHT:
                moveCursor(RIGHT);
              break;
        case Sucofunkey::MENU_CURSOR_RIGHT: 
                _jumpToNextSheet();
              break;

        case Sucofunkey::CURSOR_UP:
              moveCursor(UP);
              break;

        case Sucofunkey::MENU_CURSOR_UP: 
              if (_song->addSheetDivider(_cursorPosition)) {
                _sequencerScreen.drawGrid(SequencerScreen::SHEET_ADDED);
                _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              } else {
                // no more sheets available..
                // ToDo: show message to user?
              }
              break;


        case Sucofunkey::CURSOR_DOWN:
              moveCursor(DOWN);
              break;

        case Sucofunkey::MENU_CURSOR_DOWN: 
               _song->removeSheetDivider(_cursorPosition);
               _sequencerScreen.drawGrid(SequencerScreen::SHEET_REMOVED);
               _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);                
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
              _song->swingLevelDown(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;
        case Sucofunkey::SET_CURSOR_RIGHT:
              _song->swingLevelUp(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);              
              break;

        case Sucofunkey::SET_CURSOR_UP:
                _song->swingGroupUp(_cursorChannel, _cursorPosition);
                _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;
        case Sucofunkey::SET_CURSOR_DOWN:
                _song->swingGroupDown(_cursorChannel, _cursorPosition);
                _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;

        case Sucofunkey::FN_CURSOR_UP:
              _cursorPosition = zoom.zoomIn(_cursorPosition);
              _sequencerScreen.initializeGrid(_song, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;
        case Sucofunkey::FN_CURSOR_DOWN:
              _cursorPosition = zoom.zoomOut(_cursorPosition);              
              _sequencerScreen.initializeGrid(_song, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);              
              break;
                
        // Menu switch will try to open BlackKey menu
        case Sucofunkey::MENU:
              setMenuState(MENU_BLACKKEY);
              break;

        case Sucofunkey::PLAY:
              _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
              _play->checkIfAllSamplesAreLoaded();
              
              if (_selection.isActive()) {
                playSelection();
              } else {
                if (_currentSequencerState == SNIPPET_WAIT_DELETE) {                  
                  playSnippet(_snippets->cursorInSnippet(_cursorPosition, _cursorChannel));
                } else {
                  playSong();
                }                
              }
                            
              break;
        case Sucofunkey::PAUSE:
              stopSong();
              // autosave
              saveToSD();
              break;
        case Sucofunkey::FN_PLAY:
                // play current sample, if one is selected
              if (!_isPlaying) { _play->playMixedSample(_cursorChannel, _cursorPosition, -1); }                
              break;   
        case Sucofunkey::FN_SET:
              // delete whatever is at the current position
              _song->removePosition(_cursorChannel, _cursorPosition);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              break;                  
        case Sucofunkey::ZOOM:
              setMenuState(MENU_PIANO);
              break;    

        // set velocity from fader
        case Sucofunkey::ENCODER_1_PUSH:
            if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                _song->setVelocity(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(1, 127)));
                _sequencerScreen.updateSampleInfoVolume(_cursorChannel, _cursorPosition);
                
                if (!_isPlaying) { _play->playMixedSample(_cursorChannel, _cursorPosition, -1); }   
            }
          break;
        // set panning from fader          
        case Sucofunkey::ENCODER_2_PUSH:
            if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                _song->setStereoPosition(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(1, 127)));
                _sequencerScreen.updateSampleInfoPanning(_cursorChannel, _cursorPosition);

                if (!_isPlaying) { _play->playMixedSample(_cursorChannel, _cursorPosition, -1); }   
            }
          break;
        // set pitch from fader          
        case Sucofunkey::ENCODER_3_PUSH:
            if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
                _song->setPitchByMidiNote(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(35, 85)));                
                _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);

                if (!_isPlaying) { _play->playMixedSample(_cursorChannel, _cursorPosition, -1); }
            }
            break;            
        
        // set probability from fader          
        case Sucofunkey::ENCODER_4_PUSH:
            if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
                _song->setProbability(_cursorChannel, _cursorPosition, static_cast<byte>(_keyboard->getFaderValue(0, 100)));
                _sequencerScreen.updateSampleInfoProbability(_cursorChannel, _cursorPosition);
            }
          break;
      }          
    }


    // -------------------------------------------------------------------------
    // NOTE
    // -------------------------------------------------------------------------


    if (event.type == Sucofunkey::KEY_NOTE && event.pressed) {
      // check if blackKeys menu is active and pass the key to it
      if (_currentMenuState == MENU_BLACKKEY) {
        if ((_currentSequencerState == SELECT_SNIPPET_SLOT || _currentSequencerState == SNIPPET_WAIT_DELETE) && _keyboard->isEventWhiteKey(event.index)) {
          if (_selection.isActive()) {
            // save snippet to slot
            _snippets->saveSelectionToSlot(&_selection, _keyboard->getWhiteKeyByEventKey(event.index));            
            setSequencerState(SELECT_SNIPPET_SLOT); // to deselect the snippet option
            saveToSD();
          } else {
            // deleting a snippet slot
            int s = _snippets->cursorInSnippet(_cursorPosition, _cursorChannel);
            if (s != -1 && s == _keyboard->getWhiteKeyByEventKey(event.index)) {
              if (_snippets->deleteSnippetFromSlot(s)) {
                setSequencerState(SNIPPET_WAIT_DELETE); // deactivate snippet indicator..
                saveToSD();
              }              
            }
          }          
        } else {
          _blackKeyMenu.handleEvent(event);
        }                
        return;
      }

      byte sampleId1 = _keyboard->getSampleIdByEventKey(event.index);
        
      if (_currentMenuState == MENU_PIANO) {
        
        // ToDo: basenote setzen FN+NoteKey? -> eher in sampler??? -> da wird dann ja auch ADSR Hüllkurve gesetzt..

        if (_song->isSomethingAt(_cursorChannel, _cursorPosition)) {
          // set reference as origin in piano keyboard
          if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE) {
            // play sample
            _pianoKeyboard.setOriginReference(_cursorChannel, _cursorPosition);
            _song->setPitchByMidiNote(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition(), (_keyboard->getBank()*24)+sampleId1+4);
            _play->playMixedSample(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition(), -1);
            _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);
          }

          if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::MIDINOTE) {
            // play midi note
            _pianoKeyboard.setOriginReference(_cursorChannel, _cursorPosition);
            _song->setPitchByMidiNote(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition(), (_keyboard->getBank()*24)+sampleId1+4);
            _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_ON, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).note, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).velocity, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).channel);
            _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);
          }          
        } else {
          // empty cell -> copy piano origin, if available
          if (_pianoKeyboard.hasOrigin()) {
            if (_song->getPosition(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition()).type == SongStructure::SAMPLE) {
              // play sample
              _song->copyPosition(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition(), _cursorChannel, _cursorPosition, false);
              _song->setPitchByMidiNote(_cursorChannel, _cursorPosition, (_keyboard->getBank()*24)+sampleId1+4);
              _play->playMixedSample(_cursorChannel, _cursorPosition, -1);
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            }

            if (_song->getPosition(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition()).type == SongStructure::MIDINOTE) {
              // play midi note
              _song->copyPosition(_pianoKeyboard.getOriginChannel(), _pianoKeyboard.getOriginPosition(), _cursorChannel, _cursorPosition, false);
              _song->setPitchByMidiNote(_cursorChannel, _cursorPosition, (_keyboard->getBank()*24)+sampleId1+4);              
              _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_ON, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).note, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).velocity, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).channel);              
              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            }

          }
        }

      } else {
        // set sample at cursor position        
        if (_play->loadSetPlay(_keyboard->getBank(), sampleId1, _cursorChannel, _cursorPosition)) {
          _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
          _sequencerScreen.drawSample(_cursorChannel, _cursorPosition, false);
        }

      }
    }

    // note key release
    if (event.type == Sucofunkey::KEY_NOTE && !event.pressed) {
        if (_currentMenuState == MENU_PIANO) {
          // stop midi note                
          if (_song->isSomethingAt(_cursorChannel, _cursorPosition)) {
            if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::MIDINOTE) {
              _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_OFF, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).note, 0, _song->getMidiNoteFromBucketId(_song->getPosition(_cursorChannel, _cursorPosition).typeIndex).channel);
            }
          } 
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


    // -------------------------------------------------------------------------
    // ENCODERS
    // -------------------------------------------------------------------------

    if (event.type == Sucofunkey::ENCODER) {
      SongStructure::pointerType t = _song->getPosition(_cursorChannel, _cursorPosition).type;
      
      switch (event.index) {
        case Sucofunkey::FN_ENCODER_1:
            if (_isPlaying) {
              _song->setPlayBackSpeed(_song->getPlayBackSpeed() + (event.pressed ? 0.5 : -0.5));
              _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
              _sequencerScreen.drawBPM(_song->getPlayBackSpeed());
            } else {
              // was: move cursor on channels -> useless?
              // moveCursor(event.pressed ? DOWN : UP);
            }
          break;


        case Sucofunkey::FN_ENCODER_2:            
            /* disabled scrolling the plane, as it might be useless.. free the encoder for something more useful ;) (btw: it was on encoder 3)
            // scroll whole grid
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);
            
            if (event.pressed && _sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.SCROLL_RIGHT)) _sequencerScreen.drawGrid(_sequencerScreen.SCROLL_RIGHT);
            if (!event.pressed && _sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.SCROLL_LEFT)) _sequencerScreen.drawGrid(_sequencerScreen.SCROLL_LEFT);
            
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            */
          break;


        case Sucofunkey::FN_ENCODER_3:
            if (_isPlaying) {
              // what will be the function?
            } else {
              moveCursor(event.pressed ? RIGHT : LEFT);
            }
          break;

        // enhance/shorten grid
        case Sucofunkey::FN_ENCODER_4:
            if (!_isPlaying) {
              // 4 is one tick at zoom level NORMAL.. needed to keep everything in shape when moving on different zoom levels
              if (_song->changeSongLengthByTick(event.pressed, 4)) {
                // ToDo: find another solution without that much redrawing/flickering
                _cursorPosition = _song->getSongLength()-zoom.getZoomlevelOffset();
                _sequencerScreen.initializeGrid(_song, _cursorPosition);                
                _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
              } else {
                // ToD: max or min song length is reached.. inform user or leave it as it is???
              }   
            }         
          break;

          // --------------------------------------------------------

          case Sucofunkey::ENCODER_1:             
            if ( t == SongStructure::SAMPLE || t == SongStructure::PARAMETER_CHANGE_SAMPLE || t == SongStructure::MIDINOTE) {
              if (event.pressed) {
                _song->increaseVelocity(_cursorChannel, _cursorPosition);
              } else {
                _song->decreaseVelocity(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoVolume(_cursorChannel, _cursorPosition);              
            } else {
              // change playback speed (BPM) when nothing is selected.. same as FN+ENCODER_1 while playing
              _song->setPlayBackSpeed(_song->getPlayBackSpeed() + (event.pressed ? 0.5 : -0.5));
              _playbackTickSpeed = _play->calculatePlaybackTickSpeed();
              _sequencerScreen.drawBPM(_song->getPlayBackSpeed());
            }
            break;
          case Sucofunkey::ENCODER_2:
              if (event.pressed) {
                if ( t == SongStructure::SAMPLE || t == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                  _song->stereoPositionTickRight(_cursorChannel, _cursorPosition);
                }
                if ( t == SongStructure::MIDINOTE) {
                  _song->increaseMidiChannel(_cursorChannel, _cursorPosition);
                }                
              } else {
                if ( t == SongStructure::SAMPLE || t == SongStructure::PARAMETER_CHANGE_SAMPLE) {
                  _song->stereoPositionTickLeft(_cursorChannel, _cursorPosition);
                }
                if ( t == SongStructure::MIDINOTE) {
                  _song->decreaseMidiChannel(_cursorChannel, _cursorPosition);
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
                _song->increasePitchByOne(_cursorChannel, _cursorPosition);
              } else {
                _song->decreasePitchByOne(_cursorChannel, _cursorPosition);
              }

              _sequencerScreen.updateSampleInfoPitch(_cursorChannel, _cursorPosition);              
            }
            break;
          case Sucofunkey::ENCODER_4:
            if ( t == SongStructure::SAMPLE || t == SongStructure::MIDINOTE) {
              if (event.pressed) {
                _song->increaseProbability(_cursorChannel, _cursorPosition);
              } else {
                _song->decreaseProbability(_cursorChannel, _cursorPosition);
              }
                _sequencerScreen.updateSampleInfoProbability(_cursorChannel, _cursorPosition);              
            }
            break;
      }
    }

    // -------------------------------------------------------------------------
    // Application Events from e.g. black key menue
    // -------------------------------------------------------------------------

    if (event.type == Sucofunkey::EVENT_APPLICATION) {
      switch (event.index) {
        // Selection SEL
        case Sucofunkey::BLACKKEY_NAV_ITEM1:
              setSequencerState(SELECTION);
          break;
        // Move a cell MOV
        case Sucofunkey::BLACKKEY_NAV_ITEM2:
              setSequencerState(MOVE_CELL);
          break;      
        // Duplicate (Copy & Paste) a cell  DBL
        case Sucofunkey::BLACKKEY_NAV_ITEM3:
              setSequencerState(DOUBLE_CELL);
          break;
        // Snippets  SNI
        case Sucofunkey::BLACKKEY_NAV_ITEM4:
              if (_selection.isActive()) {
                setSequencerState(SELECT_SNIPPET_SLOT);
              } else {
                setSequencerState(SNIPPET_WAIT_DELETE);
              }              
          break;
        // Sheets          
        case Sucofunkey::BLACKKEY_NAV_ITEM5:
          break;
        // SND -> Parameter Change and Note Off
        case Sucofunkey::BLACKKEY_NAV_ITEM6:  
            {
              // Parameter Change for velocity and panning or NOTE OFF
              SongStructure::samplePointerStruct sps = _song->getPosition(_cursorChannel, _cursorPosition);
              switch (sps.type) {
                case SongStructure::SAMPLE:
                  _song->removePosition(_cursorChannel, _cursorPosition);                  
                  _song->setParameterChange(_cursorChannel, _cursorPosition, {});
                  break;
                case SongStructure::NOTE_OFF:
                  _song->removePosition(_cursorChannel, _cursorPosition);
                  break;
                case SongStructure::PARAMETER_CHANGE_SAMPLE:
                    _song->removePosition(_cursorChannel, _cursorPosition);
                    _song->setNoteOff(_cursorChannel, _cursorPosition);
                  break;
                case SongStructure::UNDEFINED:
                    _song->setParameterChange(_cursorChannel, _cursorPosition, {});
                  break;
                default:
                    _song->removePosition(_cursorChannel, _cursorPosition);
                    _song->setParameterChange(_cursorChannel, _cursorPosition, {});
                  break;
              }

              _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            }
          break;
        
        // Instrument -> MIDI out
        case Sucofunkey::BLACKKEY_NAV_ITEM7:
          _song->setMidiNote(_cursorChannel, _cursorPosition, {});
          _sequencerScreen.drawSample(_cursorChannel, _cursorPosition, false);
          break;
        
        // Reverse          
        case Sucofunkey::BLACKKEY_NAV_ITEM8:
          _song->reverseSamplePlayback(_cursorChannel, _cursorPosition);
          _sequencerScreen.drawSample(_cursorChannel, _cursorPosition, false);
          break;          
        
        case Sucofunkey::BLACKKEY_NAV_ITEM9:
          break;          

        // Clear selection/sketch
        case Sucofunkey::BLACKKEY_NAV_ITEM10:
          if (_currentSequencerState == SELECTION) {
            // clear selection
            _song->clearSelection(_selection, zoom.getZoomlevelOffset()-1);
            _sequencerScreen.drawGrid(_sequencerScreen.INIT);
            _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
            setSequencerState(SELECTION); // to deselect the selection..
          } else {
            // CLS -> deletes all notes from song
            // ToDo: Maybe display a confirmation/warning/notification message
            setSequencerState(CONFIRM_CLS); 
          }          
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
              _cursorPosition = _cursorPosition < _song->getSongLength()-zoomlevelOffset ? _cursorPosition+zoomlevelOffset : _song->getSongLength()-zoomlevelOffset;
              if (_sequencerScreen.viewportCheckUpdate(_cursorChannel, _cursorPosition, _sequencerScreen.RIGHT)) _sequencerScreen.drawGrid(_sequencerScreen.RIGHT);
              break;          
    default:
              break;
  }

  _song->setCurrentPosition(_cursorChannel, _cursorPosition, false);
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);

  // are we creating a selection? change end position
  if (_selection.isActive()) {
    _selection.setEnd(_cursorPosition + zoom.getZoomlevelOffset()-1, _cursorChannel);

    // draw frame of selection
    _sequencerScreen.drawSelection(&_selection);
  }

  // cancel SNI (Snippet delete) mode
  if (_currentSequencerState == SNIPPET_WAIT_DELETE) {
    setSequencerState(NORMAL);
  }
};


boolean Sequencer::setMenuState(MenuState state) {
  switch (state) {
    case MENU_NONE:
      if (_currentMenuState == MENU_BLACKKEY && (_currentSequencerState == NORMAL || _currentSequencerState == SNIPPET_WAIT_DELETE)) {
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
        // ToDo: add for midi, too?

        if (_song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::SAMPLE || _song->getPosition(_cursorChannel, _cursorPosition).type == SongStructure::MIDINOTE) {
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


// -------------------------------------------------------------------------
//  STATES 
// -------------------------------------------------------------------------


void Sequencer::setSequencerState(SequencerState state) {
   int s;

   if (state == _currentSequencerState) {
    // use this to do something, if the same state is set again (e.g. toggle something)
    switch(state) {
      case MOVE_CELL:          
          // move cell to here..
          if (!_savedCursorEqualsCurrent()) {
            _song->movePosition(_savedCursorChannel, _savedCursorPosition, _cursorChannel, _cursorPosition);
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
            if (_cursorPosition+zoom.getZoomlevelOffset() < _song->getSongLength() && !_song->isSomethingAt(_cursorChannel, _cursorPosition + zoom.getZoomlevelOffset())) {
              
              if(_song->copyPosition(_savedCursorChannel, _savedCursorPosition, _savedCursorChannel, _savedCursorPosition + zoom.getZoomlevelOffset(), false)) {
                moveCursor(RIGHT);
                _sequencerScreen.drawSample(_savedCursorChannel, _savedCursorPosition, true);
                _sequencerScreen.drawSample(_cursorChannel, _cursorPosition, true);
              };
            }            
          } else {
            // drop to the cursor position
            _song->copyPosition(_savedCursorChannel, _savedCursorPosition, _cursorChannel, _cursorPosition, false);
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
      case SELECTION:
          _selection.setEnd(_cursorPosition + zoom.getZoomlevelOffset()-1, _cursorChannel);
          _selection.setSelectionActive(false);
          _blackKeyMenu.setExclusiveAction(1, false);          
          _currentSequencerState = NORMAL;
          _sequencerScreen.drawSelection(&_selection); // clears the selection and redraws the grid
          break;
      case SELECT_SNIPPET_SLOT:
          _snippets->hideFreeSlots();
          _currentSequencerState = NORMAL;
          _blackKeyMenu.setExclusiveAction(4, false);

          // same as SELECTION as a second time.. disable selection
          _selection.setSelectionActive(false);
          _blackKeyMenu.setExclusiveAction(1, false);          
          _currentSequencerState = NORMAL;
          _sequencerScreen.drawSelection(&_selection); // clears the selection and redraws the grid          
          break;

      case SNIPPET_WAIT_DELETE:
          _snippets->hideFreeSlots();
          _blackKeyMenu.setExclusiveAction(4, false);
          _currentSequencerState = NORMAL;          
          _sequencerScreen.drawGrid(SequencerScreen::SNIPPET_DELETE);
          _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
          break;

      default:
          break;
    }
  } else {
    // time for some kind of constructor for the new state
    switch(state) {
      case NORMAL:
        if (_selection.isActive()) {
          _selection.setSelectionActive(false);
          _sequencerScreen.drawSelection(&_selection); // clears the selection and redraws the grid
        }

        if (_currentSequencerState == SELECT_SNIPPET_SLOT || _currentSequencerState == SNIPPET_WAIT_DELETE) {
          _snippets->hideFreeSlots();
          _blackKeyMenu.setExclusiveAction(4, false);
        }
        _currentSequencerState = NORMAL; 
        break;

      case MOVE_CELL:
        if (_song->isSomethingAt(_cursorChannel, _cursorPosition)) {
          _saveCurrentCursorPosition();
          _blackKeyMenu.setExclusiveAction(2, true);
          _currentSequencerState = MOVE_CELL;
        } else {
          _currentSequencerState = NORMAL;
        }
        break;

      case DOUBLE_CELL:
        if (_song->isSomethingAt(_cursorChannel, _cursorPosition)) {
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

      case SELECTION:
        _blackKeyMenu.setExclusiveAction(1, true);  
        _blackKeyMenu.allowAdditionalToExclusive(4); // allow saving selection as snippet
        _blackKeyMenu.allowAdditionalToExclusive(10); // allow clearing the selection
        _selection.setStart(_cursorPosition, _cursorChannel);
        _selection.setEnd(_cursorPosition + zoom.getZoomlevelOffset()-1, _cursorChannel);
        _selection.setSelectionActive(true);
        _currentSequencerState = SELECTION;        
        _sequencerScreen.drawSelection(&_selection);
        break;

      case SELECT_SNIPPET_SLOT:
        _blackKeyMenu.setExclusiveAction(4, true);
        _blackKeyMenu.disableExceptions();
        _snippets->showFreeSlots();
        _currentSequencerState = SELECT_SNIPPET_SLOT;
        break;

      case SNIPPET_WAIT_DELETE:        
        s = _snippets->cursorInSnippet(_cursorPosition, _cursorChannel);
        if (s != -1) {
          _blackKeyMenu.setExclusiveAction(4, true);
          _blackKeyMenu.disableExceptions();
          // highlight snippet LED
          _snippets->highlightSnippetSlot(s, true);
          _currentSequencerState = SNIPPET_WAIT_DELETE;
        } else {
          _currentSequencerState = NORMAL;
          _snippets->hideFreeSlots();
        };
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


// ------------------------------------------------------------------------------------------------------------------------------------
// --- PLAY ---------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------


void Sequencer::playSelection() {
   _isPlaying = !_isPlaying;

  if (!_isPlaying) {
    stopAllChannels();    
  } else {
    _playerPosition = _selection.getNormalizedSelection().startX;
    _blinkPosition = _playerPosition;
  }
}

void Sequencer::playSnippet(byte slot) {
   _isPlaying = !_isPlaying;

  if (!_isPlaying) {
    stopAllChannels();
    _isSnippetPlaying = false;
  } else {
    _isSnippetPlaying = true;
    _playingSnippet = slot;
    _playerPosition = _song->getSnippet(slot).startX;
    //_playerPosition = _selection.getNormalizedSelection().startX;
    _blinkPosition = _playerPosition;
  }
}


void Sequencer::playSong() {
   _isPlaying = !_isPlaying;

  if (!_isPlaying) { 
    stopAllChannels();

    // song was paused (2x play), next play will start at cursor position
    _nextPlayStartAtSheet = true;

    _playerPosition = _cursorPosition;
    _blinkPosition = _cursorPosition;

    _sequencerScreen.drawPlayStepIndicator(_playerPosition, false); 
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
    // autosave
    saveToSD();
  } else {
    if (_nextPlayStartAtSheet) {
      _sheetStart = _song->getSheetStartForPosition(_cursorPosition);
      _sheetEnd = _song->getSheetEndForPosition(_cursorPosition);
      
      _playerPosition = _sheetStart;
      _blinkPosition = _playerPosition;
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
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, false);
    _keyboard->setLEDState(Sucofunkey::LED_PLAY, false);
  } else {
    // ToDo: draw cursor somewhere ;)
  }
  stopAllChannels();
  _nextPlayStartAtSheet = false;
  _playerPosition = 0;
  _isSnippetPlaying = false;
}


void Sequencer::_playNext() {
    _sequencerScreen.drawPlayStepIndicator(_playerPosition, true);

    SongStructure::samplePointerStruct sps;

    int channelStart = 0;
    int channelEnd = 7;
    int positionStart = 0;
    int positionEnd = _song->getSongLength()-1;
    Selection::SelectionStruct _normalizedSelection;

    // play only selection, if we are selecting something
    if (_selection.isActive()) {
      _normalizedSelection = _selection.getNormalizedSelection();      
      channelStart = _normalizedSelection.startY;
      channelEnd = _normalizedSelection.endY;
      positionStart = _normalizedSelection.startX;
      // add the rest that is underlying.. e.g. 1...2...3...4... -> Selection on 3 -> 3... needs to be selected to have a full gridcell.. depending on zoom level.
      positionEnd = _normalizedSelection.endX;
    } else {
      if (_isSnippetPlaying && _playingSnippet > 0 && _playingSnippet <= 14) {
        channelStart =  _song->getSnippet(_playingSnippet).startY;
        channelEnd = _song->getSnippet(_playingSnippet).endY;
        positionStart = _song->getSnippet(_playingSnippet).startX;
        positionEnd = _song->getSnippet(_playingSnippet).endX;
      }
    }


    for (int c=channelStart; c <= channelEnd; c++) {
      sps = _song->getPosition(c, _playerPosition); 
      if (sps.type == SongStructure::MIDINOTE) {
        
        if (random(100) <= _song->getMidiNoteFromBucketId(sps.typeIndex).probability) {
          if (_song->getMidiNoteFromBucketId(sps.typeIndex).velocity > 0) {
            _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_ON, _song->getMidiNoteFromBucketId(sps.typeIndex).note, _song->getMidiNoteFromBucketId(sps.typeIndex).velocity, _song->getMidiNoteFromBucketId(sps.typeIndex).channel);
          } else {
            _keyboard->addApplicationEventWithDataToQueue(Sucofunkey::MIDI_SEND_NOTE_OFF, _song->getMidiNoteFromBucketId(sps.typeIndex).note, 0, _song->getMidiNoteFromBucketId(sps.typeIndex).channel);
          }      
        }        
      } else {
        if (sps.type != SongStructure::UNDEFINED) {
          _play->playMixedSample(c, _playerPosition, -1);    
        }      
      } 
    }

    if (_playerPosition == (_nextPlayStartAtSheet ? _sheetEnd : positionEnd)) {
      _playerPosition = (_nextPlayStartAtSheet ? _sheetStart : positionStart);
      _sequencerScreen.drawPlayStepIndicator(_playerPosition, false);
      _blinkPosition = _playerPosition;
    } else {
      _playerPosition++;
    } 
}


void Sequencer::stopAllChannels() {
  _playLEDon = false;
  _play->stopAllChannels();
}

boolean Sequencer::isPlaying() {
  return _isPlaying;
}




void Sequencer::loadFromSD(boolean drawGrid) {
  // load from SD card
  _song->loadFromSD(_sfsio->getSongPath());  
  _cursorPosition = 0;

  // redraw the grid
  if (drawGrid) {
    _sequencerScreen.initializeGrid(_song, _cursorPosition);
    _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);

    // set right position in song structure
    _song->setCurrentPosition(_cursorChannel, _cursorPosition, true);
  }

  // loading samples to extmem
  _sfsio->loadSamplesToMemory(_song->getSamplesUsed());
  
  if (drawGrid) {
    _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
  }
};

void Sequencer::saveToSD() {
  _song->saveToSD(_sfsio->getSongPath());
};

void Sequencer::debugInfos() {
  _song->debugInfos();
};



void Sequencer::_clearSong() {
  // 1 empty sample ram
  _sfsio->clearSampleMemory();

  // 2 clear all data structures or initialize it new
  _song->clearSelection(0, 0, 7, _song->getSongLength());

  // 3 set defaults for meta data
  _song->setMetadataToDefault();

  // 4 redraw grid
  _cursorPosition = 0;
  _cursorChannel = 0;

  _sequencerScreen.initializeGrid(_song, _cursorPosition);
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
  _song->setCurrentPosition(_cursorChannel, _cursorPosition, true);       
  _sequencerScreen.drawExtMemPercentage(_sfsio->getExtmemUsagePercent());
}


void Sequencer::_jumpToPreviousSheet() {  
  _jumpToPosition(_song->getPreviousSheetDividerPosition(_cursorPosition-1));   
}

void Sequencer::_jumpToNextSheet() {
  _jumpToPosition(_song->getNextSheetDividerPosition(_cursorPosition));   
}

void Sequencer::_jumpToPosition(uint16_t position) {
  if (position >= _song->getSongLength()) return;

  // cancel SNI (Snippet delete) mode
  if (_currentSequencerState == SNIPPET_WAIT_DELETE) {
    setSequencerState(NORMAL);
  }

  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, false);

  _cursorPosition = position;  
  _song->setCurrentPosition(_cursorChannel, _cursorPosition, false);

  _sequencerScreen.drawGridAtPosition(position);
  _sequencerScreen.drawCursorAt(_cursorChannel, _cursorPosition, true);
}


