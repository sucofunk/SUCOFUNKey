#ifndef Sequencer_h
#define Sequencer_h

#include <Arduino.h>
#include <Audio.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "SongStructure.h"
#include "Zoom.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../helper/AudioResources.h"
#include "../sampler/Sampler.h"
#include "../../gui/screens/SequencerScreen.h"
#include "../../gui/screens/components/BlackKeyMenu.h"

class Sequencer {
    public:
        Sequencer(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources);

        Zoom zoom;

        enum MenuState {
            MENU_NONE = 0,
            MENU_BLACKKEY = 1,
            MENU_PIANO = 2,
        };

        enum SequencerState {
            NORMAL = 0,
            MOVE_CELL = 1,
            DOUBLE_CELL = 2
        };

        enum Direction {
            UP = 1,
            RIGHT = 2,
            DOWN = 3,
            LEFT = 4
        };

        long receiveTimerTick();

        void moveCursor(Direction direction);

        boolean setMenuState(MenuState state);
        void setSequencerState(SequencerState state);

        long bpmToMicroseconds(float bpm, int res);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void loadSetPlay(byte bank, byte sample, byte channel, int position);
        void playSong();
        void playMixedSample(byte channel, uint16_t position);
        void pausePattern();
        void stopAllChannels();

        boolean isPlaying();
        
        void loadFromSD();
        void saveToSD();
        void debugInfos();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;        
        SampleFSIO *_sfsio;
        FSIO *_fsio;
        unsigned int *_extmemArray;
        Sampler *_sampler;
        AudioResources *_audioResources;

        SongStructure _song = SongStructure();

        SequencerScreen _sequencerScreen;
        BlackKeyMenu _blackKeyMenu;

        AudioPlayMemory *_playMemory;
        AudioMixer4 *_playMemoryMixerL;
        AudioMixer4 *_playMemoryMixerR;
        AudioSynthWavetableSUCO *_playWavetable;
        AudioMixer4 *_playWavetableMixerL;
        AudioMixer4 *_playWavetableMixerR;

        int _playMemoryMixerGain;
        int _playWavetableMixerGain;

        boolean _isActive = false;
        byte _activeBank = 1;
        boolean _keyboardMode = false;

        MenuState _currentMenuState = MENU_NONE;
        SequencerState _currentSequencerState = NORMAL;

        long _calculatePlaybackTickSpeed();
        long _playbackTickSpeed = _calculatePlaybackTickSpeed(); // microseconds interval to receive Ticks
        
        volatile boolean _isPlaying = false;

        int _playerPosition = 0;
        volatile int _blinkPosition = 0;

        void _prepareMixerRouting(byte channel);
        void _playNext();
        byte _activeNoteLEDPin = 0;

        void _saveCurrentCursorPosition();
        boolean _savedCursorEqualsCurrent();

        byte _cursorChannel = 0;
        uint16_t _cursorPosition = 0;

        // to save a position for some actions like moving a selected cell..
        byte _savedCursorChannel = 0;
        uint16_t _savedCursorPosition = 0;

        boolean _playLEDon = false;        
};

#endif
