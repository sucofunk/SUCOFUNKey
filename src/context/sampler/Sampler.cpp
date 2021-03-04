#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Sampler.h"


Sampler::Sampler(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _audioResources = audioResources;    
    _samplerScreen = SamplerScreen(_keyboard, _screen, _fsio, _sfsio, _audioResources);
    _bottomMenu = BottomMenu(_keyboard, _screen);
}

void Sampler::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT:
              if (_bottomMenu.isVisible()) {
                _bottomMenu.handleEvent({_keyboard->CURSOR_LEFT, true, false, _keyboard->KEY_OPERATION});
              } else {
                _keyboard->setBankDown();
                
                if (_keyboard->getBank() != _tempBank) {
                  if (_blinkActiveSample == true) _blinkSampleSlot(_activeSampleSlot, false);
                } else {
                  _blinkSampleSlot(_activeSampleSlot, true);
                }

                if (currentState == SAMPLE_WAIT_SAVE_SLOT) {
                  indicateFreeSamples(true, 0);                  
                } else {                 
                  indicateFreeSamples(false, 250);   
                }  
              }
              break;
        case Sucofunkey::CURSOR_RIGHT:
              if (_bottomMenu.isVisible()) {
                _bottomMenu.handleEvent({_keyboard->CURSOR_RIGHT, true, false, _keyboard->KEY_OPERATION});
              } else {                
                _keyboard->setBankUp(); 

                if (_keyboard->getBank() != _tempBank) {
                  if (_blinkActiveSample == true) _blinkSampleSlot(_activeSampleSlot, false);
                } else {
                  _blinkSampleSlot(_activeSampleSlot, true);
                }

                if (currentState == SAMPLE_WAIT_SAVE_SLOT) {                  
                  indicateFreeSamples(true, 0);
                } else {
                  indicateFreeSamples(false, 250);   
                }                
              }
              break;         
        case Sucofunkey::MENU:
              if (_bottomMenu.isVisible()) {
                _bottomMenu.showMenu(false);
              } else {
                if (currentState == SAMPLE_SELECTED) {
                  _bottomMenu.setupMenu2("Edit", 0, "Delete", 0);
                  _bottomMenu.selectItem(1);
                }
                if (currentState == SAMPLE_EDIT_TRIM) {
                  _bottomMenu.setupMenu3("Save", 0, "Save as", 0, "Cancel", 0);
                  _bottomMenu.selectItem(1);
                }
                _bottomMenu.showMenu(true);
              }
              break;
        case Sucofunkey::SET:
              if (_bottomMenu.isVisible()) {
                _bottomMenu.handleEvent({_keyboard->SET, true, false, _keyboard->KEY_OPERATION});
              }
              break;
        case Sucofunkey::FN_SET:
              if (currentState==SAMPLE_WAIT_SAVE_SLOT) {
                indicateFreeSamples(false,1);                
                _keyboard->setBank(_tempBank);
                _bottomMenu.showMenu(true);
                currentState = SAMPLE_EDIT_TRIM;
              }
              break;
        case Sucofunkey::PAUSE:
              _audioResources->playSdRaw.stop();
              break;              
        case Sucofunkey::PLAY:
              _play();
              break;        
      }
    }

    if (event.type == Sucofunkey::KEY_NOTE) {
      byte sampleId = _keyboard->getSampleIdByEventKey(event.index);      
      
      if (Sampler::currentState == SAMPLE_SELECTED || Sampler::currentState == SAMPLE_NOTHING) {
        _activeSampleSlot = sampleId;

        if (event.pressed) {
          _blinkSampleSlot(sampleId, true);
          _tempBank = _keyboard->getBank();
          currentState = SAMPLE_SELECTED;
          _bottomMenu.setupMenu2("Edit", 0, "Delete", 0);        
          _samplerScreen.showSampleInfo(_keyboard->getBank()-1, sampleId-1, 1.0);
          _audioResources->playSdRaw.play(_sfsio->sampleFilename[_keyboard->getBank()-1][sampleId-1]);
        } else {
          _audioResources->playSdRaw.stop();
        }
      }

      if (currentState == SAMPLE_WAIT_SAVE_SLOT) {
        // is selected slot free?
        if (!_sfsio->sampleBanksStatus[_keyboard->getBank()-1][sampleId-1]) {
          _samplerScreen.showSavingMessage();

          // saving..
          int sampleId72 =  _activeSampleSlot == 0 ? 72 : (_tempBank-1)*24+_activeSampleSlot-1;
          uint32_t start = _sfsio->pixelToWaveformSamples[sampleId72] * _trimMarkerStartPosition;
          uint32_t end = _sfsio->pixelToWaveformSamples[sampleId72] * (_trimMarkerEndPosition+1);

          if (_activeSampleSlot == 0) {
            _sfsio->copyFilePart(_sfsio->recorderFilename, _sfsio->sampleFilename[_keyboard->getBank()-1][sampleId-1], start*2, end*2, _volumeScaleFactor);
          } else {
            _sfsio->copyFilePart(_sfsio->sampleFilename[_tempBank-1][_activeSampleSlot-1], _sfsio->sampleFilename[_keyboard->getBank()-1][sampleId-1], start*2, end*2, _volumeScaleFactor);
          }
                    
          _sfsio->readSampleBankStatusFromSD();
          _sfsio->generateWaveFormBufferForSample(_keyboard->getBank()-1, sampleId-1);

          _keyboard->setBank(_tempBank);
          _bottomMenu.showMenu(true);
          currentState = SAMPLE_EDIT_TRIM;
          indicateFreeSamples(false,1);
          _blinkSampleSlot(_activeSampleSlot, true);
        } 
      }
    }


    if (event.type == Sucofunkey::EVENT_APPLICATION) {
      switch(event.index) {
        case Sucofunkey::BOTTOM_NAV_ITEM1:          
          // switch to edit mode
          if (currentState == SAMPLE_SELECTED) {
            editActiveSample();
          } else {
            // save current sample after editing
            if (currentState == SAMPLE_EDIT_TRIM) saveActiveSample();
          }
          break;
        case Sucofunkey::BOTTOM_NAV_ITEM2:
          // save current sample after editing
          if (currentState == SAMPLE_EDIT_TRIM) saveActiveSampleAs();
          break;
        case Sucofunkey::BOTTOM_NAV_ITEM3:
          // switch to edit mode
          if (currentState == SAMPLE_SELECTED) deleteActiveSample();
          // cancel edit operation
          if (currentState == SAMPLE_EDIT_TRIM) cancel();
          break;
      }
    }

    int maxWidth = 319;

    if (event.type == Sucofunkey::ENCODER && Sampler::currentState == SAMPLE_EDIT_TRIM) {
        byte sample72 = _activeSampleSlot == 0 ? 72 : (_tempBank-1)*24+_activeSampleSlot-1;
        switch (event.index) {
          case Sucofunkey::ENCODER_1:
                 maxWidth = _sfsio->waveFormBufferLength[sample72];
                _trimMarkerStartPosition = _trimMarkerStartPosition + (event.pressed ? 1 : -1);
                if (_trimMarkerStartPosition < 0) { _trimMarkerStartPosition = 0; };
                if (_trimMarkerStartPosition > maxWidth-2) { _trimMarkerStartPosition = maxWidth-2; };
                
                if (_trimMarkerStartPosition == _trimMarkerEndPosition && _trimMarkerStartPosition < maxWidth-2) {
                  _trimMarkerEndPosition++;
                }

                _samplerScreen.drawTrimMarker(_trimMarkerStartPosition, _trimMarkerEndPosition, sample72, _volumeScaleFactor);
                break;
          case Sucofunkey::ENCODER_2:
                maxWidth = _sfsio->waveFormBufferLength[sample72];
                _trimMarkerEndPosition += (event.pressed ? 1 : -1);
                if (_trimMarkerEndPosition < 1) { _trimMarkerEndPosition = 1; };
                if (_trimMarkerEndPosition > maxWidth-1) { _trimMarkerEndPosition = maxWidth-1; };

                if (_trimMarkerEndPosition == _trimMarkerStartPosition && _trimMarkerEndPosition > 1) {
                  _trimMarkerStartPosition--;
                }

                _samplerScreen.drawTrimMarker(_trimMarkerStartPosition, _trimMarkerEndPosition, sample72, _volumeScaleFactor);                
                break;          
          case Sucofunkey::ENCODER_3:
                _volumeScaleFactor += (event.pressed ? 0.025 : -0.025);
                if (_volumeScaleFactor < 0.1) _volumeScaleFactor = 0.1;
                if (_volumeScaleFactor > 5.0) _volumeScaleFactor = 5.0;                                
                _samplerScreen.showSampleInfo(sample72, _volumeScaleFactor);                
                _samplerScreen.drawTrimMarker(_trimMarkerStartPosition, _trimMarkerEndPosition, sample72, _volumeScaleFactor);
                break;
        }          
    }
}

