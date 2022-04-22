#ifndef Synth_h
#define Synth_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/components/BottomMenu.h"
#include <MIDI.h>

class Synth {
    public:
        Synth(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources);
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

        void _selectSample(byte bank1, byte sampleId1);
        void _playNextFreeWavetable(byte note, boolean play);        

        // queue for polyphonic events. each entry corresponds to a waveTableSynth1..6
        // 0 = not playing 1..24 -> corresponding note is playing 
        byte _polyKeyIDs[6] = {0, 0, 0, 0, 0, 0};
        
        int _val1 = 0;
        int _val2 = 0;        
        int _val3 = 0;
        int _val4 = 0;
        int _val5 = 0;
        int _val6 = 0;
        int _val7 = 0;
        int _val8 = 0;

        boolean _loop = false;
        byte _currentInstrumentId = 255;

};

#endif
