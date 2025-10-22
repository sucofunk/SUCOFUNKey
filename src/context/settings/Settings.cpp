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
   
#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Settings.h"

Settings::Settings(Sucofunkey *keyboard, Screen *screen, FSIO* fsio) {
    _keyboard = keyboard;    
    _screen = screen;
    _settingsScreen = SettingsScreen(keyboard, screen);
}

void Settings::handleEvent(Sucofunkey::keyQueueStruct event) {

    if (event.pressed) {
      switch(event.index) {
        case Sucofunkey::CURSOR_LEFT:             
          _changeOptionValue(false);
          break;
        case Sucofunkey::CURSOR_RIGHT:
          _changeOptionValue(true);
          break;
        case Sucofunkey::CURSOR_UP:
          if (_activeOption > 1) {
            _updateOption(_activeOption, false);
            _activeOption--;
            _updateOption(_activeOption, true);
          }    
          break;
        case Sucofunkey::CURSOR_DOWN:
          if (_activeOption < _optionsCount) {
            _updateOption(_activeOption, false);
            _activeOption++;
            _updateOption(_activeOption, true);
          }
          break;
      }
    }
}

void Settings::setActive(boolean active) {
  if (active) {
    _isActive = true;
    _keyboard->setBank(_activeBank);
    _settingsScreen.initSettingsScreen();
    _drawAllOptions();  
  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}

void Settings::_drawAllOptions() {
    for (int i=1; i<=_optionsCount; i++) {
      _updateOption(i, _activeOption == i ? true : false);
    }
}

void Settings::_updateOption(int position1, boolean active) {
  switch (position1) {
    case 1:
      _settingsScreen.drawOption(1, "MIDI channel Play", _keyboard->getConfig()->configurationValues.midiChannelPlay, _keyboard->getConfig()->configurationValues.midiChannelPlayValueType, active);
      break;
    case 2:
      _settingsScreen.drawOption(2, "MIDI channel Piano", _keyboard->getConfig()->configurationValues.midiChannelPiano, _keyboard->getConfig()->configurationValues.midiChannelPianoValueType, active); 
      break;
    case 3:
      _settingsScreen.drawOption(3, "Send MIDI Master Clock", _keyboard->getConfig()->configurationValues.sendMidiMasterClock, _keyboard->getConfig()->configurationValues.sendMidiMasterClockValueType, active);
      break;
    case 4:
      _settingsScreen.drawOption(4, "Send MIDI start/stop", _keyboard->getConfig()->configurationValues.sendMidiStartStop, _keyboard->getConfig()->configurationValues.sendMidiStartStopValueType, active);
      break;
  }
}

void Settings::_changeOptionValue(boolean increase) {
  switch (_activeOption) {
    case 1:            
      _changeMidiChannel(&_keyboard->getConfig()->configurationValues.midiChannelPlay, increase);
      _updateOption(_activeOption, true);
      break;
    case 2:
      _changeMidiChannel(&_keyboard->getConfig()->configurationValues.midiChannelPiano, increase);
      _updateOption(_activeOption, true);      
      break;
    case 3:
      _toggleTrueFalse(&_keyboard->getConfig()->configurationValues.sendMidiMasterClock);
      _updateOption(_activeOption, true);      
      break;
    case 4:
      _toggleTrueFalse(&_keyboard->getConfig()->configurationValues.sendMidiStartStop);
      _updateOption(_activeOption, true);      
      break;
  }
  _fsio->saveConfiguration(&_keyboard->getConfig()->configurationValues);
}

void Settings::_changeMidiChannel(int *value, boolean increase) {
  if (increase) {
    if (*value < 16) { 
      (*value)++; 
    } else {
      *value = 1; 
    }
  } else {
    if (*value > 1) { 
      (*value)--; 
    } else {
      *value = 16; 
    }
  }
}

void Settings::_toggleTrueFalse(bool *value) {
  *value = !(*value);
}