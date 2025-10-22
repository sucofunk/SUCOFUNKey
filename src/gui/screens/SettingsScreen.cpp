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
   
#include "SettingsScreen.h"

SettingsScreen::SettingsScreen(Sucofunkey* keyboard, Screen* screen) {
    _keyboard = keyboard;
    _screen = screen;
}
        
void SettingsScreen::initSettingsScreen() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);        
    _screen->drawTextInArea(_screen->AREA_HEADLINE, Screen::TEXTPOSITION_HCENTER_VCENTER, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Settings");
    _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU, Screen::TEXTPOSITION_HCENTER_VCENTER, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "Change values with < and > keys");
}

void SettingsScreen::drawOption(int position1, char *label, int value, Configuration::ValueType valueType, boolean active) {

    _screen->drawTextInArea(_screen->AREAS_CONTENT_LINES[position1-1], Screen::TEXTPOSITION_LEFT_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, active ? _screen->C_WHITE : _screen->C_LIGHTGREY, label);

    switch (valueType) {
        case Configuration::ValueType::MIDICHANNEL:
            itoa(value, _vC, 10);
            _screen->drawTextInArea(_screen->AREAS_SETTINGS_VALUES_LINES[position1-1], Screen::TEXTPOSITION_HCENTER_VCENTER, true, Screen::TEXTSIZE_MEDIUM, false, active ? _screen->C_WHITE : _screen->C_LIGHTGREY, _vC);
            break;
        case Configuration::ValueType::TRUEFALSE:
            _screen->drawTextInArea(_screen->AREAS_SETTINGS_VALUES_LINES[position1-1], Screen::TEXTPOSITION_HCENTER_VCENTER, true, Screen::TEXTSIZE_MEDIUM, false, active ? _screen->C_WHITE : _screen->C_LIGHTGREY, value == 1 ? "YES" : "NO");
        break;
        default:
            break;
    }
}

