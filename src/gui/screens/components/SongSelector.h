#ifndef SongSelector_h
#define SongSelector_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"
#include "../../../helper/FSIO.h"
#include "Input.h"

class SongSelector {
    public:
        SongSelector();        
        SongSelector(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, char *activeSongName);
        long receiveTimerTick();
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void drawSongSelector();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        char *_activeSongName;
        Input _input;
        byte _activeComponent = 0;
        Screen::Area _AREA_SONGSELECTOR;
        Screen::Area _AREA_SONGSELECTOR_LINE_1;
        Screen::Area _AREA_SONGSELECTOR_LINE_2;
        Screen::Area _AREA_SONGSELECTOR_LINE_3;
        Screen::Area _AREA_SONGSELECTOR_LINE_4;
        Screen::Area _AREA_SONGSELECTOR_LINE_5;
        int _songCount = 0;
        int _activeItem = 0;
        int _offset = -1;
        boolean _counted = false;
        char _line1[40];
        char _line2[40];
        char _line3[40];
        char _line4[40];
        char _line5[40];
        boolean getLineDescription(int fileIndex, char *lineBuffer);
};

#endif