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
            SAMPLE = 2
        };

        enum LiveSamplePlayType {
            COMPLETE = 0,
            STOPONRELEASE = 1,
            HOLDFADER = 2,
            PIANO = 3,
            LOOP = 4
        };

        typedef struct {
            LiveSlotType type = EMPTY;
            
            int snippet = -1;
            boolean loopSnippet = false;

            boolean immediateStopOnRelease = false;

            byte sampleNumber = 255; // sample, 1..72 -> 255 == nothing
            byte stereoPosition = 64; // 0 = 100% left | 64 = center | 127 = 100% right 
            byte velocity = 64;      // 0..127 -> standard: 64, as defined in midi standard for keyboards without velocity
            byte baseMidiNote = 60;
            byte pitchedNote = 60;
            byte reverse = false;

            byte midiNote = 0;
            boolean isPiano = false;

        } LiveSlotDefinitionStruct;


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
        
        boolean queueSnippet(int slot, boolean allowMultiple, boolean loop);
        void unqueueSnippet(int slot);
        void snippetsPlayNext();
        void removeLoopFlagFromSnippet(int slot);

        // playing samples in live mode..
        //void playNextFreeMemory(byte sample1, byte velocity, boolean play);
        void playNextFreeMemory(byte sample1, byte velocity, byte stereoPosition, byte baseNote, byte note, boolean reverse, boolean play);
        void handlePolyphonicAftertouch(byte sample1, byte velocity, byte stereoPosition);
        void polyChangeVelocity(byte polymem, byte velocity, byte stereoPosition);


    private:
        Sucofunkey* _keyboard;
        SampleFSIO* _sfsio;
        FSIO* _fsio;
        
        SongStructure _song;
        Snippets _snippets;
        Swing* _swing;        

        int _playbackTickSpeed = 10000;

        boolean _playMemsInUse[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
        
        // up to four snippets can play in parallel.. the slots are listed in here
        int _playingSnippets[4] = {-1, -1, -1, -1};
        boolean _loopPlayingSnippets[4] = {false, false, false, false};
        int _playPositionSnippets[4] = {-1, -1, -1, -1};
        
        // {channel1, channel2, .. channel8} <- channels in sequencer filled with index to _playmems
        byte _snippetChannels[4][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};

        byte _getFreeChannel();
        void _setChannelFree(byte channel);
        int _freeChannelCount();

        // queue for polyphonic events. each entry corresponds to a playMemLive1..6
        // 0 = not playing 1..72 -> corresponding sample is playing 
        byte _polyMemIDs[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        float _tempVelocityL = 0.0;
        float _tempVelocityR = 0.0;
};

#endif