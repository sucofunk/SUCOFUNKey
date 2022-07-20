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
   
#ifndef SamplerScreen_h
#define SamplerScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"
#include "../Screen.h"
#include "components/BottomMenu.h"
#include "components/SampleSelector.h"


class SamplerScreen {
    public:
        SamplerScreen();
        SamplerScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources);
        void handleEvent(Sucofunkey::keyQueueStruct event);

        void showEmptyScreen();      
        void clearScreen();
        void showSampleInfo(byte sampleId72, float volumeScaleFactor);        
        void showSampleInfo(byte bank0, byte sampleId0, float volumeScaleFactor);        
        void setBottomMenu(BottomMenu bottomMenu);
        void drawTrimMarker(int trimMarkerStartPosition, int trimMarkerEndPosition, byte sampleId72, float volumeScaleFactor);
        void drawTrimMarker(int trimMarkerStartPosition, int trimMarkerEndPosition, byte bank0, byte sampleId0, float volumeScaleFactor);
        void drawTrimMarkerOffsets(int startOffset, int endOffset);
        void removeTrimMarker(int position, byte sampleId72, float volumeScaleFactor);

        void showSlotSelectionHint();
        void showSavingMessage();

        void drawPlayerPosition(int x, int start, int end);
        void resetPlayerPosition();

        void showNoSampleInfo();
        void transitionToSelection();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;
        BottomMenu _bottomMenu;
        SampleSelector _sampleSelector;

        byte _activeComponent = 0; // 0 = Sampler, 1 = SampleSelector

        char _cBuff20[20];

        int zeroAxisY;
        boolean isPlayerPositionVisible = false;        

        void _drawSampleSelector();
        void _hideSampleSelector();
};

#endif
