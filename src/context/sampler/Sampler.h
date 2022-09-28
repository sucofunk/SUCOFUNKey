/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2022 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#ifndef Sampler_h
#define Sampler_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/SamplerScreen.h"
#include "../../gui/screens/components/Input.h"
#include "../../gui/screens/components/BlackKeyMenu.h"

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
            SAMPLE_SELECTED_EMPTY = 2,
            SAMPLE_EDIT_TRIM = 3,
            SAMPLE_WAIT_SAVE_SLOT = 4,
            SAMPLE_RENAME = 5,
            SAMPLER_WAIT_DELETE_CONFIRM = 6,
            SAMPLER_DELETE_CONFIRMED = 7,
            SAMPLER_LIBRARY_OPEN = 8
        };

        enum FaderState
        {
            IDLE = 0,
            TRIM_START = 1,
            TRIM_END = 2,
            VOLUME = 3
        };

        // menu functions
        void editActiveSample();
        void deleteActiveSample();
        void saveActiveSample();
        void saveActiveSampleAs();
        void cancel();

        void indicatePlayerPosition();

    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;
        boolean _isActive = false;
        byte _activeBank = 1;

        SamplerState currentState = SAMPLE_NOTHING;
        int _faderState = FaderState::IDLE;

        SamplerScreen _samplerScreen;
        BlackKeyMenu _blackKeyMenu;
        Input _input;

        byte _blinkActiveSample = false;
        byte _blinkActiveSampleStatus = false;
        byte _activeSampleLEDPin = 0;
        byte _activeSampleSlot = 0; // 1..24 corresponding to selected sample bank

        int _trimMarkerStartPosition = 0;
        int _trimMarkerEndPosition = 319;
        int _trimMarkerStartSampleCountOffset = 0;
        int _trimMarkerEndSampleCountOffset = 0;
        float _volumeScaleFactor = 1.0;

        byte _tempBank = 1;

        void _blinkSampleSlot(byte sampleId1, boolean on);
        void _play();

        int _timerCounter = 0;

        void _resetTrimMarkerOffsets(boolean start, boolean end);
};

#endif
