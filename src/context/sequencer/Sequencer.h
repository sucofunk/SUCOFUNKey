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

        enum Direction {
                UP = 1,
                RIGHT = 2,
                DOWN = 3,
                LEFT = 4
        };

        long receiveTimerTick();

        void moveCursor(Direction direction);

        long bpmToMicroseconds(float bpm, int res);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void loadSetPlay(byte bank, byte sample, byte channel, int position);
        void playPattern();
        void playMixedSample(byte channel, uint16_t position);
        void pausePattern();
        void stopAllChannels();

        boolean isPlaying();
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;        
        SampleFSIO *_sfsio;
        unsigned int *_extmemArray;
        Sampler *_sampler;
        AudioResources *_audioResources;

        SequencerScreen _sequencerScreen;

        AudioPlayMemory *_playMemory;
        AudioMixer4 *_playMemoryMixerL;
        AudioMixer4 *_playMemoryMixerR;
        AudioSynthWavetable *_playWavetable;
        AudioMixer4 *_playWavetableMixerL;
        AudioMixer4 *_playWavetableMixerR;

        int _playMemoryMixerGain;
        int _playWavetableMixerGain;

        boolean _isActive = false;
        byte _activeBank = 1;
        boolean _keyboardMode = false;

        long _calculatePlaybackTickSpeed();

        long _playbackTickSpeed = _calculatePlaybackTickSpeed(); // microseconds interval to receive Ticks
        
        Pattern _pattern = Pattern(16, _sfsio, _audioResources);
        volatile boolean _isPlaying = false;

        int _playerPosition = 0;
        volatile int _blinkPosition = 0;

        void _playNext();
        byte _activeNoteLEDPin = 0;

        byte _cursorChannel = 0;
        uint16_t _cursorPosition = 0;

        boolean _playLEDon = false;
        
        void _printPatternData();
};

#endif
