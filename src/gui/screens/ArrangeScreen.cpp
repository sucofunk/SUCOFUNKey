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
    
    for (int i=0; i<=rows; i++) {
        _screen->drawFastHLine(0, _screen->AREA_CONTENT.y1 + (i*(190/rows)), 320, _screen->C_GRID_DARK);
    }
    
    for (int i=0; i<=columns; i++) {
        _screen->drawFastVLine(i*(320/columns) - (i > 0 ? 1 : 0), _screen->AREA_CONTENT.y1, _screen->AREA_CONTENT.y2-_screen->AREA_CONTENT.y1, _screen->C_GRID_DARK);
    }
}

void ArrangeScreen::drawCursor(int position, boolean highlight) {
    int row = position / columns;
    int column = position - (row * columns);

    _screen->drawFastHLine(column*(320/columns) - (column > 0 ? 1 : 0), _screen->AREA_CONTENT.y1 + (row*(190/rows)), (320/columns), highlight ? _screen->C_ORANGE : _screen->C_GRID_DARK);  
    _screen->drawFastHLine(column*(320/columns) - (column > 0 ? 1 : 0), _screen->AREA_CONTENT.y1 + ((row+1)*(190/rows)), (320/columns), highlight ? _screen->C_ORANGE : _screen->C_GRID_DARK);  

    _screen->drawFastVLine(column*(320/columns) - (column > 0 ? 1 : 0), _screen->AREA_CONTENT.y1 + (row*(190/rows)), (190/rows), highlight ? _screen->C_ORANGE : _screen->C_GRID_DARK);  
    _screen->drawFastVLine((column+1)*(320/columns) - 1, _screen->AREA_CONTENT.y1 + (row*(190/rows)), (190/rows), highlight ? _screen->C_ORANGE : _screen->C_GRID_DARK);
};

void ArrangeScreen::annotateCell(int position, int sheet, int repeat) {
    int row = position / columns;
    int column = position - (row * columns);

    Screen::Area tempArea = { column*(320/columns) - (column > 0 ? 1 : 0) + 1 , _screen->AREA_CONTENT.y1 + (row*(190/rows) + 2), 
                              column*(320/columns) - (column > 0 ? 1 : 0) + (320/columns) - 1, _screen->AREA_CONTENT.y1 + (row*(190/rows)) + (190/rows) - 1, false, _screen->C_BLACK};

    if (sheet >= 1) {
        if (repeat > 0) {
            sprintf(_cBuff5, "%d +%d", sheet, repeat);
        } else {
            sprintf(_cBuff5, "%d", sheet);
        }
        
        _screen->drawTextInArea(tempArea, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_SMALL, false, _screen->C_WHITE, _cBuff5);
    } else {
        _screen->fillArea(tempArea, _screen->C_BLACK);
    }


};