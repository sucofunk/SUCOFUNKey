#ifndef SamplerScreen_h
#define SamplerScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"
#include "../Screen.h"
#include "components/BottomMenu.h"

class SamplerScreen {
    public:
        SamplerScreen();
        SamplerScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources);
        void handleEvent(Sucofunkey::keyQueueStruct event);

        void showEmptyScreen();        
        void showSampleInfo(byte sampleId72, float volumeScaleFactor);        
        void showSampleInfo(byte bank0, byte sampleId0, float volumeScaleFactor);        
        void setBottomMenu(BottomMenu bottomMenu);
        void drawTrimMarker(int trimMarkerStartPosition, int trimMarkerEndPosition, byte sampleId72, float volumeScaleFactor);
        void drawTrimMarker(int trimMarkerStartPosition, int trimMarkerEndPosition, byte bank0, byte sampleId0, float volumeScaleFactor);
        void removeTrimMarker(int position, byte sampleId72, float volumeScaleFactor);

        void showSlotSelectionHint();
        void showSavingMessage();

        void drawPlayerPosition(int x, int start, int end);
        void resetPlayerPosition();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;
        BottomMenu _bottomMenu;

        int zeroAxisY;
        boolean isPlayerPositionVisible = false;        
};

#endif
