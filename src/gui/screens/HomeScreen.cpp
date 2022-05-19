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
   
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "HomeScreen.h"

HomeScreen::HomeScreen() {};

HomeScreen::HomeScreen(Sucofunkey *keyboard, Screen *screen, char *activeSongName) {
    _keyboard = keyboard;
    _screen = screen;    
    _activeSongName = activeSongName;
}

void HomeScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
}

long HomeScreen::receiveTimerTick() {
    return 100000;
}


void HomeScreen::showGeneralInformation() {
    Screen::Area text1 = {_screen->AREA_SCREEN.x1, 90, _screen->AREA_SCREEN.x2-21, 130, false, _screen->C_ORANGE};
    Screen::Area projectTitle = {_screen->AREA_SCREEN.x1, _screen->AREA_SCREEN.y1, _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.x1 + 90, false, _screen->C_BLACK};

    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawTextInArea(projectTitle, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _activeSongName);
    
    _screen->fillArea(text1, _screen->C_ORANGE);
    // right arrow
    _screen->drawTriangle(text1.x2, text1.y1, text1.x2, text1.y2, text1.x2+20, 110, true, _screen->C_ORANGE);

    // black triangle + rectancle |> to make the arrow "empty"
    _screen->drawTriangle(text1.x2-5, text1.y1, text1.x2-5, text1.y2, text1.x2+15, 110, true, _screen->C_BLACK);    
    _screen->fillRect(text1.x2-12, text1.y1, 7, 41, _screen->C_BLACK);
    
    // draw the left arrowhead
    _screen->drawTriangle(text1.x2-12, text1.y1, text1.x2-12, text1.y2, text1.x2+8, 110, true, _screen->C_ORANGE);    

    _screen->drawText("Sampler   Sequencer   Instruments   Live", _screen->AREA_SCREEN.x1 + 5, 115, _screen->TEXTSIZE_MEDIUM, _screen->C_WHITE);
};


void HomeScreen::showSupporterScreen() {
}