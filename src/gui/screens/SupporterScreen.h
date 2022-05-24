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
   
#ifndef SupporterScreen_h
#define SupporterScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../Screen.h"

class SupporterScreen {
    public:
        SupporterScreen();
        SupporterScreen(Sucofunkey *keyboard, Screen *screen);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void show();        
    private:
        typedef struct  {
            int x = 64;
            int y = 64;
            int z = 0;
            int lastXPixel = 0;
            int lastYPixel = 0;
            int speed = 0;
        } Star;

        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _isRunning = false;
        void _update();

        Star stars[50];
        int _offsetX;
        int _offsetY;
        float _scaleZx;
        float _scaleZy;

        uint16_t _colors[4] = {_screen->RGBtoColor(125, 125, 125), _screen->RGBtoColor(150, 150, 150), _screen->RGBtoColor(200, 200, 200), _screen->RGBtoColor(254, 254, 254)};

        char _textBuffer[50];
        uint8_t _updatesCount = 0;
        uint8_t _supporterCount = 7;
        uint8_t _supporterPosition = 0;
        String _supporterText[7] = {"Thanks for your support... ", "Richie Hawtin ", "Halil Kleinmann ", "MrKabelbruch ", "STRANGE BOUND ", "www.sucofunk.com/donate ", "www.sucofunk.com/donate "};
};

#endif
