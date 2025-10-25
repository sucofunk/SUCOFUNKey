/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2025 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#ifndef Settings_h
#define Settings_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/SettingsScreen.h"

class Settings {
    public:
        Settings(Sucofunkey *keyboard, Screen *screen, FSIO* fsio);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO* _fsio;
        SettingsScreen _settingsScreen;
        boolean _isActive = false;
        byte _activeBank = 1;
        int _activeOption = 1; // 1..n
        int _optionsCount = 5;        

        void _drawAllOptions();
        void _updateOption(int position1, boolean active); // trigger drawing an option
        void _changeOptionValue(boolean increase); // increase = true (cursor right), decrease = false (cursor left) for _activeOption
        void _changeMidiChannel(int *value, boolean increase); // increase = true (cursor right), decrease = false (cursor left)
        void _toggleTrueFalse(bool *value); // for _activeOption
};

#endif
