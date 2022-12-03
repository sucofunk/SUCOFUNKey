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

#include "Play.h"
#include "Selection.h"

SongStructure* Play::getSong() {
    return &_song;
};

Snippets* Play::getSnippets() {
    return &_snippets;
}

// loads a sample into memory and plays it back
// returns true/false if it could be loaded to extmem
boolean Play::loadSetPlay(byte bank1, byte sample1, byte channel, int position) {
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
      _audioResources->playMem.play(_extmemArray + _sfsio->getExtmemOffset((bank1-1)*24+sample1));
    } else {
      // failed, because there is not enough memory left
      _song.removePosition(channel, position);
      Serial.println("out of memory");
      return false;
    }   
  } else {
    return false;
  }
  return true;
}


void Play::stopAllChannels() {
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
  _audioResources->playMem9.stop();
  _audioResources->playMem10.stop();
  _audioResources->playMem11.stop();
  _audioResources->playMem12.stop();
  _audioResources->playMem13.stop();
  _audioResources->playMem14.stop();
  _audioResources->playMem15.stop();
  _audioResources->playMem16.stop();

  _audioResources->playMemLive1.stop();
  _audioResources->playMemLive2.stop();
  _audioResources->playMemLive3.stop();
  _audioResources->playMemLive4.stop();
  _audioResources->playMemLive5.stop();
  _audioResources->playMemLive6.stop();
  _audioResources->playMemLive7.stop();
  _audioResources->playMemLive8.stop();
};

Play::MixerSamplePlayMemory Play::prepareMixerRouting(byte channel) {;
    MixerSamplePlayMemory mixSPM;

    if (channel <= 3) {
        mixSPM.playMemoryMixerL = &_audioResources->mixerMem1L;
        mixSPM.playMemoryMixerR = &_audioResources->mixerMem1R;
        mixSPM.playMemoryMixerGain = channel;
    } else {
        if (channel <= 7) {
          mixSPM.playMemoryMixerL = &_audioResources->mixerMem2L;
          mixSPM.playMemoryMixerR = &_audioResources->mixerMem2R;
          mixSPM.playMemoryMixerGain = channel-4;
        } else {
          if (channel <= 11) {
            mixSPM.playMemoryMixerL = &_audioResources->mixerMem3L;
            mixSPM.playMemoryMixerR = &_audioResources->mixerMem3R;
            mixSPM.playMemoryMixerGain = channel-8;
          } else {
            mixSPM.playMemoryMixerL = &_audioResources->mixerMem4L;
            mixSPM.playMemoryMixerR = &_audioResources->mixerMem4R;
            mixSPM.playMemoryMixerGain = channel-12;
          }
        }
    }

    switch (channel) {
        case 0:
        mixSPM.playMemory = &_audioResources->playMem1;
        break;
        case 1:
        mixSPM.playMemory = &_audioResources->playMem2;
        break;
        case 2:
        mixSPM.playMemory = &_audioResources->playMem3;
        break;
        case 3:
        mixSPM.playMemory = &_audioResources->playMem4;
        break;
        case 4:
        mixSPM.playMemory = &_audioResources->playMem5;
        break;
        case 5:
        mixSPM.playMemory = &_audioResources->playMem6;
        break;
        case 6:
        mixSPM.playMemory = &_audioResources->playMem7;
        break;
        case 7:
        mixSPM.playMemory = &_audioResources->playMem8;
        break;
        case 8:
        mixSPM.playMemory = &_audioResources->playMem9;
        break;
        case 9:
        mixSPM.playMemory = &_audioResources->playMem10;
        break;
        case 10:
        mixSPM.playMemory = &_audioResources->playMem11;
        break;
        case 11:
        mixSPM.playMemory = &_audioResources->playMem12;
        break;
        case 12:
        mixSPM.playMemory = &_audioResources->playMem13;
        break;
        case 13:
        mixSPM.playMemory = &_audioResources->playMem14;
        break;
        case 14:
        mixSPM.playMemory = &_audioResources->playMem15;
        break;
        case 15:
        mixSPM.playMemory = &_audioResources->playMem16;
        break;                                                                
    }

    return mixSPM;
}


