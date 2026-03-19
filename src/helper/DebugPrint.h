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

#ifndef DebugPrint_h
#define DebugPrint_h

#include <Arduino.h>
#include "../hardware/Configuration.h"

class DebugPrint {
    public:
        static void init(Configuration* config);
        
        // Print functions - only print when screen streaming is disabled
        static void print(const char* msg);
        static void print(const String& msg);
        static void print(int value);
        static void print(unsigned int value);
        static void print(long value);
        static void print(unsigned long value);
        static void print(char c);
        static void print(float value);
        static void print(double value);
        
        static void println(const char* msg);
        static void println(const String& msg);
        static void println(int value);
        static void println(unsigned int value);
        static void println(long value);
        static void println(unsigned long value);
        static void println(float value);
        static void println(double value);
        static void println();
        
    private:
        static Configuration* _config;
        static bool canPrint();
};

#endif
