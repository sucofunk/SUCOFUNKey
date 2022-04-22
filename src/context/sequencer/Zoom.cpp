#include <Arduino.h>
#include "Zoom.h"

Zoom::Zoom() { };

uint16_t Zoom::zoomIn(uint16_t cursorPosition) {
  if (_currentZoomLevel > ZOOM_IN_2) {
    setZoomLevel(_currentZoomLevel-1);
  }

  // return the changed cursort position. If we are switching between zoomlebels, the cursort needs always to stay on a position that can be shown on the grid
  int ncp = cursorPosition - (cursorPosition % getZoomlevelOffset());
  return ncp < 0 ? 0 : ncp;  
};

uint16_t Zoom::zoomOut(uint16_t cursorPosition) {
  if (_currentZoomLevel < ZOOM_OUT_2) {
    setZoomLevel(_currentZoomLevel+1);  
  }

  // return the changed cursort position. If we are switching between zoomlebels, the cursort needs always to stay on a position that can be shown on the grid
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
      return 4;
      break;
    case ZOOM_OUT_2:
      // ToDo: adjust when behavior is defined!
      return 4;
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
      return 1.0;
      break;
    case ZOOM_OUT_2:
      // ToDo: adjust when behavior is defined!
      return 1.0;
      break;
    default:
      break;
  }
}