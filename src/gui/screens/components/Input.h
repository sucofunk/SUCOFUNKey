#ifndef Input_h
#define Input_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"
#include "../../../helper/FSIO.h"

class Input {
    public:
        Input();        
        Input(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, Screen::Area inputArea);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void activateInput();
        char * getInputValue();
        void showErrorMessage(char const * errorMessage, int showForMs);

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        Screen::Area _inputArea;
        Screen::Area _cursorArea;
        char _inputValue[9] = "        ";
        byte _inputLength = 0;
        char _cursorString[9] = "        ";
        boolean _cursorON = false;
        byte _cursorPosition = 0;
        byte _lastCursorPosition = 0;

        byte _currentCharIndex = 0;
        void _redrawInput();
        void _redrawCursor();
};

#endif