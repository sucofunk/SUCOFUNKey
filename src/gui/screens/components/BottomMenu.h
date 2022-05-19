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
   
#ifndef BottomMenu_h
#define BottomMenu_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"

class BottomMenu {
    public:
        BottomMenu();
        BottomMenu(Sucofunkey *keyboard, Screen *screen);
        BottomMenu(Sucofunkey *keyboard, Screen *screen, char const *label1, int retVal1, char const *label3, int retVal3);
        BottomMenu(Sucofunkey *keyboard, Screen *screen, char const *label1, int retVal1, char const *label2, int retVal2, char const *label3, int retVal3);
        
        void handleEvent(Sucofunkey::keyQueueStruct event);
        
        void setupMenu3(char const *label1, int retVal1, char const *label2, int retVal2, char const *label3, int retVal3);
        void setupMenu2(char const *label1, int retVal1, char const *label3, int retVal3);
        
        void setMenuItems(byte itemCount);

        void setItem1Label(char const *label);
        void setItem2Label(char const *label);
        void setItem3Label(char const *label);
        void setItem1CallbackValue(int retVal);
        void setItem2CallbackValue(int retVal);
        void setItem3CallbackValue(int retVal);

        void showMenu(boolean visible);
        boolean isVisible();
        //void activateMenu(boolean active);
        void selectItem(byte item);
        void disableItem(byte item);

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        byte _itemCount = 0;
        byte _selectedItem = 1;
        boolean _visible = false;
        char const *_label1;
        char const *_label2;
        char const *_label3;
        int _retVal1;
        int _retVal2;
        int _retVal3;
        boolean _disabled1;
        boolean _disabled2;
        boolean _disabled3;                
        void _redrawMenu();
};

#endif