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
   
#ifndef Zoom_h
#define Zoom_h

#include <Arduino.h>

class Zoom {
    public:
        Zoom();

        enum Zoomlevel {
            ZOOM_IN_2 = 1,   // 16 per quarter note (64th)
            ZOOM_IN_1 = 2,   // 8 per quarter note  (32th)
            ZOOM_NORMAL = 3, // 4 per quarter note  (16th)
            ZOOM_OUT_1 = 4,  // tbd
            ZOOM_OUT_2 = 5   // tbd
        };

        uint16_t zoomIn(uint16_t cursorPosition);
        uint16_t zoomOut(uint16_t cursorPosition);
        void setZoomLevel(Zoomlevel level);
        Zoomlevel getZoomlevel();
        byte getZoomlevelOffset();
        float getZoomlevelFactor();


        Zoomlevel _currentZoomLevel = ZOOM_NORMAL;

    private:
        
};

#endif