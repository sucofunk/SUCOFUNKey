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

#ifndef ScreenStreaming_h
#define ScreenStreaming_h

#include <Arduino.h>
#include "../../hardware/Configuration.h"

#ifdef ENABLE_SCREEN_STREAMING

// Command opcodes
#define CMD_FILL_RECT   0x01
#define CMD_LINE        0x02
#define CMD_PIXEL       0x03
#define CMD_HLINE       0x04
#define CMD_VLINE       0x05
#define CMD_CIRCLE      0x06
#define CMD_TRIANGLE    0x07
#define CMD_TEXT        0x08
#define CMD_BITMAP      0x09
#define CMD_CLEAR       0xFF
#define CMD_SYNC        0xFE  // Sync marker: FE 00 00 FE 00

// Sync every N commands to allow recovery from byte drops
#define SYNC_INTERVAL   15  // More frequent sync for faster recovery

// Font IDs (matching Screen.cpp setTextSize)
#define FONT_5PT        0  // BaiJamjuree_Medium5pt7b
#define FONT_6PT        1  // BaiJamjuree_Medium6pt7b  
#define FONT_8PT        2  // BaiJamjuree_Regular8pt7b
#define FONT_8PT_MONO   3  // BaiJamjureeRegularMonoDigits8pt7b
#define FONT_OXYGEN     4  // OxygenMono_Regular8pt7b

// Buffer size - must be large enough for full screen updates (300+ commands)
// Each line command is 11 bytes, so 300 lines = 3.3KB minimum
#define STREAM_BUFFER_SIZE 32768

class ScreenStreaming {
    public:
        ScreenStreaming();
        
        // Queue drawing commands
        void logFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void logLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
        void logPixel(int16_t x, int16_t y, uint16_t color);
        void logHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        void logVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void logCircle(int16_t x, int16_t y, int16_t r, bool fill, uint16_t color);
        void logTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool fill, uint16_t color);
        void logText(int16_t x, int16_t y, uint16_t color, uint8_t fontId, const char* text);
        void logBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, const uint8_t* data);
        void logClear(uint16_t color);
        void sendSync();  // Send sync marker
        
        // Set current font (called by Screen::setTextSize)
        void setCurrentFont(uint8_t fontId);
        
        // Check for streaming request and send pending data
        void checkSerialCommand();
        
        // Enable/disable streaming
        void setEnabled(bool enabled);
        bool isEnabled();
        
    private:
        // Circular buffer (actual storage is in DMAMEM, see .cpp)
        uint8_t* _buffer;
        volatile uint16_t _head;
        volatile uint16_t _tail;
        
        uint8_t _currentFont;
        bool _enabled;
        bool _streaming;
        uint16_t _cmdCount;  // Command counter for periodic sync
        
        // Buffer operations
        bool writeToBuffer(const uint8_t* data, uint16_t len);
        void doSendSync();  // Internal sync send (no counter increment)
        uint16_t availableSpace();
        void streamPending();
        
        // Helper to write 16-bit values little-endian
        void write16(uint8_t* buf, uint16_t val);
};

#endif // ENABLE_SCREEN_STREAMING

#endif // ScreenStreaming_h
