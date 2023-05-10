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
   
#include "SupporterScreen.h"

SupporterScreen::SupporterScreen() {};

SupporterScreen::SupporterScreen(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;
    _screen = screen;    
}

void SupporterScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
    _isRunning = false;
    _keyboard->addApplicationEventToQueue(Sucofunkey::MENU_BACK);
}

long SupporterScreen::receiveTimerTick() {
    if (_isRunning) {
        _update();        
    }
    return 15000;
}

void SupporterScreen::show() {
    _screen->fadeBacklightOut(2);
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);
    _screen->fadeBacklightIn(0);

    // initializing Stars
    for (int i=0; i<50; i++) {
        stars[i].x = random(-160, 160);
        stars[i].y = random(-120, 120);
        stars[i].z = random(200);
        stars[i].speed = random(2, 8);
    }

    _isRunning = true;    
}

void SupporterScreen::_update() {
    _supporterText[_supporterPosition].toCharArray(_textBuffer, _supporterText[_supporterPosition].length());    

    // draw 50 stars..   
    for (int i=0; i<50; i++) {
        stars[i].z = stars[i].z - stars[i].speed;

        _scaleZx = 320.0/(320.0+stars[i].z);
        _scaleZy = 200.0/(200.0+stars[i].z);        

        _offsetX = static_cast<int>(stars[i].x * _scaleZx);
        _offsetY = static_cast<int>(stars[i].y * _scaleZy);
        
        if (_offsetX > -160 && _offsetX < 160 && _offsetY > -120 && _offsetY < 120) {
            _screen->drawPixel(stars[i].lastXPixel, stars[i].lastYPixel, _screen->C_BLACK);
            stars[i].lastXPixel = 160 + _offsetX;
            stars[i].lastYPixel = 120 + _offsetY;

            _screen->drawPixel(stars[i].lastXPixel, stars[i].lastYPixel, _colors[stars[i].speed % 4]);
        } else {
            stars[i].x = random(-160, 160);
            stars[i].y = random(-120, 120);
            stars[i].z = random(200);
        }
    }


    // draw Text
    _screen->drawTextInArea(_screen->AREA_SCREEN, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _supporterPosition == 0 ? _screen->C_WHITE :_screen->C_ORANGE, _textBuffer);

    if (_updatesCount > 120) {
        _updatesCount = 0;
        _screen->drawTextInArea(_screen->AREA_SCREEN, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_BLACK, _textBuffer);
        if (_supporterPosition < _supporterCount-1) {
            _supporterPosition++;
        } else {
            _supporterPosition = 0;
        }    
    }

    _updatesCount++;
    
}