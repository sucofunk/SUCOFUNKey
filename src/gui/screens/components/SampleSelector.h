#ifndef SampleSelector_h
#define SampleSelector_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"
#include "../../../helper/FSIO.h"

class SampleSelector {
    public:
        SampleSelector();
        SampleSelector(Sucofunkey *keyboard, Screen *screen, FSIO *fsio);
        long receiveTimerTick();
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void drawSampleSelector();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        char *_activeDirectory;
        byte _activeComponent = 0;
        Screen::Area _AREA_SONGSELECTOR;
        Screen::Area _AREA_SONGSELECTOR_LINE_1;
        Screen::Area _AREA_SONGSELECTOR_LINE_2;
        Screen::Area _AREA_SONGSELECTOR_LINE_3;
        Screen::Area _AREA_SONGSELECTOR_LINE_4;
        Screen::Area _AREA_SONGSELECTOR_LINE_5;
        int _sampleCount = 0;
        int _activeItem = 0;
        int _offset = 0;
        boolean _counted = false;
        char _line1[40];
        char _line2[40];
        char _line3[40];
        char _line4[40];
        char _line5[40];
        boolean getLineDescription(int fileIndex, char *lineBuffer);
};

#endif