#ifndef Home_h
#define Home_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"

class Home {
    public:
        Home(Sucofunkey *keyboard, Screen *screen, char *activeSongPath);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        char *_activeSongPath;
        boolean _isActive = false;
        byte _activeBank = 1;
};

#endif
