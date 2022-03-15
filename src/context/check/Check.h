#ifndef Check_h
#define Check_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"

class Check {
    public:
        Check(Sucofunkey *keyboard, Screen *screen);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void drawInfotext(char *msg);
        void encoderAction(byte encoder, byte action);       
        void turnAllLEDsOn();
        void showFaderPosition();
        void showVolume();
        void playSound();
        void stopSound();
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _isActive = false;
        byte _activeBank = 1;
        int _lastFaderPosition = 0;
        int _lastVolume = 0;
};

#endif
