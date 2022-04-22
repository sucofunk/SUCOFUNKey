#ifndef BlackKeyMenu_h
#define BlackKeyMenu_h

#include <Arduino.h>
#include "../../../hardware/Sucofunkey.h"
#include "../../Screen.h"

class BlackKeyMenu {
    public:
        BlackKeyMenu();
        BlackKeyMenu(Sucofunkey *keyboard, Screen *screen);
        
        void setOptions(char const *o1, char const *o2, char const *o3, char const *o4, char const *o5, char const *o6, char const *o7, char const *o8, char const *o9, char const *o10);
        void setOption(uint8_t option, char const *label);

        void setExclusiveAction(byte position, boolean activated); // disables all other options and highlights the one at position

        void handleEvent(Sucofunkey::keyQueueStruct event);
        void showMenu();
        void hideMenu();
        boolean isVisible();
                
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _visible = false;

        char const *_label1 = "   ";
        char const *_label2 = "   ";
        char const *_label3 = "   ";
        char const *_label4 = "   ";
        char const *_label5 = "   ";
        char const *_label6 = "   ";
        char const *_label7 = "   ";
        char const *_label8 = "   ";
        char const *_label9 = "   ";
        char const *_label10 = "   ";

        int _ledPINs[10] = { _keyboard->LED_FS_1, _keyboard->LED_GS_1, _keyboard->LED_AS_1, _keyboard->LED_CS_1, _keyboard->LED_DS_1, _keyboard->LED_FS_2, _keyboard->LED_GS_2, _keyboard->LED_AS_2, _keyboard->LED_CS_2, _keyboard->LED_DS_2};
    
        byte _exclusivePosition = 0;

        void _redrawMenu();
        void _redrawFrame();
        void _redrawOption(uint8_t option);
};

#endif