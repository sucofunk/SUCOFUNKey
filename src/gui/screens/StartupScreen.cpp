
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
#include "StartupScreen.h"
#include "components/SongSelector.h"

StartupScreen::StartupScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, char *activeSongName) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _activeSongName = activeSongName;
    _messageArea = {_screen->AREA_SCREEN.x1, _screen->AREA_SCREEN.y2-20, _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.y2-2, false, _screen->C_STARTUP_BG};
    _songSelector = SongSelector(_keyboard, _screen, _fsio, _activeSongName);
}

void StartupScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
    switch(_activeComponent) {
        case 1: 
            if (event.index != Sucofunkey::FN_ZOOM) {
                // send events to songSelector
                _songSelector.handleEvent(event);
            } else {
                // go to check mode by sending a global event to the main loop
                _keyboard->addApplicationEventToQueue(Sucofunkey::CHECKREQUEST);
            }
        
            break;
        default:
            break;
    }
}

long StartupScreen::receiveTimerTick() {
    switch(_activeComponent) {
        case 1: // send events to songSelector
            return _songSelector.receiveTimerTick();
            break;
        default:
            break;
    }
    return 100000;
}

void StartupScreen::showMessage(const char* message, boolean warning) {
    _screen->drawTextInArea(_messageArea, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, (warning ? _screen->C_WARNING : _screen->C_LIGHTGREY), message);
}

void StartupScreen::transitionToSelection() {
    _activeComponent = 1;
    _screen->clearAreaLTR(_screen->AREA_SCREEN, _screen->C_STARTUP_BG, 3);
    _screen->fadeBacklightOut(1);
    _drawSongSelector();        
    _screen->fadeBacklightIn(10);    
}


void StartupScreen::_drawSongSelector() {
    _songSelector.drawSongSelector();
};
