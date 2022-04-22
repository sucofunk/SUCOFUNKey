#ifndef Live_h
#define Live_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/components/BottomMenu.h"

class Live {
    public:
        Live(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        unsigned int *_extmemArray;
        AudioResources *_audioResources;
        boolean _isActive = false;
        byte _activeBank = 1;

        BottomMenu _bottomMenu;

};

#endif
