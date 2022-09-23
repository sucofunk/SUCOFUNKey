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
   
#include "PianoKeyboard.h"

PianoKeyboard::PianoKeyboard() {};

PianoKeyboard::PianoKeyboard(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;
    _screen = screen;
};

void PianoKeyboard::handleEvent(Sucofunkey::keyQueueStruct event) {};

void PianoKeyboard::show() {
    _screen->fillArea(_screen->AREA_PIANO_ROLL, _screen->C_BLACK);
    _screen->drawTextInArea(_screen->AREA_PIANO_ROLL, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "KEYBOARD MODE");
//    _screen->drawLine(20, _screen->AREA_PIANO_ROLL.y2, 300, _screen->AREA_PIANO_ROLL.y2, _screen->C_WHITE);
//    _screen->drawLine(20, _screen->AREA_PIANO_ROLL.y2-10, 300, _screen->AREA_PIANO_ROLL.y2-10, _screen->C_WHITE);
    _visible = true;
    _lastBank = _keyboard->getBank();
    Serial.print("store bank setting::");
    Serial.println(_lastBank);
    _keyboard->setBank(2);
};

void PianoKeyboard::hide() {
    _screen->fillArea(_screen->AREA_PIANO_ROLL, _screen->C_BLACK);
    _visible = false;
    removeOriginReference();

    Serial.print("re-store bank setting::");
    Serial.println(_lastBank);

    _keyboard->setBank(_lastBank);
};

boolean PianoKeyboard::isVisible() {
    return _visible;
};

void PianoKeyboard::setOriginReference(byte channel, int position) {
    _originChannel = channel;
    _originPosition = position;
};

byte PianoKeyboard::getOriginChannel() {
    return _originChannel;
};

int PianoKeyboard::getOriginPosition() {
    return _originPosition;
};


void PianoKeyboard::removeOriginReference() {
    _originChannel = 127;
    _originPosition = -1;
};

boolean PianoKeyboard::hasOrigin() {
    if (_originPosition >= 0 && _originChannel != 127) return true;
    return false;
};