// plays one sample from the sequencer grid, as defined at channel/position
// snippet == -1 -> take original channels from sequencer
void Play::playMixedSample(byte channel, uint16_t position, int snippetSlot) {
  Play::MixerSamplePlayMemory mixSPM;

  if (snippetSlot == -1) {
    mixSPM = prepareMixerRouting(channel);
  } else {
    // playing an arrangement/snippets -> getChannel from snippetChannels

    int playingSnippetsIndex = -1;

    for (int i=0; i<4; i++) {
      if (_playingSnippets[i] == snippetSlot) {
        playingSnippetsIndex = i;
        break;
      }
    }

    // something went wrong
    if (playingSnippetsIndex == -1) {
      Serial.println("Fehler!");
      return;
    }

    // read the exact channel from snippetsChannel
    mixSPM = prepareMixerRouting(_snippetChannels[playingSnippetsIndex][channel]);
  }
  

  SongStructure::samplePointerStruct sps = _song.getPosition(channel, position);

  if (sps.type == SongStructure::NOTE_OFF) {
    mixSPM.playMemory->stop();
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
        swingLevel = _song.getSwing()->getLevelFromBinarySwingExpression(swingExpression);
        swingGroup = _song.getSwing()->getGroupFromBinarySwingExpression(swingExpression);    
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
        mixSPM.playMemoryMixerL->gain(mixSPM.playMemoryMixerGain, (velocity/127.0)*1.0);
        mixSPM.playMemoryMixerR->gain(mixSPM.playMemoryMixerGain, (velocity/127.0)*(stereoPosition/64.0));
      } else {
        mixSPM.playMemoryMixerL->gain(mixSPM.playMemoryMixerGain, (velocity/127.0)*((127-stereoPosition)/64.0));
        mixSPM.playMemoryMixerR->gain(mixSPM.playMemoryMixerGain, (velocity/127.0)*1.0);
      }
  
      if (sps.type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
        mixSPM.playMemory->adjustFrequencyByTick(pitchChange-127);
        if (reverse) mixSPM.playMemory->reversePlayback();        
      }

      if (swingGroup != 0) {
        swingLevel = _song.getSwingGroupLevel(swingGroup);
      }
      shiftSamples = _song.getSwing()->getShiftSamplesForSwingLevel(swingLevel);

      // do not play a sample, if position is a parameter change .. but check if sample is in EXTMEM
      if (_song.getPosition(channel, position).type == SongStructure::SAMPLE && _sfsio->getExtmemOffset(_song.getSampleFromBucketId(_song.getPosition(channel, position).typeIndex).sampleNumber) != -1)  {
        mixSPM.playMemory->playPitched(_extmemArray + _sfsio->getExtmemOffset(sampleNumber), 60, pitchedNote, shiftSamples, reverse);
      }
    }
  }
}


int Play::bpmToMicroseconds(float bpm, int res) {
  if (bpm < 1.0) {
    // some kind of nasty fallback to prevent division by 0, if bpm is 0
    return 10000;
  }

  return floor((60000000.0/bpm/res));
}

int Play::calculatePlaybackTickSpeed() {  
  _playbackTickSpeed = bpmToMicroseconds(_song.getPlayBackSpeed(), _song.getSongResolution()*4);
  
  // recalculate swing delays
  _song.setSwingTickMicroseconds(_playbackTickSpeed);
  
  return _playbackTickSpeed;
}

int Play::calculatePlaybackTickSpeed(float bpm) {  
  _playbackTickSpeed = bpmToMicroseconds(bpm, _song.getSongResolution()*4);
  
  // recalculate swing delays
  _song.setSwingTickMicroseconds(_playbackTickSpeed);
  
  return _playbackTickSpeed;
}



