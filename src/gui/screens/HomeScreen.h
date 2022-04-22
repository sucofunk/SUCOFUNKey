#ifndef HomeScreen_h
#define HomeScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../Screen.h"

class HomeScreen {
    public:
        HomeScreen();
        HomeScreen(Sucofunkey *keyboard, Screen *screen, char *activeSongName);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void showSupporterScreen();
        void showGeneralInformation();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        char *_activeSongName;
        char *_songsBasePath;
        byte _activeComponent = 0;
};

#endif
