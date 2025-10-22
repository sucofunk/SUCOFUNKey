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
   
#ifndef Selection_h
#define Selection_h

#include <Arduino.h>

class Selection {
    public:
        Selection();

        typedef struct {
            int startX = -1;
            int startY = -1;
            int endX = -1;
            int endY = -1;
        } SelectionStruct;

        void setSelectionActive(boolean active);
        SelectionStruct getSelection();
        SelectionStruct getNormalizedSelection();
        void setStart(int startX, int startY);
        void setEnd(int endX, int endY);
        boolean isActive();
        boolean isCopy();
        void setSelectionAsCopy(boolean copy);
        void setWarning(boolean warning);
        boolean isWarning();
        void printSelection();
        void shiftUP(int cellCount);
        void shiftDOWN(int cellCount);
        void shiftLEFT(int cellCount);
        void shiftRIGHT(int cellCount);

    private:
        boolean _isActive = false;
        boolean _isCopy = false; // will be set, when moving a copy of the active selection around -> different color!
        boolean _warning = false;
        SelectionStruct _selection;
        SelectionStruct _normalizedSelection;
        void _normalize();
};

#endif