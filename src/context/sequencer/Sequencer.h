#ifndef Sequencer_h
#define Sequencer_h

#include <Arduino.h>
#include <Audio.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Pattern.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"
#include "../sampler/Sampler.h"
#include "../../gui/screens/SequencerScreen.h"

class Sequencer {
    public:
        Sequencer(Sucofunkey *keyboard, Screen *screen, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources);
        long receiveTimerTick();
        long bpmToMicroseconds(float bpm, int quant);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void loadSetPlay(byte bank, byte sample, byte column, int row);
        void moveRow(int count);
        void moveColumn(byte count);
        void playPattern();
        void pausePattern();
        void stopAllChannels();
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;        
        SampleFSIO *_sfsio;
        unsigned int *_extmemArray;
        Sampler *_sampler;
        AudioResources *_audioResources;

        SequencerScreen _sequencerScreen;

        boolean _isActive = false;
        byte _activeBank = 1;
        byte _highlightEvery = 4;
        float _bpm = 80.0;
        long _playbackTickSpeed = bpmToMicroseconds(_bpm, _highlightEvery); // microseconds interval to receive Ticks
        
        Pattern _pattern = Pattern(64, _sfsio, _audioResources);
        volatile boolean _isPlaying = false;
        boolean _playedFirstRow = false;
        volatile int16_t _cursorRow = 0;        
        byte _cursorColumn = 1;
        
        void _playNext();
        byte _activeNoteLEDPin = 0;
};

#endif