long Sampler::receiveTimerTick() {
    _timerCounter++;

    if (_timerCounter == 3 && _blinkActiveSample && _activeSampleLEDPin != 0) {
      _blinkActiveSampleStatus = !_blinkActiveSampleStatus;
      _keyboard->setLEDState(_activeSampleLEDPin, _blinkActiveSampleStatus);      
    }

    if (_timerCounter == 3) {
      _timerCounter = 0;
    }

    if (_audioResources->playSdRaw.isPlaying()) {
      indicatePlayerPosition();
    } else {
      _samplerScreen.resetPlayerPosition();
    }    

    return 100000;
}

void Sampler::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);

    if (_sfsio->isRecodingAvailable()) {
      _activeSampleSlot = 0;
      _samplerScreen.showSampleInfo(3, 0, 1.0f);
      currentState = SAMPLE_SELECTED;
      // show menu instantly, as we want the user to edit/save the fresh record
      handleEvent({_keyboard->MENU, true, false, _keyboard->KEY_OPERATION, 0});
    } else {
      _samplerScreen.showEmptyScreen();
      indicateFreeSamples(true, 250);
      _activeSampleSlot = 0;
      currentState = SAMPLE_NOTHING;
    }

  } else {
    _isActive = false;
    _keyboard->setBank(0);
    _blinkActiveSample = false;
    _keyboard->setLEDState(_activeSampleLEDPin, false);
    _audioResources->playSdRaw.stop();
  }
}

