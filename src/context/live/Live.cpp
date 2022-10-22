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
#include "../../helper/SampleFSIO.h"
#include "../../gui/Screen.h"
#include "Live.h"
#include "MIDI.h"

Live::Live(Sucofunkey* keyboard, Screen* screen, FSIO* fsio, SampleFSIO* sfsio, Play* play) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _play = play;
    _liveScreen = LiveScreen(keyboard, screen, sfsio);
    _bpm = _play->getSong()->getPlayBackSpeed();

    for (int i=0; i<72; i++) {
      _LEDHighlightSlots[i] = false;
    }

    for (int i=0; i<128; i++) {
      _midiNoteToSlot[i] = -1;
    }
}

// returns the current tick speed.. as tempo changes are not handled global
long Live::receiveTimerTick() {
  if (_isInitialized) {
    _play->snippetsPlayNext();  
  
    if (_blinkPosition == 0) {
      _playLEDon = true;
    } else {
      _playLEDon = false;
    }

    _keyboard->setLEDState(Sucofunkey::LED_PLAY, _playLEDon);
  }

  if (_blinkPosition < (_play->getSong()->getSongResolution()*4)-1) {
    _blinkPosition++;
  } else {
    _blinkPosition = 0;
  }

  return _playbackTickSpeed;
}



