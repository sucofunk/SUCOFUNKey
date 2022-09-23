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
#include "Zoom.h"

Zoom::Zoom() { };

uint16_t Zoom::zoomIn(uint16_t cursorPosition) {
  if (_currentZoomLevel > ZOOM_IN_2) {
    setZoomLevel(_currentZoomLevel-1);
  }

  // return the changed cursort position. If we are switching between zoomlevels, the cursor needs always to stay on a position that can be shown on the grid
  int ncp = cursorPosition - (cursorPosition % getZoomlevelOffset());
  return ncp < 0 ? 0 : ncp;  
};

uint16_t Zoom::zoomOut(uint16_t cursorPosition) {
  if (_currentZoomLevel < ZOOM_OUT_2) {
    setZoomLevel(_currentZoomLevel+1);  
  }

  // return the changed cursort position. If we are switching between zoomlevels, the cursor needs always to stay on a position that can be shown on the grid
  int ncp = cursorPosition - (cursorPosition % getZoomlevelOffset());
  return ncp < 0 ? 0 : ncp;  
};

void Zoom::setZoomLevel(Zoomlevel level) {
  _currentZoomLevel = level;
  
  // redraw Grid
  Serial.print("ZoomLevel Change::");
  Serial.println(_currentZoomLevel);
};

Zoom::Zoomlevel Zoom::getZoomlevel() {
    return _currentZoomLevel;
};


// returns the amount of steps of the highest resolution/zoomlevel to take at the current zoomlevel, when  moving one position
byte Zoom::getZoomlevelOffset() {
  switch (_currentZoomLevel) {
    case ZOOM_IN_2:
      return 1;
      break;
    case ZOOM_IN_1:
      return 2;
      break;
    case ZOOM_NORMAL:
      return 4;
      break;
    case ZOOM_OUT_1:
      // ToDo: adjust when behavior is defined!
      return 8;
      break;
    case ZOOM_OUT_2:
      // ToDo: adjust when behavior is defined!
      return 32;
      break;
    default:
      break;
  }
}

float Zoom::getZoomlevelFactor() {
  switch (_currentZoomLevel) {
    case ZOOM_IN_2:
      return 4.0;
      break;
    case ZOOM_IN_1:
      return 2.0;
      break;
    case ZOOM_NORMAL:
      return 1.0;
      break;
    case ZOOM_OUT_1:
      // ToDo: adjust when behavior is defined!
      return 0.5;
      break;
    case ZOOM_OUT_2:
      // ToDo: adjust when behavior is defined!
      return 0.125;
      break;
    default:
      break;
  }
}