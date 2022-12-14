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
   
#ifndef Arrange_h
#define Arrange_h

#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/components/BottomMenu.h"
#include "../sequencer/Play.h"
#include <MIDI.h>

class Arrange {
    public:
        Arrange(Sucofunkey* keyboard, Screen* screen, FSIO* fsio, SampleFSIO* sfsio, unsigned int* extmemArray, AudioResources* audioResources, Play* play);
        long receiveTimerTick();
        void setActive(boolean active);
        void handleEvent(Sucofunkey::keyQueueStruct event);

    private:
        Sucofunkey* _keyboard;
        Screen* _screen;
        FSIO* _fsio;
        SampleFSIO* _sfsio;
        unsigned int* _extmemArray;
        AudioResources* _audioResources;
        Play* _play;
        boolean _isActive = false;
        byte _activeBank = 1;
        int _playbackTickSpeed = 10000;
        boolean _playLEDon = false;
        boolean _isInitialized = false;
        boolean _LEDHighlightSlots[14] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false};        
};

#endif
