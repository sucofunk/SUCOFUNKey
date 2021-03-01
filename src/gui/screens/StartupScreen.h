#ifndef StartupScreen_h
#define StartupScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "components/SongSelector.h"

class StartupScreen {
    public:
        StartupScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, char *activeSongName);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void showLogo();
        void showMessage(const char* message, boolean warning);
        void transitionToSelection();
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        char *_activeSongName;
        char *_songsBasePath;
        Screen::Area _logoArea;
        Screen::Area _messageArea;
        SongSelector _songSelector;
        void _drawSongSelector();
        byte _activeComponent = 0; // 1 = SongSelector
};

#endif
