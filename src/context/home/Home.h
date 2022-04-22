#ifndef Home_h
#define Home_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../gui/screens/HomeScreen.h"
#include "../../gui/screens/components/BottomMenu.h"

class Home {
    public:
        Home(Sucofunkey *keyboard, Screen *screen, char *activeSongPath, char *activeSongName);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        char *_activeSongPath;
        char *_activeSongName;
        boolean _isActive = false;

        BottomMenu _bottomMenu;
        HomeScreen _homeScreen;        
};

#endif
