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

#include <Arduino.h>
#include "Swing.h"

void Swing::setTickMicroseconds(int tickMicroseconds) {
    _microSecondsPerTick = tickMicroseconds;
    _recalculate();
};

int Swing::getShiftSamplesForSwingLevel(byte level) {
    return _samplesByLevel[level];
};


// -> 4 Bit for Level, 4 Bit for Group -> LEVEL 2, GROUP 8 => 0010|1000
byte Swing::getBinarySwingExpression(byte level, byte group) {
    return ((level << 4) + group);
};

byte Swing::getLevelFromBinarySwingExpression(byte expression) {
    return ((expression & 0xF0) >> 4);
};

byte Swing::getGroupFromBinarySwingExpression(byte expression) {
    return (expression & 0xF);
};


byte Swing::levelUpInExpression(byte expression) {
    byte level = getLevelFromBinarySwingExpression(expression);
    byte group = getGroupFromBinarySwingExpression(expression);
    if (level < 11) {
        level++;
    }
    return getBinarySwingExpression(level, group);
};

byte Swing::levelDownInExpression(byte expression) {
    byte level = getLevelFromBinarySwingExpression(expression);
    byte group = getGroupFromBinarySwingExpression(expression);
    if (level > 0) {
        level--;
    }
    return getBinarySwingExpression(level, group);
};


byte Swing::levelUp(byte level) {
    if (level < 11) {
        level++;
    }
    return level;
};

byte Swing::levelDown(byte level) {
    if (level > 0) {
        level--;
    }
    return level;    
};


byte Swing::groupUpInExpression(byte expression) {
    byte level = getLevelFromBinarySwingExpression(expression);
    byte group = getGroupFromBinarySwingExpression(expression);
    if (group < 8) {
        group++;
    }
    return getBinarySwingExpression(level, group);
};

byte Swing::groupDownInExpression(byte expression) {
    byte level = getLevelFromBinarySwingExpression(expression);
    byte group = getGroupFromBinarySwingExpression(expression);
    if (group > 0) {
        group--;
    }
    return getBinarySwingExpression(level, group);
}; 


void Swing::_recalculate() {
    // 1/1000000 = 1 microsecond
    double microsecondsPerSample = 1000000 / 44100.0;
    double subDivisionSamples = _microSecondsPerTick / microsecondsPerSample / 12;

    for (int i=0; i<12; i++) {
        _samplesByLevel[i] = (int)(i * subDivisionSamples);
    }
};
