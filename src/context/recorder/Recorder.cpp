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
            Serial.print("RECORD in Recorder::");
            Serial.println(currentState);
            if (currentState == RECORDER_RECORDING) {
              _recorderScreen.showRecorderScreen();
              stopRecording();
            } else {
              startRecording();
              _recorderScreen.showRecorderScreenRecording();
            }
            break;
      }
    }

    if (event.type == Sucofunkey::ENCODER) {
        switch (event.index) {
          case Sucofunkey::ENCODER_1:
                break;
          case Sucofunkey::ENCODER_2:
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
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}

void Recorder::startRecording() {
    // delete old recording, if available
    if (_sfsio->isRecodingAvailable()) {
        _sfsio->deleteFile(_sfsio->recorderFilename);
    }

    _frec = SD.open(_sfsio->recorderFilename, FILE_WRITE);

    Serial.print(_sfsio->recorderFilename);
    Serial.print("::");
    Serial.println(_frec);

    if (_frec) {
        _keyboard->setLEDState(Sucofunkey::LED_RECORD, true);
        currentState = RECORDER_RECORDING;

        _audioResources->queue1.begin();
    }
}

void Recorder::continueRecording() {  
  if (_audioResources->queue1.available() >= 2) {
    
    if (_audioResources->queue1.available() > 40) Serial.println(_audioResources->queue1.available());  

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

  _sfsio->generateWaveFormBufferForSample(3, 0);
  _keyboard->addApplicationEventToQueue(_keyboard->RECORDED);
}

boolean Recorder::isRecording() {
    return currentState == RECORDER_RECORDING ? true : false;
};