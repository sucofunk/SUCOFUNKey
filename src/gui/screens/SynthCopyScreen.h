/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2026 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#ifndef SynthCopyScreen_h
#define SynthCopyScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../Screen.h"

class SynthCopyScreen {
    public:
        SynthCopyScreen();
        SynthCopyScreen(Sucofunkey *keyboard, Screen *screen);
        void handleEvent(Sucofunkey::keyQueueStruct event);
        long receiveTimerTick();
        void show(byte channel, byte startNote, byte endNote, long releaseMS);
        void updateChannel(byte channel);
        void updateRange(byte startNote, byte endNote);
        void updateTriggerTime(long releaseMS);
        void updateRecordingState(boolean isRecording, byte current = 0, byte total = 0);
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        boolean _isRunning = false;
        
        Screen::Area _headlineArea;
        Screen::Area _lineChannel;
        Screen::Area _lineRange;
        Screen::Area _lineTrigger;
        Screen::Area _lineRecording;
};

#endif
