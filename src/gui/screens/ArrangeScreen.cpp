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
   
#include "ArrangeScreen.h"

ArrangeScreen::ArrangeScreen(Sucofunkey* keyboard, Screen* screen, SampleFSIO* sfsio) {
    _keyboard = keyboard;
    _screen = screen;        
    _sfsio = sfsio;
}

void ArrangeScreen::showEmptyOverview() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);
    
    for (int i=0; i<=10; i++) {
        _screen->drawFastHLine(0, _screen->AREA_CONTENT.y1 + (i*19), 320, _screen->C_GRID_DARK);
    }
    
    for (int i=0; i<=6; i++) {
        _screen->drawFastVLine(i*53, _screen->AREA_CONTENT.y1, _screen->AREA_CONTENT.y2-_screen->AREA_CONTENT.y1, _screen->C_GRID_DARK);
    }


//    _screen->drawFastHLine(offsetLR + static_cast<int>(0.5*cellSize), sharpieTopLineY, 3*cellSize+4, _screen->C_WHITE);
//    _screen->drawFastHLine(offsetLR + static_cast<int>(0.5*cellSize), sharpieBottomLineY, 3*cellSize+4, _screen->C_WHITE);

}