void Live::handleEvent(Sucofunkey::keyQueueStruct event) {
    _editingSlotId = (_editingSlotBank-1)*24+_editingSlotKey-1; // 0..71

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT:        
          if (_currentState == OVERVIEW) {
            _activeBank = _keyboard->setBankDown();
            _overview(false);
          }

          if (_currentState == SLOT_TYPE_SELECT) {
            _slots[_editingSlotId].type = Play::SNIPPET;
            _handleSlotTypeSelection(_editingSlotBank, _editingSlotKey, false);
          }

          if (_currentState == WAIT_SAMPLE_SELECT) {
            _sampleBank = _keyboard->setBankDown();
          }

          if (_currentState == PIANO) {
            _pianoBank = _keyboard->setBankDown();
          }

          break;

        case Sucofunkey::CURSOR_RIGHT:
          if (_currentState == OVERVIEW) {
            _activeBank = _keyboard->setBankUp();
            _overview(false);
          }

          if (_currentState == SLOT_TYPE_SELECT) {
            _slots[_editingSlotId].type = Play::SAMPLE;
            _handleSlotTypeSelection(_editingSlotBank, _editingSlotKey, false);
          }

          if (_currentState == WAIT_SAMPLE_SELECT) {
            _sampleBank = _keyboard->setBankUp();
          }

          if (_currentState == PIANO) {
            _pianoBank = _keyboard->setBankUp();
          }

          break;

        case Sucofunkey::SET:
          if (_currentState == CONFIG_SNIPPET || _currentState == CONFIG_SAMPLE) {
            // save midi in note for slot
            if(_saveAndActivateMIDINoteForSlot(_editingSlotId)) {
              saveConfig();
            }
            _cancel();
          }

          if (_currentState == WAIT_SNIPPET_SELECT) {
            if (_slots[_editingSlotId].snippet == -1) break;

            _changeState(CONFIG_SNIPPET);
            // set default midi in note
            _slots[_editingSlotId].midiNote = _editingSlotId + 29;
            _liveScreen.showSnippetConfig(_slots[_editingSlotId]);
            break;
          }


          if (_currentState == WAIT_SAMPLE_SELECT) {
            if (_slots[_editingSlotId].sampleNumber == 255) break;

            _changeState(CONFIG_SAMPLE);
            // set default midi in note
            _slots[_editingSlotId].midiNote = _editingSlotId + 29;
            _liveScreen.showSampleConfig(_slots[_editingSlotId]);
            break;
          }


          if (_currentState == SLOT_TYPE_SELECT) {
            if (_slots[_editingSlotId].type == Play::SNIPPET) { _changeState(WAIT_SNIPPET_SELECT); };
            if (_slots[_editingSlotId].type == Play::SAMPLE) { _changeState(WAIT_SAMPLE_SELECT); };
            break;
          }

          break;

        case Sucofunkey::FN_SET:
          // remove piano flag from sample
          if (_currentState == PIANO) {
            _slots[_pianoSampleSlotIndex].isPiano = false;
            
            // is piano sample in active bank? redraw it with the sample color
            int from = (_activeBank-1)*24;
            int to = from + 23;

            if (_pianoSampleSlotIndex >= from && _pianoSampleSlotIndex <= to) {
              _liveScreen.drawOverviewSlot(_slots[_pianoSampleSlotIndex], (_pianoSampleSlotIndex - from + 1));
            }

            _pianoSampleSlotIndex = -1;
            _pianoBank = 2;
            _cancel(false);
            break;
          }         

          if (_currentState == CONFIG_SNIPPET || _currentState == CONFIG_SAMPLE) {
            _removeSlot(_editingSlotId);
          } else {
            _cancel();
          }          
          break;

        case Sucofunkey::ZOOM:
          if (_currentState == OVERVIEW) {
            if (_pianoSampleSlotIndex == -1) {
              // select piano sample
              _changeState(WAIT_PIANO_SAMPLE_SELECT);
            } else {
              _changeState(PIANO);
            }
          } else {
            if (_currentState == PIANO || _currentState == WAIT_PIANO_SAMPLE_SELECT) {
              _cancel(false);
            }
          }
          break;

        case Sucofunkey::FN_ZOOM:
          break;

        case Sucofunkey::PLAY:
            if (_currentState == CONFIG_SAMPLE) {
              _playSlot(_editingSlotId, 128, true, 128);
            }
          break;        
          break;

        case Sucofunkey::PAUSE:
            if (_currentState == CONFIG_SAMPLE) {
              _playSlot(_editingSlotId, 128, false, 128);
            }        
          break;

        case Sucofunkey::ENCODER_1_PUSH:
          _handleEncoders(1, false, 0);
          break;
        case Sucofunkey::FN_ENCODER_1_PUSH:
          _handleEncoders(1, true, 0);
          break;
        case Sucofunkey::ENCODER_2_PUSH:
          _handleEncoders(2, false, 0);
          break;
        case Sucofunkey::FN_ENCODER_2_PUSH:
          _handleEncoders(2, true, 0);
          break;
        case Sucofunkey::ENCODER_3_PUSH:
          _handleEncoders(3, false, 0);
          break;
        case Sucofunkey::FN_ENCODER_3_PUSH:
          _handleEncoders(3, true, 0);
          break;
        case Sucofunkey::ENCODER_4_PUSH:
          _handleEncoders(4, false, 0);
          break;
        case Sucofunkey::FN_ENCODER_4_PUSH:
          _handleEncoders(4, true, 0);
          break;

        default:
          break;                  
      }      
    }



    if (event.type == Sucofunkey::KEY_NOTE) {
      int slot = _keyboard->getSampleIdByEventKey(event.index);
      boolean tempBool;
      int tempInt;
      int slotsIndex = (slot-1)+((_activeBank-1)*24); 
      
      switch (_currentState) {
        case SLOT_TYPE_SELECT:
          if (event.pressed) _cancel();
          break;
        
        case OVERVIEW:
            if (_slots[slotsIndex].type != Play::EMPTY) {
              _playSlot(slotsIndex, 128, event.pressed, 128);
            } else {              
              _changeState(SLOT_TYPE_SELECT);
              _handleSlotTypeSelection(_activeBank, slot, true);
            }

          break;

        case WAIT_SNIPPET_SELECT:
          tempBool = _keyboard->isEventWhiteKey(event.index);
          if (tempBool) {
            tempInt = _keyboard->getWhiteKeyByEventKey(event.index);

            if (event.pressed) {
              _slots[_editingSlotId].snippet = tempInt;
              _liveScreen.showSelectSnippet(_slots[_editingSlotId], false);
              _play->queueSnippet(tempInt, false, false); // play for prelisten              
            } else {
              _play->unqueueSnippet(tempInt); // stop prelisten
            }
          }
          break;

        case WAIT_SAMPLE_SELECT:        
            if (event.pressed) {
              _slots[_editingSlotId].sampleNumber = _keyboard->getSampleIdByEventKey(event.index) + ((_sampleBank-1)*24);
              _liveScreen.showSelectSample(_slots[_editingSlotId], false);
               // play for prelisten
               _play->_audioResources->playSdRaw.play(_sfsio->sampleFilename[_sampleBank-1][_keyboard->getSampleIdByEventKey(event.index)-1]);
            } else {
               // stop prelisten
              _play->_audioResources->playSdRaw.stop();
            }

          break;
        
        case CONFIG_SAMPLE:
            if (_slots[slotsIndex].type != Play::EMPTY) {
              _playSlot(slotsIndex, 128, event.pressed, 128);
            }            
          break;
        
        case WAIT_PIANO_SAMPLE_SELECT:
          if (event.pressed) {
            if (_slots[slotsIndex].type == Play::SAMPLE) {
              if (_pianoSampleSlotIndex != -1) {
                _slots[_pianoSampleSlotIndex].isPiano = false;
              }

              _pianoSampleSlotIndex = slotsIndex;
              _slots[slotsIndex].isPiano = true;
              _liveScreen.drawOverviewSlot(_slots[_pianoSampleSlotIndex], slot);
              _changeState(PIANO);
              }
          } 
          break;

        case PIANO:
          _playPiano((_pianoBank-1)*24 + slot + 28, 128, event.pressed);
          break;

        default:
          break;  
      }
    }


    if (event.type == Sucofunkey::KEY_FN_NOTE && event.pressed) {
      int slot = _keyboard->getSampleIdByEventKey(event.index);
      int slotsIndex = (slot-1)+((_activeBank-1)*24);

      // show slot configuration
      //if (_currentState == OVERVIEW && _slots[slotsIndex].type == Play::SNIPPET) {
      if (_slots[slotsIndex].type == Play::SNIPPET) {
        if (_currentState != OVERVIEW) { _cancel(false); }

        _editingSlotKey = slot;
        _editingSlotBank = _activeBank;
        _changeState(CONFIG_SNIPPET);
      }
    

      // show sample configuration
      //if (_currentState == OVERVIEW && _slots[slotsIndex].type == Play::SAMPLE) {
      if (_slots[slotsIndex].type == Play::SAMPLE) {
        if (_currentState != OVERVIEW) { _cancel(false); }

        _editingSlotKey = slot;
        _editingSlotBank = _activeBank;
        _changeState(CONFIG_SAMPLE);
      }
    }

    if (event.type == Sucofunkey::ENCODER) {
      switch (event.index) {
        case Sucofunkey::ENCODER_1:
          _handleEncoders(1, false, event.pressed ? 1 : -1);
          break;
        case Sucofunkey::ENCODER_2:
          _handleEncoders(2, false, event.pressed ? 1 : -1);
          break;
        case Sucofunkey::ENCODER_3:
          _handleEncoders(3, false, event.pressed ? 1 : -1);
          break;
        case Sucofunkey::ENCODER_4:
          _handleEncoders(4, false, event.pressed ? 1 : -1);
          break;
        case Sucofunkey::FN_ENCODER_1:
          _handleEncoders(1, true, event.pressed ? 1 : -1);
          break;
        case Sucofunkey::FN_ENCODER_2:
          _handleEncoders(2, true, event.pressed ? 1 : -1);
          break;
        case Sucofunkey::FN_ENCODER_3:
          _handleEncoders(3, true, event.pressed ? 1 : -1);
          break;
        case Sucofunkey::FN_ENCODER_4:
          _handleEncoders(4, true, event.pressed ? 1 : -1);
          break;
        default:
          break;
      }          
    }
}

