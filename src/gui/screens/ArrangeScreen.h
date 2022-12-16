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
   
#ifndef ArrangeScreen_h
#define ArrangeScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../context/sequencer/Play.h"
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "../../helper/SampleFSIO.h"

class ArrangeScreen {
    public:
        ArrangeScreen(){};
        ArrangeScreen(Sucofunkey* keyboard, Screen* screen, SampleFSIO* sfsio);

        int rows = 10;
        int columns = 8;

        void showEmptyOverview();
        void drawCursor(int position, boolean highlight);
        void annotateCell(int position, int sheet, int repeat);

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        SampleFSIO* _sfsio;

        char _cBuff3[3];
        char _cBuff5[5];
        char _cBuff10[10];
};

#endif
