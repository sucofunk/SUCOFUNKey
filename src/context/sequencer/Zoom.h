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