void Live::setActive(boolean active) {
  if (active) {
    _isActive = true;    
    _keyboard->setBank(_activeBank);
    loadConfig();
    _changeState(OVERVIEW);
    _overview(true);
    // might have changed in sequencer
    _play->checkIfAllSamplesAreLoaded();
    _isInitialized = true;
    _playbackTickSpeed = _play->calculatePlaybackTickSpeed();    
  } else {
    _isActive = false;
    _isInitialized = false;
    _keyboard->setBank(0);
  }
}


void Live::receiveMidiData(byte channel, midi::MidiType type, int d1, int d2) {
  switch (type) {
    case midi::MidiType::NoteOn:
      if (channel == 1) {
        if (_currentState == WAIT_MIDI_TRAINING_INPUT_SNIPPET) {        
          _liveScreen.updateSnippetConfig(_slots[_editingSlotId], 4, false, (_setMIDINote(d1) == true ? LiveScreen::MIDI_NOTE_FREE : LiveScreen::MIDI_NOTE_IN_USE)); // Encoder 4 = MIDI IN
        } else {
          if (_midiNoteToSlot[d1] != -1) {
            // d2 = velocity
            _playSlot(_midiNoteToSlot[d1], d2, true, 128);
          } 
        }
      } 

      if (channel == 2) {
        _playPiano(d1, d2, true);
      }

      break;
    case midi::MidiType::NoteOff:

     if (channel == 1) {
      if (_midiNoteToSlot[d1] != -1 && _currentState != WAIT_MIDI_TRAINING_INPUT_SNIPPET && _currentState != WAIT_MIDI_TRAINING_INPUT_SAMPLE) {
        _playSlot(_midiNoteToSlot[d1], d2, false, 128);
      }
     } 

     if (channel == 2) {
        _playPiano(d1, d2, false);
     }
    
     break;

    case midi::MidiType::AfterTouchPoly:      

      if (channel == 1 && _slots[_midiNoteToSlot[d1]].type == Play::SAMPLE) {
        _play->handlePolyphonicAftertouch(_slots[_midiNoteToSlot[d1]].sampleNumber, d2, _slots[_midiNoteToSlot[d1]].stereoPosition);
      }

      if (channel == 2) {
        _play->handlePolyphonicAftertouch(_slots[_pianoSampleSlotIndex].sampleNumber, d2, _slots[_pianoSampleSlotIndex].stereoPosition);
      }

      break;

    default:
      break;
  }
}


