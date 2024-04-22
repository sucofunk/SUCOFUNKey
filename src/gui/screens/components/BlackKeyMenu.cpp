/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2024 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#include "BlackKeyMenu.h"

BlackKeyMenu::BlackKeyMenu() {};

BlackKeyMenu::BlackKeyMenu(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;
    _screen = screen;
};


void BlackKeyMenu::setOptions(char const *o1, char const *o2, char const *o3, char const *o4, char const *o5, char const *o6, char const *o7, char const *o8, char const *o9, char const *o10) {
    _label1 = o1;
    _label2 = o2;
    _label3 = o3;
    _label4 = o4;
    _label5 = o5;
    _label6 = o6;
    _label7 = o7;
    _label8 = o8;
    _label9 = o9;
    _label10 = o10;
};

void BlackKeyMenu::setOption(uint8_t option, char const *label) {
    switch(option) {
        case 1:
            _label1 = label;
            break;
        case 2:
            _label2 = label;
            break;
        case 3:
            _label3 = label;
            break;
        case 4:
            _label4 = label;
            break;
        case 5:
            _label5 = label;
            break;
        case 6:
            _label6 = label;
            break;
        case 7:
            _label7 = label;
            break;
        case 8:
            _label8 = label;
            break;
        case 9:
            _label9 = label;
            break;
        case 10:
            _label10 = label;
            break;
        default:
            break;
    }
};


void BlackKeyMenu::setExclusiveAction(byte position, boolean activated) {
    if (activated) {
        _exclusivePosition = position;
        _keyboard->setLEDState(_ledPINs[position-1], true);
    } else {
        _exclusivePosition = 0;
        _keyboard->setLEDState(_ledPINs[position-1], false);

        for (int i=0; i<10; i++) {
            _allowedToExclusive[i] = false;
        }
    }

    for (int i=1; i<=10; i++) {
        _redrawOption(i);
    }
};

// 1..10
void BlackKeyMenu::allowAdditionalToExclusive(byte position) {
    _allowedToExclusive[position-1] = true;
    _redrawOption(position);    
};

void BlackKeyMenu::disableExceptions() {
    for (int i=0; i<10; i++) {        
        _allowedToExclusive[i] = false;
        _redrawOption(i+1);
    }        
}


// 1..10
boolean BlackKeyMenu::_isAllowed(byte position) {
    return _allowedToExclusive[position-1];
}


void BlackKeyMenu::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed && event.type == Sucofunkey::KEY_NOTE) {
        switch(event.index) {
            case Sucofunkey::FS_1:
                if (_exclusivePosition == 0 || _exclusivePosition == 1 || _isAllowed(1)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM1);
                }
                break;
            case Sucofunkey::GS_1:
                if (_exclusivePosition == 0 || _exclusivePosition == 2 || _isAllowed(2)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM2);
                }
                break;
            case Sucofunkey::AS_1:
                if (_exclusivePosition == 0 || _exclusivePosition == 3 || _isAllowed(3)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM3);
                }
                break;
            case Sucofunkey::CS_1:
                if (_exclusivePosition == 0 || _exclusivePosition == 4 || _isAllowed(4)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM4);
                }
                break;
            case Sucofunkey::DS_1:
                if (_exclusivePosition == 0 || _exclusivePosition == 5 || _isAllowed(5)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM5);
                }
                break;
            case Sucofunkey::FS_2:
                if (_exclusivePosition == 0 || _exclusivePosition == 6 || _isAllowed(6)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM6);
                }
                break;
            case Sucofunkey::GS_2:
                if (_exclusivePosition == 0 || _exclusivePosition == 7 || _isAllowed(7)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM7);
                }
                break;
            case Sucofunkey::AS_2:
                if (_exclusivePosition == 0 || _exclusivePosition == 8 || _isAllowed(8)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM8);
                }
                break;
            case Sucofunkey::CS_2:
                if (_exclusivePosition == 0 || _exclusivePosition == 9 || _isAllowed(9)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM9);
                }
                break;
            case Sucofunkey::DS_2:
                if (_exclusivePosition == 0 || _exclusivePosition == 10 || _isAllowed(10)) { 
                    _keyboard->addApplicationEventToQueue(_keyboard->BLACKKEY_NAV_ITEM10);
                }
                break;
            default:
                break;
        }
    }
};

void BlackKeyMenu::showMenu() {
    _screen->fillArea(_screen->AREA_BOTTOM_MENU, _screen->C_BLACK);
    _visible = true;
    _redrawMenu();
};


void BlackKeyMenu::hideMenu() {
    _visible = false;
    _screen->fillArea(_screen->AREA_BOTTOM_MENU, _screen->C_BLACK);
    //_screen->fillArea(_screen->AREA_BLACK_KEY_MENU, _screen->C_BLACK);

    removeExclusiveAndExceptions(false);
};


