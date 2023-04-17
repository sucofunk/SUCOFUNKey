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
   
#ifndef Check_h
#define Check_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"

class Check {
    public:
        Check(Sucofunkey *keyboard, Screen *screen);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void drawInfotext(char *msg);
        void encoderAction(byte encoder, byte action);       
        void turnAllLEDsOn();
        void showFaderPosition();
        void showVolume();
        void playSound();
        void stopSound();
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _isActive = false;
        byte _activeBank = 1;
        int _lastFaderPosition = 0;
        int _lastVolume = 0;
        char _tempChar[20];
};

#endif
