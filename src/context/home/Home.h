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
   
#ifndef Home_h
#define Home_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../gui/screens/HomeScreen.h"
#include "../../gui/screens/components/BottomMenu.h"

class Home {
    public:
        Home(Sucofunkey *keyboard, Screen *screen, char *activeSongPath, char *activeSongName);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        char *_activeSongPath;
        char *_activeSongName;
        boolean _isActive = false;

        BottomMenu _bottomMenu;
        HomeScreen _homeScreen;        
};

#endif
