/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2023 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#ifndef LiveScreen_h
#define LiveScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../context/sequencer/Play.h"
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "../../helper/SampleFSIO.h"

class LiveScreen {
    public:
        LiveScreen(){};
        LiveScreen(Sucofunkey* keyboard, Screen* screen, SampleFSIO* sfsio);

        enum Options {
            NONE = 0,
            MIDI_NOTE_IN_USE = 1,
            MIDI_NOTE_FREE = 2
        };


        void showEmptyOverview(boolean createAreas);
        void drawOverviewSlot(Play::LiveSlotDefinitionStruct slot, int slotNumber1);

        void drawBPM(float bpm);

        void showSlotTypeSelection(Play::LiveSlotDefinitionStruct slot, boolean initial);     
        void showSelectSnippet(Play::LiveSlotDefinitionStruct slot, boolean initial);
        void showSnippetConfig(Play::LiveSlotDefinitionStruct slot);
        void updateSnippetConfig(Play::LiveSlotDefinitionStruct slot, int encoder, boolean push, Options option); // encoder: 1..4 and 5..8 (with FN hold)

        void drawMIDIinWaitForTraining(boolean show);

        void showSelectSample(Play::LiveSlotDefinitionStruct slot, boolean initial);
        void showSampleConfig(Play::LiveSlotDefinitionStruct slot);
        void updateSampleConfig(Play::LiveSlotDefinitionStruct slot, int encoder, boolean push, Options option); // encoder: 1..4 and 5..8 (with FN hold)
        void showPianoSampleSelectMessage(boolean show);
        void showPianoMessage(boolean show);

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        SampleFSIO* _sfsio;

        char _cBuff3[3];
        char _cBuff5[5];
        char _cBuff10[10];
        int _centerLineY = 105;
        int _tempInt;

        Screen::Area _slotTypeSelectionAreaLeft;
        Screen::Area _slotTypeSelectionAreaCenter;
        Screen::Area _slotTypeSelectionAreaRight;
        Screen::Area _slotAreas[24];
        Screen::Area _bpmLabelArea;
        Screen::Area _bpmValueArea;

        Screen::Area _snippetNameArea;
        Screen::Area _snippetsCompleteArea;
        Screen::Area _snippetsLoopArea;        
        Screen::Area _snippetsMIDIArea;        
        
        Screen::Area _sampleScratchArea;
        Screen::Area _sampleDirectionArea;
        Screen::Area _sampleCompleteArea;

        Screen::Area _waveFormArea;

        void _drawSampleWaveform(int sampleId72, boolean reverse);
};

#endif