void BlackKeyMenu::removeExclusiveAndExceptions(boolean redraw) {
    // turn all LEDs off, if one is on AND delete all exclusive exceptions
    for (int i=0; i<10; i++) {
        _keyboard->setLEDState(_ledPINs[i], false);
        _allowedToExclusive[i] = false;
    }
    
    // stop exclusive position -> hiding the menu is like canceling an option
    _exclusivePosition = 0;

    if (redraw) {
        _redrawMenu();
    }    
}


boolean BlackKeyMenu::isVisible() {
    return _visible;    
};

void BlackKeyMenu::_redrawMenu() {
    _redrawFrame();
    
    for (int i=1; i<=10; i++) {
        _redrawOption(i);
    }
};

void BlackKeyMenu::_redrawFrame() {

    // vertical lines
    _screen->drawLine(0, _screen->AREA_BLACK_KEY_MENU.y1, 0, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);    
    _screen->drawLine(26, _screen->AREA_BLACK_KEY_MENU.y1, 26, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_MEDIUM);
    _screen->drawLine(52, _screen->AREA_BLACK_KEY_MENU.y1, 52, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_MEDIUM);         
    _screen->drawLine(78, _screen->AREA_BLACK_KEY_MENU.y1, 78, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);    

    _screen->drawLine(98, _screen->AREA_BLACK_KEY_MENU.y1, 98, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);
    _screen->drawLine(124, _screen->AREA_BLACK_KEY_MENU.y1, 124, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_MEDIUM);
    _screen->drawLine(150, _screen->AREA_BLACK_KEY_MENU.y1, 150, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);

    _screen->drawLine(170, _screen->AREA_BLACK_KEY_MENU.y1, 170, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);
    _screen->drawLine(196, _screen->AREA_BLACK_KEY_MENU.y1, 196, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_MEDIUM);
    _screen->drawLine(222, _screen->AREA_BLACK_KEY_MENU.y1, 222, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_MEDIUM);
    _screen->drawLine(248, _screen->AREA_BLACK_KEY_MENU.y1, 248, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);

    _screen->drawLine(267, _screen->AREA_BLACK_KEY_MENU.y1, 267, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);
    _screen->drawLine(293, _screen->AREA_BLACK_KEY_MENU.y1, 293, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_MEDIUM);
    _screen->drawLine(319, _screen->AREA_BLACK_KEY_MENU.y1, 319, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);    

    // horizontal lines
    _screen->drawLine(0, _screen->AREA_BLACK_KEY_MENU.y1, 78, _screen->AREA_BLACK_KEY_MENU.y1, _screen->C_GRID_BRIGHT);    
    _screen->drawLine(1, _screen->AREA_BLACK_KEY_MENU.y2, 78, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);    

    _screen->drawLine(98, _screen->AREA_BLACK_KEY_MENU.y1, 150, _screen->AREA_BLACK_KEY_MENU.y1, _screen->C_GRID_BRIGHT);
    _screen->drawLine(98, _screen->AREA_BLACK_KEY_MENU.y2, 150, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);

    _screen->drawLine(170, _screen->AREA_BLACK_KEY_MENU.y1, 247, _screen->AREA_BLACK_KEY_MENU.y1, _screen->C_GRID_BRIGHT);
    _screen->drawLine(170, _screen->AREA_BLACK_KEY_MENU.y2, 247, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);

    _screen->drawLine(267, _screen->AREA_BLACK_KEY_MENU.y1, 319, _screen->AREA_BLACK_KEY_MENU.y1, _screen->C_GRID_BRIGHT);
    _screen->drawLine(267, _screen->AREA_BLACK_KEY_MENU.y2, 319, _screen->AREA_BLACK_KEY_MENU.y2, _screen->C_GRID_BRIGHT);
};

void BlackKeyMenu::_redrawOption(uint8_t option) {
    switch(option) {
        case 1:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM1, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM1, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 1 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[0]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label1);
            break;
        case 2:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM2, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM2, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 2 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[1]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label2);
            break;            
        case 3:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM3, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM3, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 3 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[2]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label3);
            break;
        case 4:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM4, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM4, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 4 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[3]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label4);
            break;                
        case 5:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM5, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM5, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 5 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[4]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label5);
            break;            
        case 6:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM6, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM6, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 6 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[5]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label6);
            break;
        case 7:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM7, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM7, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 7 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[6]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label7);
            break;
        case 8:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM8, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM8, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 8 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[7]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label8);
            break;
        case 9:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM9, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM9, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 9 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[8]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label9);
            break;
        case 10:
            _screen->fillArea(_screen->AREA_BLACK_KEY_MENU_ITEM10, _screen->C_BLACK);
            _screen->drawTextInArea(_screen->AREA_BLACK_KEY_MENU_ITEM10, Screen::TEXTPOSITION_HCENTER_BOTTOM , false, Screen::TEXTSIZE_SMALL, false, _exclusivePosition == 10 ? _screen->C_ORANGE : ((_exclusivePosition != 0 && _allowedToExclusive[9]) ? _screen->C_WHITE : (_exclusivePosition == 0 ? _screen->C_WHITE : _screen->C_GRID_BRIGHT)), _label10);
            break;                                                            
    }
};
