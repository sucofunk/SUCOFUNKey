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
#include "../../context/sequencer/Zoom.h"

class SequencerScreen {
    public:
        SequencerScreen();
        SequencerScreen(Sucofunkey *keyboard, Screen *screen, SampleFSIO *sfsio, AudioResources *audioResources, Zoom *zoom);

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
                SCALE = 10
        };

        void initializeGrid(SongStructure *pattern, uint16_t cursorPosition); 
        void drawGrid(LastAction action);
        void drawCursorAt(byte channel, uint16_t position, boolean draw);
        void drawSample(byte channel, uint16_t position, boolean drawBackground);
        void drawSamples();

        boolean viewportCheckUpdate(byte channel, uint16_t position, LastAction action);
        void drawPlayStepIndicator(uint16_t position, boolean draw);

        void showSampleInfos(byte channel, uint16_t position, boolean fullInfo);
        void updateSampleInfoVolume(byte channel, uint16_t position);
        void updateSampleInfoPanning(byte channel, uint16_t position);
        void updateSampleInfoPitch(byte channel, uint16_t position);
        void updateSampleInfoProbability(byte channel, uint16_t position);
        void hideSampleInfos();

        void drawBPM(float bpm);
        void drawExtMemPercentage(byte percent);

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;

        Zoom *_zoom;

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
        uint16_t _xPositionCapacity = 21; // available columns

        SongStructure *_song;
        SongStructure::sampleStruct _tempSample;

        uint8_t _amountOfGridCellsToDraw();
        boolean _sampleInfosVisible = false;
};

#endif