// checks if the used samples in the song are available on disk and already loaded into extmem. if not, do so..
void Play::checkIfAllSamplesAreLoaded() {
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



// -----------------------------------------------------------------------------------------------------------------
// --- Snippets ----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------

boolean Play::queueSnippet(int slot, boolean allowMultiple, boolean loop) {
  Selection::SelectionStruct s = _song.getSnippet(slot);
  int channels = s.endY-s.startY+1;
  
  int playingSnippetsIndex = -1;


  // playing a snippet multiple times is not allowed? check first if it is already in queue
  if (!allowMultiple) {
    for (int i=0; i<4; i++) {
      if (_playingSnippets[i] == slot) return false;
    }
  }
     
  for (int i=0; i<4; i++) {
    if (_playingSnippets[i] == -1) {
      playingSnippetsIndex = i;
      break;
    }
  }

  // snippet is not defined OR there are already four snippets playing in parallel
  if (playingSnippetsIndex == -1) return false;

  // no free channels
  if (_freeChannelCount() < channels) return false;

  _playingSnippets[playingSnippetsIndex] = slot;
  _loopPlayingSnippets[playingSnippetsIndex] = loop;
  _playPositionSnippets[playingSnippetsIndex] = 0;

  for (int c = s.startY; c <= s.endY; c++) {
    _snippetChannels[playingSnippetsIndex][c] = _getFreeChannel();
  }

  return true;
};


void Play::unqueueSnippet(int slot) {
  int playingSnippetsIndex = -1;

  for (int i=0; i<4; i++) {
    if (_playingSnippets[i] == slot) {
      playingSnippetsIndex = i;
      break;
    }
  }

  _playingSnippets[playingSnippetsIndex] = -1;
  _playPositionSnippets[playingSnippetsIndex] = -1;

  for (int c=0; c<8; c++) {
    if (_snippetChannels[playingSnippetsIndex][c] != 0) {
      _setChannelFree(_snippetChannels[playingSnippetsIndex][c]);
    }

    _snippetChannels[playingSnippetsIndex][c] = 0;
  }
};


void Play::snippetsPlayNext() {
  
  // ToDo: don't forget MIDI!

  int snippetLength = 0;
  Selection::SelectionStruct snippet;

  for (int s = 0; s < 4; s++) {
    if (_playingSnippets[s] != -1) {
      snippet = _song.getSnippet(_playingSnippets[s]);
      snippetLength = snippet.endX - snippet.startX+1;
    
      // play all channels of snippet
      for (int c = snippet.startY; c <= snippet.endY; c++) {
        if (_song.getPosition(c, snippet.startX + _playPositionSnippets[s]).type != SongStructure::UNDEFINED) {
          playMixedSample(c, snippet.startX + _playPositionSnippets[s], _playingSnippets[s]);
        }        
      }

      if (_playPositionSnippets[s] < snippetLength-1) {
        _playPositionSnippets[s]++;
      } else {
        // end of snippet reached.. remove it from queue if not looping
        if (!_loopPlayingSnippets[s]) {
          unqueueSnippet(_playingSnippets[s]);
        } else {
          _playPositionSnippets[s] = 0;
        }
        
      }
    }    
  }
};

void Play::removeLoopFlagFromSnippet(int slot) {
  for (int s = 0; s < 4; s++) {
    if (_playingSnippets[s] == slot && _loopPlayingSnippets[s]) {
      _loopPlayingSnippets[s] = false;
      break;
    }    
  }
};


// 1..16
// returns the next free channel (1..16) or 0 if all channels are in use
byte Play::_getFreeChannel() {
  for (byte i=0; i<16; i++) {
    if (!_playMemsInUse[i]) {
      _playMemsInUse[i] = true;
      return i+1;
    }
  }
  return 0;
};

// 1..16
void Play::_setChannelFree(byte channel) {
  _playMemsInUse[channel-1] = false;  
};

int Play::_freeChannelCount() {
  int retVal = 0;

  for (int i=0; i<16; i++) {
    if (!_playMemsInUse[i]) retVal++;
  }

  return retVal;
};





// -----------------------------------------------------------------------------------------------------------------
// --- Live --------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------

void Play::playNextFreeMemory(byte sample1, byte velocity, byte stereoPosition, byte baseNote, byte note, boolean reverse, boolean play) {

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
          _polyMemNotes[i] = note;
        
        switch(i) {
          case 0:
            polyChangeVelocity(i, velocity, stereoPosition);
            _audioResources->playMemLive1.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;
          case 1:
            polyChangeVelocity(i, velocity, stereoPosition);          
            _audioResources->playMemLive2.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;
          case 2:
            polyChangeVelocity(i, velocity, stereoPosition);          
            _audioResources->playMemLive3.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;
          case 3:
            polyChangeVelocity(i, velocity, stereoPosition);          
            _audioResources->playMemLive4.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;
          case 4:
            polyChangeVelocity(i, velocity, stereoPosition);          
            _audioResources->playMemLive5.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;
          case 5:
            polyChangeVelocity(i, velocity, stereoPosition);          
            _audioResources->playMemLive6.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;
          case 6:
            polyChangeVelocity(i, velocity, stereoPosition);          
            _audioResources->playMemLive7.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;
          case 7:
            polyChangeVelocity(i, velocity, stereoPosition);          
            _audioResources->playMemLive8.playPitched(_extmemArray + _sfsio->getExtmemOffset(sample1), baseNote, note, 0, reverse);
            break;                                                                        
        }

        return;
      }
    }  
  } else {
    // stop wavetable
    for (int i=0; i<6; i++) {
      if (_polyMemIDs[i] == sample1 && _polyMemNotes[i] == note) {
          _polyMemIDs[i] = 0;
          _polyMemIDs[i] = 128;

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


void Play::polyChangeVelocity(byte polymem, byte velocity, byte stereoPosition) {

  if (stereoPosition < 64) {
    _tempVelocityL = (velocity/127.0)*1.0;
    _tempVelocityR = (velocity/127.0)*(stereoPosition/64.0);
  } else {
    _tempVelocityL = (velocity/127.0)*((127-stereoPosition)/64.0);
    _tempVelocityR = (velocity/127.0)*1.0;
  }

  switch(polymem) {
    case 0:
      _audioResources->mixerMem5L.gain(0, _tempVelocityL);
      _audioResources->mixerMem5R.gain(0, _tempVelocityR);
      break;
    case 1:
      _audioResources->mixerMem5L.gain(1, _tempVelocityL);
      _audioResources->mixerMem5R.gain(1, _tempVelocityR);
      break;
    case 2:
      _audioResources->mixerMem5L.gain(2, _tempVelocityL);
      _audioResources->mixerMem5R.gain(2, _tempVelocityR);
      break;
    case 3:
      _audioResources->mixerMem5L.gain(3, _tempVelocityL);
      _audioResources->mixerMem5R.gain(3, _tempVelocityR);
      break;
    case 4:
      _audioResources->mixerMem6L.gain(0, _tempVelocityL);
      _audioResources->mixerMem6R.gain(0, _tempVelocityR);
      break;
    case 5:
      _audioResources->mixerMem6L.gain(1, _tempVelocityL);
      _audioResources->mixerMem6R.gain(1, _tempVelocityR);
      break;
    case 6:
      _audioResources->mixerMem6L.gain(2, _tempVelocityL);
      _audioResources->mixerMem6R.gain(2, _tempVelocityR);
      break;
    case 7:
      _audioResources->mixerMem6L.gain(3, _tempVelocityL);
      _audioResources->mixerMem6R.gain(3, _tempVelocityR);
      break;
  }
}


void Play::handlePolyphonicAftertouch(byte sample1, byte velocity, byte stereoPosition) {
  for (int i=0; i<8; i++) {
    if (_polyMemIDs[i] == sample1) {
      polyChangeVelocity(i, velocity, stereoPosition);
      return;
    }
  }
}