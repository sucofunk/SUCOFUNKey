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
}

void Home::handleEvent(Sucofunkey::keyQueueStruct event) {
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
    }
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