void Sampler::_play() {
  // select sample to play (slot|recorder)
  char * filename = (_activeSampleSlot == 0 ? _sfsio->recorderFilename : _sfsio->sampleFilename[_keyboard->getBank()-1][_activeSampleSlot-1]);

  // play whole sample..
  if (currentState == SAMPLE_SELECTED) {
      _audioResources->playSdRaw.play(filename);
  }
  
  // .. or selected trim-part
  if (currentState == SAMPLE_EDIT_TRIM) {
    int sampleId72 = (_activeSampleSlot == 0 ? 72 : (_keyboard->getBank()-1)*24+_activeSampleSlot-1);

    uint32_t start = _sfsio->pixelToWaveformSamples[sampleId72] * _trimMarkerStartPosition;
    uint32_t end = _sfsio->pixelToWaveformSamples[sampleId72] * _trimMarkerEndPosition;

    _audioResources->playSdRaw.play(filename, start*2, end*2, _volumeScaleFactor);
  }
}

// reads samplefiles form sd, if readFromSD is true
// lights free sample slots up for ms milliseconds or leaves them on, if ms = 0 
void Sampler::indicateFreeSamples(boolean readFromSD, long ms) {
  byte bank = _keyboard->getBank();

  if (readFromSD) {
    _sfsio->readSampleBankStatusFromSD();
  }

  // light free sample slots up
  for (byte i=1; i<25; i++) {    
      _keyboard->setLEDState(_keyboard->getLEDPinBySampleId(i), !_sfsio->sampleBanksStatus[bank-1][i-1]);
  }       

  if (ms > 0) {
    delay(ms);
    // turn all sample slot LEDs off
    for (byte i=1; i<25; i++) {    
        _keyboard->setLEDState(_keyboard->getLEDPinBySampleId(i), false);
    }     
  }
};

void Sampler::_blinkSampleSlot(byte sampleId1, boolean on){
  if (on && sampleId1 > 0) {
    // turn current blinking LED off
    if (_activeSampleLEDPin != 0) {
      _keyboard->setLEDState(_activeSampleLEDPin, false);  
    }

    _activeSampleLEDPin = _keyboard->getLEDPinBySampleId(sampleId1);
    _blinkActiveSample = true;
  } else {    
    _blinkActiveSample = false;
    _keyboard->setLEDState(_activeSampleLEDPin, false);
  }
};


