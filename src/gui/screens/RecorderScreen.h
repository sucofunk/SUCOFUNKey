#ifndef RecorderScreen_h
#define RecorderScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"
#include "../Screen.h"

class RecorderScreen {
    public:
        RecorderScreen();
        RecorderScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources);

        void showRecorderScreen();
        void showRecorderScreenRecording();
        void drawInputPeakMeter(float peak);
        
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;
        Screen::Area _peakArea;
        Screen::Area _textArea;
};

#endif
