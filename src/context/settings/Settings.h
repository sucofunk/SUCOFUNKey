#ifndef Settings_h
#define Settings_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"

class Settings {
    public:
        Settings(Sucofunkey *keyboard, Screen *screen);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _isActive = false;
        byte _activeBank = 1;
};

#endif
