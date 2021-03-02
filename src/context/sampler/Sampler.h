#ifndef Sampler_h
#define Sampler_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/SamplerScreen.h"
#include "../../gui/screens/components/BottomMenu.h"

class Sampler {
    public:
        Sampler(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void indicateFreeSamples(boolean readFromSD, long ms);

        enum SamplerState
        {
            SAMPLE_NOTHING = 0,
            SAMPLE_SELECTED = 1,
            SAMPLE_EDIT_TRIM = 2,
            SAMPLE_WAIT_SAVE_SLOT = 3
        };        

        // menu functions
        void editActiveSample();
        void deleteActiveSample();
        void saveActiveSample();
        void saveActiveSampleAs();
        void cancel();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;
        boolean _isActive = false;
        byte _activeBank = 1;

        int currentState = SAMPLE_NOTHING;

        SamplerScreen _samplerScreen;

        byte _blinkActiveSample = false;
        byte _blinkActiveSampleStatus = false;
        byte _activeSampleLEDPin = 0;
        byte _activeSampleSlot = 0; // 1..24 corresponding to selected sample bank

        BottomMenu _bottomMenu;

        int _trimMarkerStartPosition = 0;
        int _trimMarkerEndPosition = 319;
        float _volumeScaleFactor = 1.0;

        byte _tempBank;

        void _blinkSampleSlot(byte sampleId1, boolean on);
        void _play();
};

#endif
