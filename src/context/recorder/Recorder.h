/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2025 by Marc Berendes (marc @ sucofunk.com)
    
   ----------------------------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.    

   ---------------------------------------------------------------------------------------------- */
   
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
            RECORDER_WAIT_PEAK = 2,
            RECORDER_MULTISAMPLE_RECORDING = 3,
            RECORDER_MULTISAMPLE_WAIT_PEAK = 4,
            RECORDER_MULTISAMPLE_WAIT_END = 5,
            ADJUSTING_SILENCE_TRESHOLD = 6
        };        

        RecorderState currentState = RECORDER_NOTHING;

        void cancelRecording();
        void startRecording();
        void continueRecording();
        void stopRecording();
        boolean isRecording();

        //void pauseRecording();
        //boolean isPaused();

        void activateInput();

        void setMultiSampleTresholdMode(boolean multisample);
        void setMultiSampleTreshold(int lengthBelowTreshold, float treshold);
        boolean isMultiSampleTresholdMode();
        
        void adjustSilenceTreshold();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;
        boolean _isActive = false;
        boolean _isRecording = false;
        boolean _isPaused = false;
        File _frec;
        unsigned long recByteSaved = 0L;
        RecorderScreen _recorderScreen;

        float _peak = 0.0;

        byte _lastInput = Sucofunkey::INPUT_NONE;
        boolean _onScreenPeak = true;

        boolean _isMultiSampleTresholdRecording = false;
        float _multiSampleTreshold = 0.00025;
        float _multisampleSilenceTresholdFactor = 1.7;
        int _multiSampleTresholdDuration = 5;
        int _durationBelowTreshold = 0;

        long _bytesRecorded = 0;
        void _sendMarker();

};

#endif
