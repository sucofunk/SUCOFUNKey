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
    _bottomMenu = BottomMenu(_keyboard, _screen, "Instructions", 0, "Settings", 0, "Supporter", 0);
    _bottomMenu.disableItem(1);
    _bottomMenu.disableItem(2);
    _bottomMenu.selectItem(3);
}

void Home::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (_homeScreen.passEventsToMe()) {
      _homeScreen.handleEvent(event);
      return;
    }

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT: 
              if (_bottomMenu.isVisible()) {
                _bottomMenu.handleEvent({_keyboard->CURSOR_LEFT, true, false, _keyboard->KEY_OPERATION});
              }
              break;
        case Sucofunkey::CURSOR_RIGHT:
              if (_bottomMenu.isVisible()) {
                _bottomMenu.handleEvent({_keyboard->CURSOR_RIGHT, true, false, _keyboard->KEY_OPERATION});
              }
              break;
        case Sucofunkey::SET:
              if (_bottomMenu.isVisible()) {
                _bottomMenu.handleEvent({_keyboard->SET, true, false, _keyboard->KEY_OPERATION});
              }
              break;              
        case Sucofunkey::MENU:
              if (_bottomMenu.isVisible()) {
                _bottomMenu.showMenu(false);
              } else {
                _bottomMenu.showMenu(true);
              }
              break;
      }

      if (event.type == Sucofunkey::EVENT_APPLICATION) {
        switch(event.index) {
          case Sucofunkey::BOTTOM_NAV_ITEM3:
            _bottomMenu.showMenu(false);
            _homeScreen.showSupporterScreen();
            break;
          default: 
            break;  
        }
      }
    }

}

long Home::receiveTimerTick() {
  return _homeScreen.receiveTimerTick();
}


void Home::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(0);
    _homeScreen.showGeneralInformation();
    _bottomMenu.showMenu(false);
  } else {
    _isActive = false;
    _keyboard->setBank(0);
    _bottomMenu.showMenu(false);
  }
}
