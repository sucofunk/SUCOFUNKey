#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Recorder.h"

Recorder::Recorder(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources) {
    _keyboard = keyboard;    
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _audioResources = audioResources;    
    _recorderScreen = RecorderScreen(_keyboard, _screen, _fsio, _sfsio, _audioResources);
}

void Recorder::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::RECORD:
            if (currentState == RECORDER_RECORDING) {
              _recorderScreen.showRecorderScreen();
              stopRecording();
            } else {
              startRecording();
              _recorderScreen.showRecorderScreenRecording();
            }
            break;
        case Sucofunkey::FN_RECORD:
            if (currentState == RECORDER_NOTHING) {
              cancelRecording();
            }
            break;
        case Sucofunkey::PAUSE:
            if (currentState == RECORDER_RECORDING) {
              _recorderScreen.showRecorderScreen();
              stopRecording();
            } else {
              if (currentState == RECORDER_NOTHING) {
                cancelRecording();
              }
            }

            break;
        case Sucofunkey::INPUTSELECTOR:
            _lastInput = _keyboard->getInput();
            _keyboard->toggleInput();
            activateInput();
            break;
      }
    }

    if (event.type == Sucofunkey::ENCODER) {
        switch (event.index) {
          case Sucofunkey::ENCODER_1:
                if (event.pressed) {
                  _audioResources->increaseMicGain();
                } else {
                  _audioResources->decreaseMicGain();
                }
                break;
          case Sucofunkey::ENCODER_2:
                if (event.pressed) {
                  _audioResources->increaseLineInVolume();
                } else {
                  _audioResources->decreaseLineInVolume();
                }
                break;
          case Sucofunkey::ENCODER_3:
                break;
          case Sucofunkey::ENCODER_4:
                break;
        }
    }

}

long Recorder::receiveTimerTick() {
    if (_isActive) {
         _recorderScreen.drawInputPeakMeter(_audioResources->peak1.read());
    }

    return 70000;
}

void Recorder::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(0);
    currentState = RECORDER_NOTHING;
    _recorderScreen.showRecorderScreen();
    _keyboard->setInput(_lastInput);
    activateInput();    
  } else {
    if (!_isActive) return;
    _isActive = false;
    // store current Input for next recording
    _lastInput = _keyboard->getInput();
    _keyboard->setBank(0);

    if (currentState == RECORDER_NOTHING) {
      _audioResources->muteInput();
      _audioResources->muteResampling();
      _lastInput = _keyboard->getInput();
      _keyboard->setInput(Sucofunkey::INPUT_NONE);
      activateInput();
    }

  }
}

void Recorder::cancelRecording() {
  currentState == RECORDER_NOTHING;
  _keyboard->setInput(Sucofunkey::INPUT_NONE);
  _audioResources->muteInput();
  _audioResources->muteResampling();
  _keyboard->addApplicationEventToQueue(_keyboard->RECORDER_CANCEL);  
}

void Recorder::startRecording() {
    // delete old recording, if available
    if (_sfsio->isRecodingAvailable()) {
        _sfsio->deleteFile(_sfsio->recorderFilename);
    }

    _frec = SD.open(_sfsio->recorderFilename, FILE_WRITE);

    if (_frec) {
        _keyboard->setLEDState(Sucofunkey::LED_RECORD, true);
        currentState = RECORDER_RECORDING;

        _audioResources->queue1.begin();
    }
}

void Recorder::continueRecording() {  
  if (_audioResources->queue1.available() >= 2) {
    
//    if (_audioResources->queue1.available() > 40) Serial.println(_audioResources->queue1.available());  

    byte buffer[512];
    memcpy(buffer, _audioResources->queue1.readBuffer(), 256);
    _audioResources->queue1.freeBuffer();
    memcpy(buffer + 256, _audioResources->queue1.readBuffer(), 256);
    _audioResources->queue1.freeBuffer();
    // write all 512 bytes to the SD card
    _frec.write(buffer, 512);
  }
}

void Recorder::stopRecording() {
  _audioResources->queue1.end();

  // write last bit of buffer to file..
  while (_audioResources->queue1.available() > 0) {
    _frec.write((byte*)_audioResources->queue1.readBuffer(), 256);
    _audioResources->queue1.freeBuffer();
  }

  _frec.close();

  _keyboard->setLEDState(Sucofunkey::LED_RECORD, false);
  currentState = RECORDER_NOTHING;

  // recording stopped from another context? save selected input.
  if (!_isActive) {
    _lastInput = _keyboard->getInput();
    _keyboard->setInput(Sucofunkey::INPUT_NONE);
    _audioResources->muteInput();
    _audioResources->muteResampling();
  }

  _sfsio->generateWaveFormBufferForSample(3, 0);
  _keyboard->addApplicationEventToQueue(_keyboard->RECORDED);
}

boolean Recorder::isRecording() {
    return currentState == RECORDER_RECORDING ? true : false;
};


void Recorder::activateInput() {
  switch(_keyboard->getInput()) {
    case Sucofunkey::INPUT_LINE:
        _audioResources->setInputLine();
        _audioResources->unmuteInput();
        _audioResources->muteResampling();
        break;
    case Sucofunkey::INPUT_LINE_RESAMPLE:
        _audioResources->setInputLine();
        _audioResources->unmuteInput();
        _audioResources->unmuteResampling();
        break;
    case Sucofunkey::INPUT_MIC:
        _audioResources->setInputMic();
        _audioResources->unmuteInput();
        _audioResources->muteResampling();
        break;
    case Sucofunkey::INPUT_MIC_RESAMPLE:
        _audioResources->setInputMic();
        _audioResources->unmuteInput();
        _audioResources->unmuteResampling();
        break;
    case Sucofunkey::INPUT_RESAMPLE:
        _audioResources->muteInput();
        _audioResources->unmuteResampling();
        break;
    case Sucofunkey::INPUT_NONE:
        _audioResources->muteInput();
        _audioResources->muteResampling();
        break;    
  }
}
