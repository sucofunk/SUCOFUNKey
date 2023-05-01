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
   
#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Home.h"

Home::Home(Sucofunkey *keyboard, Screen *screen, char *activeSongPath, char *activeSongName) {
    _keyboard = keyboard;    
    _screen = screen;
    _activeSongPath = activeSongPath;
    _activeSongName = activeSongName;
    _homeScreen = HomeScreen(_keyboard, _screen, _activeSongName);
    
    _blackKeyMenu = BlackKeyMenu(_keyboard, _screen);
    _blackKeyMenu.setOption(1, "SNG");  // song selector
    _blackKeyMenu.setOption(4, "LIN");  // route line in trough
    _blackKeyMenu.setOption(5, "CFG");  // settings/config
    _blackKeyMenu.setOption(10, "SUP"); // supporter screen
}

void Home::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (_homeScreen.passEventsToMe()) {
      _homeScreen.handleEvent(event);
      return;
    }

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::MENU:
              if (_blackKeyMenu.isVisible()) {
                _blackKeyMenu.hideMenu();
              } else {
                _blackKeyMenu.showMenu();
              }
              break;
      }

      if (event.type == Sucofunkey::EVENT_APPLICATION) {
        switch(event.index) {
          case Sucofunkey::BLACKKEY_NAV_ITEM1:
            _blackKeyMenu.hideMenu();
            _homeScreen.showSongSelector();
            break;

          case Sucofunkey::BLACKKEY_NAV_ITEM10:
            _blackKeyMenu.hideMenu();
            _homeScreen.showSupporterScreen();
            break;

          default: 
            break;  
        }
      }

      if (event.type == Sucofunkey::KEY_NOTE) {
        // handle menu keys
        if (_blackKeyMenu.isVisible()) {
          _blackKeyMenu.handleEvent(event);
        } 
      }
    } // event pressed
}

long Home::receiveTimerTick() {
  return _homeScreen.receiveTimerTick();
}


void Home::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(0);
    _homeScreen.showGeneralInformation();
    _blackKeyMenu.showMenu();
  } else {
    _isActive = false;
    _keyboard->setBank(0);
    _blackKeyMenu.hideMenu();
  }
}
