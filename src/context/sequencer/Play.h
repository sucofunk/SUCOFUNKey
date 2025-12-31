/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2025 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#ifndef Play_h
#define Play_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../helper/AudioResources.h"
#include "SongStructure.h"
#include "Selection.h"
#include "Snippets.h"
#include "Swing.h"

class Play {
    public:
        Play(Sucofunkey *keyboard, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources) {
            _keyboard = keyboard;
            _fsio = fsio;
            _sfsio = sfsio;
            _extmemArray = extmemArray;
            _audioResources = audioResources;

            _swing = _song.getSwing();
            _snippets = Snippets(_keyboard, &_song);

            _audioResources->playMemLive1.setKeyboardReference(keyboard);
            _audioResources->playMemLive2.setKeyboardReference(keyboard);
            _audioResources->playMemLive3.setKeyboardReference(keyboard);
            _audioResources->playMemLive4.setKeyboardReference(keyboard);
            _audioResources->playMemLive5.setKeyboardReference(keyboard);
            _audioResources->playMemLive6.setKeyboardReference(keyboard);
            _audioResources->playMemLive7.setKeyboardReference(keyboard);
            _audioResources->playMemLive8.setKeyboardReference(keyboard);
        };

        typedef struct {
            AudioPlayMemorySUCO* playMemory;
            AudioMixer4* playMemoryMixerL;
            AudioMixer4* playMemoryMixerR;
            int playMemoryMixerGain;
        } MixerSamplePlayMemory;


        enum LiveSlotType {
            EMPTY = 0,
            SNIPPET = 1,
            SAMPLE = 2,
            MUTE_SCRATCHING = 3,
            ADJUST_FADER = 4
        };

        enum ScratchModes {
            NONE = 0,
            FADER_TAPE = 1,
            FADER_VINYL = 2,
            LINE_IN_SERATO = 3
        };

        struct LiveSlotDefinitionStruct {
            LiveSlotType type = EMPTY;
            
            int snippet = -1;
            boolean loop = false;

            boolean immediateStopOnRelease = false;

            byte sampleNumber = 255; // sample, 1..72 -> 255 == nothing
            byte stereoPosition = 64; // 0 = 100% left | 64 = center | 127 = 100% right 
            byte velocity = 64;      // 0..127 -> standard: 64, as defined in midi standard for keyboards without velocity
            byte baseMidiNote = 60;
            byte pitchedNote = 60;
            byte reverse = false;

            byte midiNote = 0;
            boolean isPiano = false;

            //boolean faderScratching = false; // if fader will be read while playing sample to scratch back and forth
            ScratchModes scratchMode = NONE;

        } ;


        unsigned int *_extmemArray;
        AudioResources *_audioResources;

        SongStructure* getSong();
        Snippets* getSnippets();


        boolean loadSetPlay(byte bank1, byte sample1, byte channel, int position);
        void stopAllChannels();
        MixerSamplePlayMemory prepareMixerRouting(byte channel);
        void playMixedSample(byte channel, uint16_t position, int snippetSlot); // sequencer -> snippet = -1 --> take channel as in sequencer

        int bpmToMicroseconds(float bpm, int res);
        int calculatePlaybackTickSpeed();
        int calculatePlaybackTickSpeed(float bpm);
        void checkIfAllSamplesAreLoaded();
        
        boolean isSnippetPlaying(int slot);        
        boolean queueSnippet(int slot, boolean allowMultiple, boolean loop);
        
        // chain a snippet to another snippet. it will start when the "syncSlot" ends
        boolean chainSnippet(int syncSlot, int chainSlot, boolean loop);
        void unqueueSnippet(int slot);
        void snippetsPlayNext();
        void removeLoopFlagFromSnippet(int slot);

        boolean queueArrangement(int startPosition, boolean loop); // startPosition starts at 0 -> first sheet in arrangement
        void arrangementPlayNext();
        boolean isArrangementPlaying();
        void stopArrangement();

        // playing samples in live mode..
        boolean playNextFreeMemory(byte sample1, byte velocity, byte stereoPosition, byte baseNote, byte note, boolean reverse, ScratchModes scratchMode, boolean play, boolean loop);
        void handlePolyphonicAftertouch(byte sample1, byte velocity, byte stereoPosition);
        void polyChangeVelocity(byte polymem, byte velocity, byte stereoPosition);

        // muting channels
        void muteChannel(byte channel);
        void unMuteChannel(byte channel);
        void unMuteAllChannels();
        boolean isChannelMuted(byte channel);

    private:
        Sucofunkey* _keyboard;
        SampleFSIO* _sfsio;
        FSIO* _fsio;
        
        SongStructure _song;
        Snippets _snippets;
        Swing* _swing;        

        AudioPlayMemorySUCO* _getPlayMemSlot(byte slot); // 0..7

        int _playbackTickSpeed = 10000;

        byte _maxConcurrentSamples = 2; // Option: make it configurable per sample?

        byte _tempCurrentlyPlaying = 0;
        byte _tempFirstIndex = 127; // for temporarily saving the first play position
        byte _freeMemChannelCount = 0;
        boolean _keepReleaseSlotPlaying = false;

        boolean _playMemsInUse[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
        
        // up to four snippets can play in parallel.. the slots are listed in here
        int _playingSnippets[4] = {-1, -1, -1, -1};
        boolean _loopPlayingSnippets[4] = {false, false, false, false};

        int _chainedSnippets[4][2] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
        boolean _loopChainedSnippets[4][2] = {{false, false}, {false, false}, {false, false}, {false, false}};
        
        int _playPositionSnippets[4] = {-1, -1, -1, -1};
        
        // {channel1, channel2, .. channel8} <- channels in sequencer filled with index to _playmems
        byte _snippetChannels[4][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};

        byte _getFreeChannel();
        void _setChannelFree(byte channel);
        int _freeChannelCount();


        // queue for polyphonic events. each entry corresponds to a playMemLive1..6
        // 0 = not playing 1..72 -> corresponding sample is playing 
        byte _polyMemIDs[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        // stores the played note for each sample.. to stop the right sample, if playing a sample polyphonic
        byte _polyMemNotes[8] = {128, 128, 128, 128, 128, 128, 128, 128};

        // counter, how many samples were triggered after this one playing. used to find out, which sample started first, if one is triggered twice or more often
        byte _polyMemIncrement[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        float _tempVelocityL = 0.0;
        float _tempVelocityR = 0.0;

        boolean _loopArrangement = false;
        boolean _arrangementIsPlaying = false;
        boolean _arrangementIsPaused = false;
        int _arrangementSheetPosition = 0;
        int _playPositionArrangement = -1;
        int _lastPositionUntilNextArrangementSheet = 0;
        int _currentSheetPlays = 0;
        int _waitingInChainQueue = 0;

        boolean _mutedChannels[8] = {false, false, false, false, false, false, false, false};
};

#endif