// updates the slot, but does not update the _midiToSlot Lookup table
// storing or restoring needs to be done on SET
boolean Live::_setMIDINote(byte note) {   
  
  // pay attention that the midi note cannot be higher than the midi range from 0 to 127
  if (note < 128) {
    _slots[_editingSlotId].midiNote = note;
  } else {
    _slots[_editingSlotId].midiNote = 127;
  }

  // slot is free
  if (_midiNoteToSlot[_slots[_editingSlotId].midiNote] == -1) {
    return true;
  } 

  if (_midiNoteToSlot[_slots[_editingSlotId].midiNote] == _editingSlotId) {
    return true;
  } 

  return false;
};

// returns 129, if there is no midi note assinged to the slot
byte Live::_getMidiNoteForSlot(int slotIndex) {
  for (int i=0; i<128; i++) {
    if (_midiNoteToSlot[i] == slotIndex) return i;
  }
  return 129;
};


boolean Live::_saveAndActivateMIDINoteForSlot(int slotIndex) {
  byte noteForSlot = _getMidiNoteForSlot(slotIndex);

  // case 1: no change  
  if (noteForSlot != 129 && noteForSlot == _slots[slotIndex].midiNote) return true;

  // case 2: selected note is free and will be taken
  if (_midiNoteToSlot[_slots[slotIndex].midiNote] == -1) {
    // remove current reference
    if (noteForSlot != 129) {
      _midiNoteToSlot[noteForSlot] = -1;
    }
    
    // set new reference
    _midiNoteToSlot[_slots[slotIndex].midiNote] = slotIndex;
    return true;
  } else {
    // case 3: selected note is already in use with another slot
    if (_midiNoteToSlot[_slots[slotIndex].midiNote] != slotIndex) {
      // restore old value..      
      if (noteForSlot != 129) {
        // .. if there is one ..
        _slots[slotIndex].midiNote = noteForSlot;
        return true;
      } else {
        // case 4: .. if not, try to set note correspponding to the slot (note of slot + 29 for initial F on the keyboard)
        if (_midiNoteToSlot[slotIndex+29] == -1) {
          _slots[slotIndex].midiNote = slotIndex+29;
          _midiNoteToSlot[slotIndex+29] = slotIndex;
          return true;
        }
      }
    }
  }

  return false;
};





void Live::_overview(boolean initialize) {
  if (initialize) _liveScreen.showEmptyOverview(true);

  int s = 1;

  for (int i=(_keyboard->getBank()-1)*24+1; i<=(_keyboard->getBank()-1)*24+24; i++) {
    // i = 1..72 in bunches of 24, depending on selected bank
    _liveScreen.drawOverviewSlot(_slots[i-1], s);
    s++;
  }

  _liveScreen.drawBPM(_bpm);
  _updateAllLoopingLEDs();
}


void Live::_updateAllLoopingLEDs() {
  for (int i=0; i<24; i++) {
    _keyboard->setLEDState(_keyboard->getLEDPinBySampleId(i+1), _LEDHighlightSlots[i+((_activeBank-1)*24)]);
  }
}



