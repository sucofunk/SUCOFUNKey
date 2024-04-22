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
   
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "HomeScreen.h"

HomeScreen::HomeScreen() {};

HomeScreen::HomeScreen(Sucofunkey *keyboard, Screen *screen, char *activeSongName) {
    _keyboard = keyboard;
    _screen = screen;    
    _activeSongName = activeSongName;
    _supporter = SupporterScreen(keyboard, screen);     
    _songSelector = SongSelector(_keyboard, _screen, _fsio, _activeSongName);
}

void HomeScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
        switch (_activeComponent) {
            case SUPPORTER:
                if (event.index == Sucofunkey::MENU_BACK) {                    
                    showGeneralInformation();
                } else {
                    _supporter.handleEvent(event);
                }                
                break;
            case SONGSELECTOR:
                if (event.index == Sucofunkey::MENU_BACK) {
                    showGeneralInformation();
                } else {
                    _songSelector.handleEvent(event);
                }                
                
                break;
            default:
                break;
        }
    }
}

boolean HomeScreen::passEventsToMe() {
    if (_activeComponent == SUPPORTER || _activeComponent == SONGSELECTOR) return true;
    return false;
}

long HomeScreen::receiveTimerTick() {
    if (_activeComponent != NONE) {
        if (_activeComponent == SUPPORTER) { return _supporter.receiveTimerTick(); }
        if (_activeComponent == SONGSELECTOR) { return _songSelector.receiveTimerTick(); }
    } else {
        return 100000;
    }
    
    return 100000;
}

void HomeScreen::showGeneralInformation() {
    _activeComponent = NONE;

    Screen::Area projectTitle = {_screen->AREA_SCREEN.x1, _screen->AREA_SCREEN.y1, _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.x1 + 90, false, _screen->C_BLACK};


    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    // Song title
    _screen->drawTextInArea(projectTitle, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _activeSongName);


    int encYPos = 140;
    int radius = 20;
    int menuSpacing = 5;
    int menuWidthHalf = 6; 
    int spacingWidth = 15;
    int legendSpacing = 15;


    int encX[4] = {40, 120, 200, 280};

//    Screen::Area encoder1Area = {encX[0]-20, encYPos + radius + legendSpacing, encX[0]+20, encYPos + radius + legendSpacing + 20, false, _screen->C_BLACK};
//    Screen::Area encoder2Area = {encX[1]-20, encYPos + radius + legendSpacing, encX[1]+20, encYPos + radius + legendSpacing + 20, false, _screen->C_BLACK};
//    Screen::Area encoder3Area = {encX[2]-20, encYPos + radius + legendSpacing, encX[2]+20, encYPos + radius + legendSpacing + 20, false, _screen->C_BLACK};
//    Screen::Area encoder4Area = {encX[3]-20, encYPos + radius + legendSpacing, encX[3]+20, encYPos + radius + legendSpacing + 20, false, _screen->C_BLACK};


    Screen::Area text1Area = {encX[0]-20, encYPos - radius - legendSpacing - 12, encX[0]+20, encYPos - radius - legendSpacing, false, _screen->C_BLACK};
    Screen::Area text2Area = {encX[1]-20, encYPos - radius - legendSpacing - 12, encX[1]+20, encYPos - radius - legendSpacing, false, _screen->C_BLACK};
    Screen::Area text3Area = {encX[2]-20, encYPos - radius - legendSpacing - 12, encX[2]+20, encYPos - radius - legendSpacing, false, _screen->C_BLACK};
    Screen::Area text4Area = {encX[3]-20, encYPos - radius - legendSpacing - 12, encX[3]+20, encYPos - radius - legendSpacing, false, _screen->C_BLACK};


    for (int i=0; i<4; i++) {
        // Encoder circle
        _screen->drawCircle(encX[i], encYPos, radius, false, _screen->C_WHITE);

        // Encoder Marker
        switch (i) {
            case 0:
                // bottom left
                _screen->fillRect(encX[i]-radius, encYPos+radius-spacingWidth, spacingWidth, spacingWidth, _screen->C_BLACK);
            break;            
            case 1:
                // top left
                _screen->fillRect(encX[i]-radius, encYPos-radius, spacingWidth, spacingWidth, _screen->C_BLACK);
            break;
            case 2:
                // top right
                _screen->fillRect(encX[i]+radius-spacingWidth, encYPos-radius, spacingWidth, spacingWidth, _screen->C_BLACK);
            break;
            case 3:
                // bottom right
                _screen->fillRect(encX[i]+radius-spacingWidth, encYPos+radius-spacingWidth, spacingWidth, spacingWidth, _screen->C_BLACK);            
            break;
            default:
            break;
        }
        

        // Burger Menu icon
        _screen->drawFastHLine(encX[i]-menuWidthHalf, encYPos, menuWidthHalf*2, _screen->C_LIGHTGREY);
        _screen->drawFastHLine(encX[i]-menuWidthHalf, encYPos-menuSpacing, menuWidthHalf*2, _screen->C_LIGHTGREY);
        _screen->drawFastHLine(encX[i]-menuWidthHalf, encYPos+menuSpacing, menuWidthHalf*2, _screen->C_LIGHTGREY);

    }
    
    // Text ENC 1,2,3,4
//    _screen->drawTextInArea(encoder1Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "ENC 1");
//    _screen->drawTextInArea(encoder2Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "ENC 2");
//    _screen->drawTextInArea(encoder3Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "ENC 3");
//    _screen->drawTextInArea(encoder4Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "ENC 4");


    // Text Sample, SKETCH, ARRANGE, PLAY
    _screen->drawTextInArea(text1Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_ORANGE, "SAMPLE");
    _screen->drawTextInArea(text2Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_ORANGE, "SKETCH");
    _screen->drawTextInArea(text3Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_ORANGE, "ARRANGE");
    _screen->drawTextInArea(text4Area, _screen->TEXTPOSITION_HCENTER_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_ORANGE, "PLAY");
   
};


void HomeScreen::showSupporterScreen() {    
    _activeComponent = SUPPORTER;
    _supporter.show();
}

void HomeScreen::showSongSelector() {
    _activeComponent = SONGSELECTOR;
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);
    _songSelector.drawSongSelector();
}

