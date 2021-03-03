#ifndef Recorder_h
#define Recorder_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/RecorderScreen.h"
#include "../../gui/screens/components/BottomMenu.h"

class Recorder {
    public:
        Recorder(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources);
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();        

        enum RecorderState
        {
            RECORDER_NOTHING = 0,
            RECORDER_RECORDING = 1,
            RECORDER_WAITPEAK = 2
        };        

        RecorderState currentState = RECORDER_NOTHING;

        void startRecording();
        void continueRecording();
        void stopRecording();
        boolean isRecording();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;
        boolean _isActive = false;
        boolean _isRecording = false;
        File _frec;
        unsigned long recByteSaved = 0L;
        RecorderScreen _recorderScreen;

        byte _lastInput = Sucofunkey::INPUT_NONE;

        void activateInput();
};

#endif
