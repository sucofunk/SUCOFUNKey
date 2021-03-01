#ifndef BottomMenu_h
#define BottomMenu_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"

class BottomMenu {
    public:
        BottomMenu();
        BottomMenu(Sucofunkey *keyboard, Screen *screen);
        BottomMenu(Sucofunkey *keyboard, Screen *screen, char *label1, int retVal1, char *label3, int retVal3);
        BottomMenu(Sucofunkey *keyboard, Screen *screen, char *label1, int retVal1, char *label2, int retVal2, char *label3, int retVal3);
        
        void handleEvent(Sucofunkey::keyQueueStruct event);
        
        void setupMenu3(char *label1, int retVal1, char *label2, int retVal2, char *label3, int retVal3);
        void setupMenu2(char *label1, int retVal1, char *label3, int retVal3);
        
        void setMenuItems(byte itemCount);

        void setItem1Label(char *label);
        void setItem2Label(char *label);
        void setItem3Label(char *label);
        void setItem1CallbackValue(int retVal);
        void setItem2CallbackValue(int retVal);
        void setItem3CallbackValue(int retVal);

        void showMenu(boolean visible);
        boolean isVisible();
        void activateMenu(boolean active);
        void selectItem(byte item);

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        byte _itemCount = 0;
        byte _selectedItem = 1;
        boolean _visible = false;
        char *_label1;
        char *_label2;
        char *_label3;
        int _retVal1;
        int _retVal2;
        int _retVal3;
        void _redrawMenu();
};

#endif