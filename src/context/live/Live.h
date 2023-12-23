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
   
#ifndef Live_h
#define Live_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/LiveScreen.h"
#include "../sequencer/Play.h"

#include <MIDI.h>

class Live {
    public:
        
        Live(Sucofunkey* keyboard, Screen* screen, FSIO* fsio, SampleFSIO* sfsio, Play* play);

        enum LiveState {
            OVERVIEW = 0,
            SLOT_TYPE_SELECT = 1,
            WAIT_SNIPPET_SELECT = 2,
            WAIT_SAMPLE_SELECT = 3,
            CONFIG_SNIPPET = 4,
            CONFIG_SAMPLE = 5,
            WAIT_MIDI_TRAINING_INPUT_SNIPPET = 6,
            WAIT_MIDI_TRAINING_INPUT_SAMPLE = 7,
            WAIT_PIANO_SAMPLE_SELECT = 8,
            PIANO = 9
        };

        void setActive(boolean active);
        long receiveTimerTick();

        void handleEvent(Sucofunkey::keyQueueStruct event);
        void receiveMidiData(byte channel, midi::MidiType type, int d1, int d2);

        void loadConfig();
        void saveConfig();


    private:
        Sucofunkey* _keyboard;
        Screen* _screen;
        FSIO* _fsio;
        SampleFSIO* _sfsio;
        Play* _play;

        LiveScreen _liveScreen;

        volatile int _blinkPosition = 0;
        boolean _isActive = false;
        byte _activeBank = 1;
        byte _sampleBank = 1;
        byte _pianoBank = 2;

        LiveState _currentState = OVERVIEW;

        byte _editingSlotBank = 0;
        byte _editingSlotKey = 0;
        int _editingSlotId = -1;
        
        int _holdSlotIndex = -1; // used to determine, if a key is hold to chain another snippet/sample
        int _holdSlotAction = false; // used to determine, if another snippet/sample was selected while holding the key

        float _bpm;

        Play::LiveSlotDefinitionStruct _slots[72];

        int _playbackTickSpeed = 10000;
        boolean _playLEDon = false;
        boolean _isInitialized = false;
        boolean _LEDHighlightSlots[72];

        // Snippets are stored with slotIndex (0..71) and -1 if not in use
        int _midiNoteToSlot[128];

        int _pianoSampleSlotIndex = -1;

        void _changeState(LiveState state);
        void _overview(boolean initialize);
        void _playSlot(int slotIndex, byte velocity, boolean pressed, byte note);
        void _updateAllLoopingLEDs();

        void _handleSlotTypeSelection(byte bank, byte key, boolean initialize);
        void _cancel();
        void _cancel(boolean showOverview);
        void _changeBPM(float bpm);

        void _handleEncoders(byte encoder, boolean function, int action);
        void _handleOverviewEncoders(byte encoder, boolean function, int action);
        void _handleSnippetConfiguration(byte encoder, boolean function, int action);
        void _handleSampleConfiguration(byte encoder, boolean function, int action);

        boolean _setMIDINote(byte note);
        byte _getMidiNoteForSlot(int slotIndex);
        boolean _saveAndActivateMIDINoteForSlot(int slotIndex);

        void _removeSlot(int slotIndex);

        void _playPiano(byte note, byte velocity, boolean play);
};

#endif