void Sampler::editActiveSample() {
    currentState = SAMPLE_EDIT_TRIM;
    byte sample72 = _activeSampleSlot == 0 ? 72 : (_tempBank-1)*24+_activeSampleSlot-1;
    _bottomMenu.setupMenu3("Save", 0, "Save as", 0, "Cancel", 0);
    _bottomMenu.showMenu(true);
    _trimMarkerEndPosition = _sfsio->waveFormBufferLength[sample72]-1;
    _samplerScreen.drawTrimMarker(_trimMarkerStartPosition, _trimMarkerEndPosition, sample72, _volumeScaleFactor);
    Serial.print("editActiveSample::endmarker::");
    Serial.println(_trimMarkerEndPosition-1);
}

void Sampler::deleteActiveSample() {
  Serial.println("deleteActiveSample");
  byte sample72 = _activeSampleSlot == 0 ? 72 : (_tempBank-1)*24+_activeSampleSlot-1;

  _sfsio->deleteFile( _activeSampleSlot == 0 ? _sfsio->recorderFilename : _sfsio->sampleFilename[_tempBank-1][_activeSampleSlot-1]);
  
  _sfsio->readSampleBankStatusFromSD();
  _sfsio->clearWaveFormBufferById(sample72);
  _sfsio->waveFormBufferLength[sample72] = 0;
  _bottomMenu.showMenu(false);
  _samplerScreen.showEmptyScreen();  
}

void Sampler::saveActiveSample() {
  _samplerScreen.showSavingMessage();

  int sampleId72 = _activeSampleSlot == 0 ? 72 : (_tempBank-1)*24+_activeSampleSlot-1;
  uint32_t start = _sfsio->pixelToWaveformSamples[sampleId72] * _trimMarkerStartPosition;
  uint32_t end = _sfsio->pixelToWaveformSamples[sampleId72] * (_trimMarkerEndPosition+1);

  char * filename = _activeSampleSlot == 0 ? _sfsio->recorderFilename : _sfsio->sampleFilename[_tempBank-1][_activeSampleSlot-1];

  // copy to temp file 
  _sfsio->deleteFile("TEMP.RAW");
  _sfsio->copyFilePart(filename, "TEMP.RAW", start*2, end*2, _volumeScaleFactor);

  // remove sample 
  _sfsio->deleteFile(filename);

  // copy temp to sample 
  _sfsio->copyFile("TEMP.RAW", filename);

  // remove temp
  _sfsio->deleteFile("TEMP.RAW");

  _sfsio->readSampleBankStatusFromSD();
  if (_activeSampleSlot == 0) {
    _sfsio->generateWaveFormBufferForSample(3, 0);
  } else {
    _sfsio->generateWaveFormBufferForSample(_keyboard->getBank()-1, _activeSampleSlot-1);
  }

  currentState = SAMPLE_SELECTED;
  _samplerScreen.showSampleInfo(sampleId72, 1.0);
  _bottomMenu.showMenu(false);

  _trimMarkerStartPosition = 0;
  _trimMarkerEndPosition = 319;
  _volumeScaleFactor = 1.0;
}

void Sampler::saveActiveSampleAs() {
  Serial.println("saveActiveSampleAs");
  currentState = SAMPLE_WAIT_SAVE_SLOT;
  _bottomMenu.showMenu(false);
  _samplerScreen.showSlotSelectionHint();
  indicateFreeSamples(true, 0);
}

void Sampler::cancel() {
  Serial.println("cancel");
  currentState = SAMPLE_SELECTED;

  if (_activeSampleSlot == 0) {
    _samplerScreen.showSampleInfo(72, 1.0);
  } else {
    _samplerScreen.showSampleInfo(_keyboard->getBank()-1, _activeSampleSlot-1, 1.0);    
  }
  
  _bottomMenu.showMenu(false);
  _trimMarkerStartPosition = 0;
  _trimMarkerEndPosition = 319;
  _volumeScaleFactor = 1.0;
}


void Sampler::indicatePlayerPosition() {
  int sampleId72 = _activeSampleSlot == 0 ? 72 : (_tempBank-1)*24+_activeSampleSlot-1;
  
  uint32_t pixelMs = _sfsio->sampleLengthMS[sampleId72] / 319;
  //Serial.print("pixelMs::");
  //Serial.println(pixelMs);
  if (_audioResources->playSdRaw.isPlaying()) {
    _samplerScreen.drawPlayerPosition(_audioResources->playSdRaw.positionMillis() / pixelMs, (currentState == SAMPLE_EDIT_TRIM ? _trimMarkerStartPosition : 0), (currentState == SAMPLE_EDIT_TRIM ? _trimMarkerEndPosition : 319));
  }
}