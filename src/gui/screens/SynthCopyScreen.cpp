/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2026 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#include "SynthCopyScreen.h"

SynthCopyScreen::SynthCopyScreen() {};

SynthCopyScreen::SynthCopyScreen(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;
    _screen = screen;    
}

void SynthCopyScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
    _isRunning = false;
    _keyboard->addApplicationEventToQueue(Sucofunkey::MENU_BACK);
}

long SynthCopyScreen::receiveTimerTick() {
    return 15000;
}

void SynthCopyScreen::show() {
    Screen::Area headlineArea = {0, 80, _screen->AREA_SCREEN.x2, 100, false, _screen->C_BLACK};

    _screen->fadeBacklightOut(2);
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawTextInArea(headlineArea, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Synthkopierer");


    _screen->fadeBacklightIn(0);  
}