void Live::_changeState(LiveState state) {
  _editingSlotId = (_editingSlotBank-1)*24+_editingSlotKey-1;

  switch (state) {
    case OVERVIEW:
      _editingSlotBank = 0;
      _editingSlotKey = 0;      
      _keyboard->setBank(_activeBank);
      break;
    case WAIT_SNIPPET_SELECT:
      _liveScreen.showSelectSnippet(_slots[_editingSlotId], true);
      break;
    case WAIT_SAMPLE_SELECT:
      _liveScreen.showSelectSample(_slots[_editingSlotId], true);
      _keyboard->setBank(_sampleBank);
      break;
    case CONFIG_SNIPPET:
      _liveScreen.showSnippetConfig(_slots[_editingSlotId]);
      break;
    case CONFIG_SAMPLE:
      _liveScreen.showSampleConfig(_slots[_editingSlotId]);
      break;
    case WAIT_PIANO_SAMPLE_SELECT:
      _liveScreen.showPianoSampleSelectMessage(true);
      break;
    case PIANO:
      _liveScreen.showPianoMessage(true);
      _keyboard->setBank(_pianoBank);
    default:
      break;
  }
  
  _currentState = state;
};


// velocity == 128 -> take velocity from slot
void Live::_playSlot(int slotIndex, byte velocity, boolean pressed, byte note) {
  Play::LiveSlotDefinitionStruct slot = _slots[slotIndex];

  if (slot.type == Play::EMPTY) return;

  if (slot.type == Play::SNIPPET) {
    if (pressed) {
      if (slot.snippet != -1) {
        
        if (slot.loopSnippet) {
          if (!_LEDHighlightSlots[slotIndex]) {
            // start looping snippet
            _play->queueSnippet(slot.snippet, true, true);
            _LEDHighlightSlots[slotIndex] = true;
          } else {
            // stop looping
            if (slot.immediateStopOnRelease) {
              // immediately stop
              _play->unqueueSnippet(slot.snippet);
              _LEDHighlightSlots[slotIndex] = false;
            } else {
              // remove loop flag to stop snippet when it is played
              _play->removeLoopFlagFromSnippet(slot.snippet);
              _LEDHighlightSlots[slotIndex] = false;
            }
          }
        } else {
          // just queue a snippet to play once
          _play->queueSnippet(slot.snippet, true, false);
        }
        _updateAllLoopingLEDs();
      }  
    } else {
      // stop snippet, if it is playing only while holding the note key
      if (!slot.loopSnippet && slot.immediateStopOnRelease) {
        _play->unqueueSnippet(slot.snippet);
      }
    }
  }

  if (slot.type == Play::SAMPLE) {
    if (pressed) {
      // play sample    
      _play->playNextFreeMemory(slot.sampleNumber, velocity == 128 ? slot.velocity : velocity, slot.stereoPosition, slot.baseMidiNote, note == 128 ? slot.pitchedNote : note, slot.reverse, true);
    } else {
      if (slot.immediateStopOnRelease || _currentState == PIANO) {
        _play->playNextFreeMemory(slot.sampleNumber, 0, slot.stereoPosition, slot.baseMidiNote, slot.pitchedNote, slot.reverse, false);
      }
    }
  }

};

void Live::_handleSlotTypeSelection(byte bank, byte key, boolean initialize) {   
  int s = (bank-1)*24+key-1;

  if (initialize) {
    _editingSlotBank = bank; // 1..3
    _editingSlotKey = key; // 1..24

    _slots[s].type = Play::SNIPPET; // selected by default  
  }
    
  _liveScreen.showSlotTypeSelection(_slots[s], initialize);
}


void Live::_cancel() {
  _cancel(true);
}

void Live::_cancel(boolean showOverview) {
  switch (_currentState) {
    case SLOT_TYPE_SELECT:
    case WAIT_SNIPPET_SELECT:
    case WAIT_SAMPLE_SELECT:
      _slots[(_editingSlotBank-1)*24+_editingSlotKey-1].type = Play::EMPTY;
      _changeState(OVERVIEW);                      
      break;      
    case CONFIG_SNIPPET:
    case CONFIG_SAMPLE:
      _changeState(OVERVIEW);
      break;      
    case WAIT_PIANO_SAMPLE_SELECT:
      _liveScreen.showPianoSampleSelectMessage(false);
      _changeState(OVERVIEW);
      break;
    case PIANO:
      _liveScreen.showPianoMessage(false);
      _changeState(OVERVIEW);    
      break;
    default:
      break;
  }

  if (showOverview) { 
    _overview(true); 
  }  
}

