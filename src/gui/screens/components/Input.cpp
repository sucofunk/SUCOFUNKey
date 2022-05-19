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
   
#include "../../Screen.h"
#include "../../../helper/FSIO.h"
#include "Input.h"

// ToDo:
//  - parameterize keyboard type (filename or "complete")

Input::Input() {};

Input::Input(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, Screen::Area inputArea) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _inputArea = inputArea;
    _cursorArea = { _inputArea.x1, _inputArea.y1+10, _inputArea.x2, _inputArea.y2+10, false, _inputArea.bgColor };
}

long Input::receiveTimerTick() {
    _redrawCursor();
    return 350000;
}

void Input::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
        switch (event.index) {
            case Sucofunkey::SET:
                _keyboard->addApplicationEventToQueue(Sucofunkey::INPUT_SET);
                break;
            case Sucofunkey::FN_SET:
                _keyboard->addApplicationEventToQueue(Sucofunkey::INPUT_CANCEL);
                break;
            case Sucofunkey::CURSOR_LEFT:
                if (_cursorPosition > 0) _cursorPosition--;
                _currentCharIndex = 0;
                _redrawInput();
                break;    
            case Sucofunkey::CURSOR_RIGHT:
                if (_cursorPosition < 7) _cursorPosition++;
                _currentCharIndex = 0;
                _redrawInput();
                break;    
            default:
                if (event.type == Sucofunkey::KEY_NOTE || event.type == Sucofunkey::KEY_FN_NOTE) {                                   
                    _inputValue[_cursorPosition] = _keyboard->getFilenameCharByEventKey(event.index, _currentCharIndex);

                    if (_currentCharIndex == 1) { 
                         _currentCharIndex = 0;
                    } else {
                        _currentCharIndex = 1;
                    }                    
                    
                    _redrawInput();
                }
                break;
        }
    }
};

void Input::activateInput() {    
    _currentCharIndex = 0;
    _screen->fillArea(_inputArea, _screen->C_STARTUP_BG);
    _screen->fillArea(_cursorArea, _screen->C_STARTUP_BG);

    _screen->hr(_inputArea, 1.0, _screen->C_LIGHTGREY);
}

char *Input::getInputValue() {
    return _inputValue;
}

void Input::_redrawInput() {
    _screen->drawTextInArea(_inputArea, Screen::TEXTPOSITION_LEFT_VCENTER, true, Screen::TEXTSIZE_MEDIUM, true, _screen->C_WHITE, _inputValue);
    _redrawCursor();
}

void Input::_redrawCursor() {
    _cursorON = !_cursorON;

    if (_cursorON) {
        _cursorString[_lastCursorPosition] = '_';
        _screen->drawTextInArea(_cursorArea, Screen::TEXTPOSITION_LEFT_VCENTER, false, Screen::TEXTSIZE_MEDIUM, true, _screen->C_STARTUP_BG, _cursorString);        
        _cursorString[_lastCursorPosition] = ' ';

        _cursorString[_cursorPosition] = '_';
        _screen->drawTextInArea(_cursorArea, Screen::TEXTPOSITION_LEFT_VCENTER, false, Screen::TEXTSIZE_MEDIUM, true, _screen->C_ORANGE, _cursorString);
        _lastCursorPosition = _cursorPosition;        
    } else {
        _cursorString[_lastCursorPosition] = '_';
        _screen->drawTextInArea(_cursorArea, Screen::TEXTPOSITION_LEFT_VCENTER, false, Screen::TEXTSIZE_MEDIUM, true, _screen->C_STARTUP_BG, _cursorString);
        _cursorString[_lastCursorPosition] = ' ';
    }
}

void Input::showErrorMessage(char const * errorMessage, int showForMs) {
    _screen->drawTextInArea(_inputArea, Screen::TEXTPOSITION_LEFT_VCENTER, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_GREY, _inputValue);
    _screen->drawTextInArea(_inputArea, Screen::TEXTPOSITION_LEFT_VCENTER, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WARNING, errorMessage);
    delay(showForMs);
    _redrawInput();
};