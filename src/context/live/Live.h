#ifndef Live_h
#define Live_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/components/BottomMenu.h"
#include <MIDI.h>

class Live {
    public:
        Live(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        void receiveMidiData(midi::MidiType type, int d1, int d2);

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

        float _tempVelocity = 0.0;

        void _playNextFreeMemory(byte sample1, byte velocity, boolean play);
        void _handlePolyphonicAftertouch(byte sample1, byte value);
        void _polyChangeVelocity(byte polymem, byte velocity);

        // queue for polyphonic events. each entry corresponds to a playMemLive1..6
        // 0 = not playing 1..72 -> corresponding sample is playing 
        byte _polyMemIDs[8] = {0, 0, 0, 0, 0, 0, 0, 0};

};

#endif
