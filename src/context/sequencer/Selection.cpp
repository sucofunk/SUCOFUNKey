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
#include "Selection.h"

Selection::Selection() { 
    _selection = SelectionStruct();
    _normalizedSelection = SelectionStruct();
};

void Selection::setSelectionActive(boolean active) {
    _isActive = active;

    // reset selection, if selection ended
    if (!active) {
        _selection = SelectionStruct();
        _normalizedSelection = SelectionStruct();        
    }
};

Selection::SelectionStruct Selection::getSelection() {
    return _selection;
};

Selection::SelectionStruct Selection::getNormalizedSelection() {
    _normalize();
    return _normalizedSelection;
};

void Selection::setStart(int startX, int startY) {
    _selection.startX = startX;
    _selection.startY = startY;

    if (_selection.endX == -1 && _selection.endY == -1) {
        _selection.endX = startX;
        _selection.endY = startY;
    }
};

void Selection::setEnd(int endX, int endY) {
    _selection.endX = endX;
    _selection.endY = endY;
};

boolean Selection::isActive() {
    return _isActive;
};

void Selection::_normalize() {
    _normalizedSelection.startX = min(_selection.startX, _selection.endX);
    _normalizedSelection.endX = max(_selection.startX, _selection.endX);
    _normalizedSelection.startY = min(_selection.startY, _selection.endY);
    _normalizedSelection.endY = max(_selection.startY, _selection.endY);    
}