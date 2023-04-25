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
   
#ifndef SampleSelector_h
#define SampleSelector_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"
#include "../../../helper/FSIO.h"

class SampleSelector {
    public:
        SampleSelector();
        SampleSelector(Sucofunkey *keyboard, Screen *screen, FSIO *fsio);
        long receiveTimerTick();
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void drawSampleSelector();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        char *_activeDirectory;
        byte _activeComponent = 0;
        Screen::Area _AREA_SONGSELECTOR;
        Screen::Area _AREA_SONGSELECTOR_LINE_1;
        Screen::Area _AREA_SONGSELECTOR_LINE_2;
        Screen::Area _AREA_SONGSELECTOR_LINE_3;
        Screen::Area _AREA_SONGSELECTOR_LINE_4;
        Screen::Area _AREA_SONGSELECTOR_LINE_5;
        int _sampleCount = 0;
        int _activeItem = 0;
        int _offset = 0;
        boolean _counted = false;
        char _line1[40];
        char _line2[40];
        char _line3[40];
        char _line4[40];
        char _line5[40];
        boolean getLineDescription(int fileIndex, char *lineBuffer);
};

#endif