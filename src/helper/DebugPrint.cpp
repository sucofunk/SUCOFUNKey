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

#include "DebugPrint.h"

Configuration* DebugPrint::_config = nullptr;

void DebugPrint::init(Configuration* config) {
    _config = config;
}

bool DebugPrint::canPrint() {
    // Print if config not set yet, or if screen streaming is disabled
    if (_config == nullptr) return true;

    #ifdef ENABLE_SCREEN_STREAMING
     return !_config->configurationValues.enableScreenStreaming;
    #endif

    return true;    
}

void DebugPrint::print(const char* msg) {
    if (canPrint()) Serial.print(msg);
}

void DebugPrint::print(const String& msg) {
    if (canPrint()) Serial.print(msg);
}

void DebugPrint::print(int value) {
    if (canPrint()) Serial.print(value);
}

void DebugPrint::print(unsigned int value) {
    if (canPrint()) Serial.print(value);
}

void DebugPrint::print(long value) {
    if (canPrint()) Serial.print(value);
}

void DebugPrint::print(unsigned long value) {
    if (canPrint()) Serial.print(value);
}

void DebugPrint::print(char c) {
    if (canPrint()) Serial.print(c);
}

void DebugPrint::print(float value) {
    if (canPrint()) Serial.print(value);
}

void DebugPrint::print(double value) {
    if (canPrint()) Serial.print(value);
}

void DebugPrint::println(const char* msg) {
    if (canPrint()) Serial.println(msg);
}

void DebugPrint::println(const String& msg) {
    if (canPrint()) Serial.println(msg);
}

void DebugPrint::println(int value) {
    if (canPrint()) Serial.println(value);
}

void DebugPrint::println(unsigned int value) {
    if (canPrint()) Serial.println(value);
}

void DebugPrint::println(long value) {
    if (canPrint()) Serial.println(value);
}

void DebugPrint::println(unsigned long value) {
    if (canPrint()) Serial.println(value);
}

void DebugPrint::println(float value) {
    if (canPrint()) Serial.println(value);
}

void DebugPrint::println(double value) {
    if (canPrint()) Serial.println(value);
}

void DebugPrint::println() {
    if (canPrint()) Serial.println();
}
