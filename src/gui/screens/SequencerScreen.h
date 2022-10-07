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
   
#ifndef SequencerScreen_h
#define SequencerScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"
#include "../Screen.h"
#include "../../context/sequencer/SongStructure.h"
#include "../../context/sequencer/Swing.h"
#include "../../context/sequencer/Zoom.h"
#include "../../context/sequencer/Selection.h"
#include "../../context/sequencer/Play.h"

class SequencerScreen {
    public:
        SequencerScreen();
        SequencerScreen(Sucofunkey* keyboard, Screen* screen, SampleFSIO* sfsio, AudioResources* audioResources, Zoom* zoom, Play* play, Selection* selection);

        enum LastAction {
                INIT = 0,
                UP = 1,
                RIGHT = 2,
                DOWN = 3,
                LEFT = 4,
                SCROLL_LEFT = 5,
                SCROLL_RIGHT = 6,
                APPEND = 7,
                SHORTEN = 8,
                RESOLUTION = 9,
                SCALE = 10,
                SELECTION = 11,
                SNIPPET_DELETE = 12
        };

        void initializeGrid(SongStructure *pattern, uint16_t cursorPosition); 
        void drawGrid(LastAction action);
        void drawCursorAt(byte channel, uint16_t position, boolean draw);
        void drawSample(byte channel, uint16_t position, boolean drawBackground);
        void drawSamples();

        boolean viewportCheckUpdate(byte channel, uint16_t position, LastAction action);
        void drawPlayStepIndicator(uint16_t position, boolean draw);

        void showSampleInfos(byte channel, uint16_t position, boolean fullInfo);
        void showMidiNoteInfos(byte channel, uint16_t position);
        void updateMidiChannel(byte channel, uint16_t position);
        void updateSampleInfoVolume(byte channel, uint16_t position);
        void updateSampleInfoPanning(byte channel, uint16_t position);
        void updateSampleInfoPitch(byte channel, uint16_t position);
        void updateSampleInfoProbability(byte channel, uint16_t position);
        void hideSampleInfos();

        void drawBPM(float bpm);
        void drawSwingInfoFromExpression(byte expression);
        void drawSwingInfo(byte level, byte group);
        void drawExtMemPercentage(byte percent);

        void drawSelection(Selection *selection);
        void drawSnippets();

    private:
        Sucofunkey* _keyboard;
        Screen* _screen;
        FSIO* _fsio;
        SampleFSIO* _sfsio;
        AudioResources *_audioResources;

        Zoom* _zoom;
        Selection* _selection;

        Play* _play;

        char _cBuff3[3];
        char _cBuff2[2];
        char _cBuff10[10];

        char _cBuff5_1[5];
        char _cBuff5_2[5];
        char _cBuff5_3[5];
        char _cBuff5_4[5];                

        int _x;
        int _y;

        int _tempInt = 0;

        uint16_t _xPositionOffset = 0;    // if we move the cursor to the right at the end of the screen, the pattern moves and starts with the offset on the left
        uint16_t _xPositionCapacity = 16; // available columns
        uint8_t  _cellWidth = 19;
        uint8_t  _cellHeight = 15;

        uint8_t _cursorChannel = 0;
        uint16_t _cursorPosition = 0;

        SongStructure* _song;
        Swing* _swing;
        SongStructure::sampleStruct _tempSample;

        uint8_t _amountOfGridCellsToDraw();
        boolean _sampleInfosVisible = false;

        void _drawSelection();
        void _drawSnippet(Selection::SelectionStruct snippet);
};

#endif