void Live::_changeBPM(float bpm) {
  _bpm = bpm;
  _playbackTickSpeed = _play->calculatePlaybackTickSpeed(_bpm);

  if (_currentState == OVERVIEW) {
    _liveScreen.drawBPM(_bpm);
  }  
}


void Live::_handleEncoders(byte encoder, boolean function, int action) {
  switch (_currentState) {
    case OVERVIEW:
      _handleOverviewEncoders(encoder, function, action);
      break;
    case WAIT_MIDI_TRAINING_INPUT_SNIPPET:
    case CONFIG_SNIPPET:
      _handleSnippetConfiguration(encoder, function, action);
      break;
    case WAIT_MIDI_TRAINING_INPUT_SAMPLE:  
    case CONFIG_SAMPLE:
      _handleSampleConfiguration(encoder, function, action);
      break;
    default:
      break;
  };
}

// action 0 = push
void Live::_handleSnippetConfiguration(byte encoder, boolean function, int action) {
  
  if (!function) {
    switch (encoder) {
      case 1:
        if (!function) {
          // toggling play complete and stop on release
          _slots[_editingSlotId].immediateStopOnRelease = !_slots[_editingSlotId].immediateStopOnRelease;          
          _liveScreen.updateSnippetConfig(_slots[_editingSlotId], 1, (action == 0 ? true : false), LiveScreen::NONE);
        }
        break;
      case 2:
        if (!function) {
          _slots[_editingSlotId].loopSnippet = !_slots[_editingSlotId].loopSnippet;
          _liveScreen.updateSnippetConfig(_slots[_editingSlotId], 2, (action == 0 ? true : false), LiveScreen::NONE);
        } 
        break;
      case 4:
        if (action == 0) {
          // handle push -> Learn MIDI event -> triggering display is in receive midi functions
          if (_currentState == WAIT_MIDI_TRAINING_INPUT_SNIPPET) {
            // end waiting for note
            _liveScreen.drawMIDIinWaitForTraining(false);          
            _changeState(CONFIG_SNIPPET);
          } else {
            // start waiting for note
            _changeState(WAIT_MIDI_TRAINING_INPUT_SNIPPET);
            _liveScreen.drawMIDIinWaitForTraining(true);
          }
        } else {
          // handle encoder updates          
          boolean free = _setMIDINote(_slots[_editingSlotId].midiNote + action);
          _liveScreen.updateSnippetConfig(_slots[_editingSlotId], 4, (action == 0 ? true : false), free ? LiveScreen::MIDI_NOTE_FREE : LiveScreen::MIDI_NOTE_IN_USE);
        }
        
        break;
      default:
        break;                        
    }
  } else {
    // no FN+Encoder functionality planned yet
  }
}


