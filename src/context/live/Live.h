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
