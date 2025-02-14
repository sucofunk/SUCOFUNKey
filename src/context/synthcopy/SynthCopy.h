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
   
#ifndef SynthCopy_h
#define SynthCopy_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "../../helper/AudioResources.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../gui/screens/SynthCopyScreen.h"
#include "../sequencer/Play.h"

#include <MIDI.h>

class SynthCopy {
    public:
        
        SynthCopy(Sucofunkey* keyboard, Screen* screen, FSIO* fsio, SampleFSIO* sfsio, Play* play);

        void setActive(boolean active);
        long receiveTimerTick();
        void handleEvent(Sucofunkey::keyQueueStruct event);

    private:
        Sucofunkey* _keyboard;
        Screen* _screen;
        FSIO* _fsio;
        SampleFSIO* _sfsio;
        Play* _play;
        boolean _isActive = false;

        SynthCopyScreen _synthCopyScreen;

        byte _channel = 10;
};

#endif