void Live::_handleSampleConfiguration(byte encoder, boolean function, int action) {
  if (!function) {
    switch (encoder) {
      case 1:
        if (!function) {
          if (action == 0) {
            // toggling play complete and stop on release
            _slots[_editingSlotId].immediateStopOnRelease = !_slots[_editingSlotId].immediateStopOnRelease;          
            _liveScreen.updateSampleConfig(_slots[_editingSlotId], 1, true, LiveScreen::NONE);
          } else {
            if (action == -1 && _slots[_editingSlotId].velocity > 1) _slots[_editingSlotId].velocity -= 2;
            if (action == 1 && _slots[_editingSlotId].velocity < 126) _slots[_editingSlotId].velocity += 2;
            _liveScreen.updateSampleConfig(_slots[_editingSlotId], 1, false, LiveScreen::NONE);            
          }
        }
        break;
      
      case 2:
        if (!function) {
          if (action == 0) {
            _slots[_editingSlotId].reverse = !_slots[_editingSlotId].reverse;
            _liveScreen.updateSampleConfig(_slots[_editingSlotId], 2, (action == 0 ? true : false), LiveScreen::NONE);
          } else {
            if (action == -1 && _slots[_editingSlotId].stereoPosition > 1) _slots[_editingSlotId].stereoPosition -= 2;
            if (action == 1 && _slots[_editingSlotId].stereoPosition < 126) _slots[_editingSlotId].stereoPosition += 2;
            _liveScreen.updateSampleConfig(_slots[_editingSlotId], 2, false, LiveScreen::NONE);
          }
        } 
        break;
      
      case 3:
        if (!function) {
          if (action == 0) {
          } else {
            if (action == -1 && _slots[_editingSlotId].pitchedNote > 29) _slots[_editingSlotId].pitchedNote--;
            if (action == 1 && _slots[_editingSlotId].pitchedNote < 100) _slots[_editingSlotId].pitchedNote++;
            _liveScreen.updateSampleConfig(_slots[_editingSlotId], 3, false, LiveScreen::NONE);
          }
        } 
        break;

      case 4:
        if (action == 0) {
          // handle push -> Learn MIDI event
          if (_currentState == WAIT_MIDI_TRAINING_INPUT_SAMPLE) {
            // end waiting for note
            _liveScreen.drawMIDIinWaitForTraining(false);          
            _changeState(CONFIG_SAMPLE);
          } else {
            // start waiting for note
            _changeState(WAIT_MIDI_TRAINING_INPUT_SAMPLE);
            _liveScreen.drawMIDIinWaitForTraining(true);
          }
        } else {
          // handle encoder updates          
          boolean free = _setMIDINote(_slots[_editingSlotId].midiNote + action);
          _liveScreen.updateSampleConfig(_slots[_editingSlotId], 4, (action == 0 ? true : false), free ? LiveScreen::MIDI_NOTE_FREE : LiveScreen::MIDI_NOTE_IN_USE);
        }
        
        break;
      default:
        break;                        
    }
  } else {
    // no FN+Encoder functionality planned yet
  }
}


void Live::_handleOverviewEncoders(byte encoder, boolean function, int action) {

  if (encoder == 1 && !function && action == 1) {
    // increase bpm
    _changeBPM(_bpm >= 300.0 ? 300.0 : _bpm + 0.5);
  }

  if (encoder == 1 && !function && action == -1) {
    // decrease bpm
    _changeBPM(_bpm <= 30.0 ? 30.0 : _bpm - 0.5);
  }

  if (encoder == 1 && !function && action == 0) {
    // set bpm to fader value
    _changeBPM(_keyboard->getFaderValue(30, 300));
  }

  if (encoder == 1 && function && action == 0) {
    // reset bpm to default from sequencer
    _changeBPM(_play->getSong()->getPlayBackSpeed());
  }

}


void Live::loadConfig() {
  char buff[40];
  File readFile;
  byte *bufferBlocks;

  // read _slots
  strcpy(buff, _sfsio->getSongPath());
  strcat(buff, "/PATTERN/LIVESLTS.DAT");

  if(SD.exists(buff)) {
    readFile = SD.open(buff, FILE_READ);


    for (uint16_t i=0; i<sizeof(_slots)/sizeof(Play::LiveSlotDefinitionStruct); i++) {
        bufferBlocks = (byte *) &_slots[i];

        for (uint16_t j=0; j<sizeof(Play::LiveSlotDefinitionStruct); j++) {
            if (readFile.available()) {
                *(bufferBlocks + j) = readFile.read();
            }
        }            
    }

    readFile.close();

    for (int i=0; i<128; i++) {
      _midiNoteToSlot[i] = -1;
    }

    // reconstruct _midiToSlots array from _slots
    for (int i=0; i<72; i++) {
      if (_slots[i].type != Play::EMPTY) {
        _midiNoteToSlot[_slots[i].midiNote] = i;
      }
    }

  } else {
    Serial.println("Live Configuration does not exist on SD card.. using default values.");
  }
};


void Live::saveConfig() {
    char buff[40];
    File writeFile;
    byte *bufferBlocks;

    // write _slots 
    strcpy(buff, _sfsio->getSongPath());
    strcat(buff, "/PATTERN/LIVESLTS.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_slots)/sizeof(Play::LiveSlotDefinitionStruct); i++) {
        bufferBlocks = (byte *) &_slots[i];
        writeFile.write(bufferBlocks, sizeof(_slots[i]));
    }
 
    writeFile.close(); 
};


void Live::_removeSlot(int slotIndex) {
  _slots[slotIndex] = Play::LiveSlotDefinitionStruct();
  _cancel();
};


void Live::_playPiano(byte note, byte velocity, boolean play) {  
    _playSlot(_pianoSampleSlotIndex, velocity, play, note);
};