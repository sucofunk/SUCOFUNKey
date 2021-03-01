#ifndef Synth_h
#define Synth_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"

class Synth {
    public:
        Synth(Sucofunkey *keyboard, Screen *screen);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _isActive = false;
        byte _activeBank = 2;
};

#endif
