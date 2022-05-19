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
   
#ifndef StartupScreen_h
#define StartupScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "components/SongSelector.h"

class StartupScreen {
    public:
        StartupScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, char *activeSongName);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void showLogo();
        void hideLogo();
        void showMessage(const char* message, boolean warning);
        void transitionToSelection();
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        char *_activeSongName;
        char *_songsBasePath;
        Screen::Area _messageArea;
        SongSelector _songSelector;
        void _drawSongSelector();
        byte _activeComponent = 0; // 1 = SongSelector
};

#endif
