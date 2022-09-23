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
   
#ifndef PianoKeyboard_h
#define PianoKeyboard_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"

class PianoKeyboard {
    public:
        PianoKeyboard();
        PianoKeyboard(Sucofunkey *keyboard, Screen *screen);
        
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void show();
        void hide();

        boolean isVisible();
        
        // position on grid to "copy" when moving to an empty cell in piano mode
        void setOriginReference(byte channel, int position);
        byte getOriginChannel();
        int getOriginPosition();
        void removeOriginReference();
        boolean hasOrigin();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _visible = false;
        
        byte _originChannel = 127;
        int _originPosition = -1;
        byte _lastBank;
};

#endif