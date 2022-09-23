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
   
#ifndef Swing_h
#define Swing_h

#include <Arduino.h>

class Swing {
    public:
        Swing();

        // set the amount of microseconds the sequencer plays a 64th note
        void setTickMicroseconds(long tickMicroseconds);
        
        // returns the amount of samples to wait to reach the time-shift for a swing level
        int getShiftSamplesForSwingLevel(byte level);

        // swing expression in the SongStructure is a one byte value and combines swing level and swing group
        // why? because our memory is very limited and this will save a few kb or ram
        //
        // -> 4 Bit for Level, 4 Bit for Group -> LEVEL 2, GROUP 8 => 0010|1000
        // levels for groups are stored in the meta information
        // group 0 == no group -> swing level will be used
        byte getBinarySwingExpression(byte level, byte group);
        byte getLevelFromBinarySwingExpression(byte expression);
        byte getGroupFromBinarySwingExpression(byte expression);
        byte levelUp(byte level);
        byte levelDown(byte level);

        byte levelUpInExpression(byte expression); // returns updated expression
        byte levelDownInExpression(byte expression); // returns updated expression
        byte groupUpInExpression(byte expression); // returns updated expression
        byte groupDownInExpression(byte expression); // returns updated expression

    private:
        long _microSecondsPerTick = 0;

        // levels are 0 .. 11 
        // 0 = no delay
        // 12 = 11/12 of one tick -> brings up a resolution of 16*12 = 192 per beat on a 4/4 at the normal zoom level
        int _samplesByLevel[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        void _recalculate();
};

#endif