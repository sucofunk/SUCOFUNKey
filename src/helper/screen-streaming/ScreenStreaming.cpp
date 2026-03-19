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

#include "ScreenStreaming.h"

#ifdef ENABLE_SCREEN_STREAMING

// Buffer in DMAMEM to save regular RAM
DMAMEM static uint8_t _streamBuffer[STREAM_BUFFER_SIZE];

ScreenStreaming::ScreenStreaming() {
    _buffer = _streamBuffer;
    _head = 0;
    _tail = 0;
    _currentFont = FONT_5PT;
    _enabled = false;
    _streaming = false;
    _cmdCount = 0;
}

void ScreenStreaming::write16(uint8_t* buf, uint16_t val) {
    buf[0] = val & 0xFF;
    buf[1] = (val >> 8) & 0xFF;
}

uint16_t ScreenStreaming::availableSpace() {
    if (_head >= _tail) {
        return STREAM_BUFFER_SIZE - (_head - _tail) - 1;
    } else {
        return _tail - _head - 1;
    }
}

bool ScreenStreaming::writeToBuffer(const uint8_t* data, uint16_t len) {
    if (!_enabled || !_streaming) return false;
    
    // Send sync marker periodically
    _cmdCount++;
    if (_cmdCount >= SYNC_INTERVAL) {
        doSendSync();
        _cmdCount = 0;
    }
    
    // Wait for enough buffer space before writing (prevents partial writes)
    while (Serial.availableForWrite() < len + 10) {
        delayMicroseconds(50);
    }
    
    // Write entire command and verify all bytes written
    size_t written = Serial.write(data, len);
    
    // If not all bytes written, keep trying
    while (written < len) {
        Serial.flush();  // Wait for buffer to drain
        size_t more = Serial.write(&data[written], len - written);
        if (more > 0) written += more;
    }
    
    // Flush after every command to ensure byte order integrity
    Serial.flush();
    
    // Throttle command rate - prevents USB buffer overflow
    delayMicroseconds(200);
    
    return true;
}

// Send sync marker: FE 00 00 FE 00 (can't appear in text or valid coordinates)
void ScreenStreaming::doSendSync() {
    if (!_enabled || !_streaming) return;
    
    // Wait for buffer to drain before sending sync
    Serial.flush();
    
    static const uint8_t syncMarker[5] = {0xFE, 0x00, 0x00, 0xFE, 0x00};
    Serial.write(syncMarker, 5);
    Serial.flush();  // Ensure sync is sent before continuing
}

void ScreenStreaming::sendSync() {
    doSendSync();
    _cmdCount = 0;  // Reset counter
}

void ScreenStreaming::setEnabled(bool enabled) {
    _enabled = enabled;
    if (!enabled) {
        _streaming = false;
        _head = 0;
        _tail = 0;
    }
}

bool ScreenStreaming::isEnabled() {
    return _enabled;
}

void ScreenStreaming::setCurrentFont(uint8_t fontId) {
    _currentFont = fontId;
}

// CMD_FILL_RECT: 01 x:2 y:2 w:2 h:2 color:2 = 11 bytes
void ScreenStreaming::logFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    uint8_t cmd[11];
    cmd[0] = CMD_FILL_RECT;
    write16(&cmd[1], x);
    write16(&cmd[3], y);
    write16(&cmd[5], w);
    write16(&cmd[7], h);
    write16(&cmd[9], color);
    writeToBuffer(cmd, 11);
}

// CMD_LINE: 02 x0:2 y0:2 x1:2 y1:2 color:2 = 11 bytes
void ScreenStreaming::logLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    uint8_t cmd[11];
    cmd[0] = CMD_LINE;
    write16(&cmd[1], x0);
    write16(&cmd[3], y0);
    write16(&cmd[5], x1);
    write16(&cmd[7], y1);
    write16(&cmd[9], color);
    writeToBuffer(cmd, 11);
}

// CMD_PIXEL: 03 x:2 y:2 color:2 = 7 bytes
void ScreenStreaming::logPixel(int16_t x, int16_t y, uint16_t color) {
    uint8_t cmd[7];
    cmd[0] = CMD_PIXEL;
    write16(&cmd[1], x);
    write16(&cmd[3], y);
    write16(&cmd[5], color);
    writeToBuffer(cmd, 7);
}

// CMD_HLINE: 04 x:2 y:2 w:2 color:2 = 9 bytes
void ScreenStreaming::logHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    uint8_t cmd[9];
    cmd[0] = CMD_HLINE;
    write16(&cmd[1], x);
    write16(&cmd[3], y);
    write16(&cmd[5], w);
    write16(&cmd[7], color);
    writeToBuffer(cmd, 9);
}

// CMD_VLINE: 05 x:2 y:2 h:2 color:2 = 9 bytes
void ScreenStreaming::logVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    uint8_t cmd[9];
    cmd[0] = CMD_VLINE;
    write16(&cmd[1], x);
    write16(&cmd[3], y);
    write16(&cmd[5], h);
    write16(&cmd[7], color);
    writeToBuffer(cmd, 9);
}

// CMD_CIRCLE: 06 x:2 y:2 r:2 fill:1 color:2 = 10 bytes
void ScreenStreaming::logCircle(int16_t x, int16_t y, int16_t r, bool fill, uint16_t color) {
    uint8_t cmd[10];
    cmd[0] = CMD_CIRCLE;
    write16(&cmd[1], x);
    write16(&cmd[3], y);
    write16(&cmd[5], r);
    cmd[7] = fill ? 1 : 0;
    write16(&cmd[8], color);
    writeToBuffer(cmd, 10);
}

// CMD_TRIANGLE: 07 x0:2 y0:2 x1:2 y1:2 x2:2 y2:2 fill:1 color:2 = 16 bytes
void ScreenStreaming::logTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool fill, uint16_t color) {
    uint8_t cmd[16];
    cmd[0] = CMD_TRIANGLE;
    write16(&cmd[1], x0);
    write16(&cmd[3], y0);
    write16(&cmd[5], x1);
    write16(&cmd[7], y1);
    write16(&cmd[9], x2);
    write16(&cmd[11], y2);
    cmd[13] = fill ? 1 : 0;
    write16(&cmd[14], color);
    writeToBuffer(cmd, 16);
}

// CMD_TEXT: 08 x:2 y:2 color:2 font:1 len:1 chars... = 9 + len bytes
void ScreenStreaming::logText(int16_t x, int16_t y, uint16_t color, uint8_t fontId, const char* text) {
    uint8_t len = strlen(text);
    if (len > 200) len = 200; // Max text length
    
    uint8_t header[9];
    header[0] = CMD_TEXT;
    write16(&header[1], x);
    write16(&header[3], y);
    write16(&header[5], color);
    header[7] = fontId;
    header[8] = len;
    
    if (availableSpace() < 9 + len) return;
    
    writeToBuffer(header, 9);
    writeToBuffer((const uint8_t*)text, len);
}

// CMD_BITMAP: 09 x:2 y:2 w:2 h:2 color:2 dataLen:2 data... 
// For now, just log position and dimensions, skip actual bitmap data to save bandwidth
void ScreenStreaming::logBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, const uint8_t* data) {
    // Calculate bitmap data size (1 bit per pixel, packed)
    uint16_t dataLen = ((w + 7) / 8) * h;
    
    uint8_t header[13];
    header[0] = CMD_BITMAP;
    write16(&header[1], x);
    write16(&header[3], y);
    write16(&header[5], w);
    write16(&header[7], h);
    write16(&header[9], color);
    write16(&header[11], dataLen);
    
    if (availableSpace() < 13 + dataLen) return;
    
    writeToBuffer(header, 13);
    writeToBuffer(data, dataLen);
}

// CMD_CLEAR: FF color:2 = 3 bytes
void ScreenStreaming::logClear(uint16_t color) {
    uint8_t cmd[3];
    cmd[0] = CMD_CLEAR;
    write16(&cmd[1], color);
    writeToBuffer(cmd, 3);
}

void ScreenStreaming::checkSerialCommand() {
    if (!_enabled) return;
    
    // Check for streaming start/stop commands
    if (Serial.available() >= 3) {
        char cmd[4] = {0};
        cmd[0] = Serial.peek();
        
        if (cmd[0] == 'S') {
            Serial.read(); // consume 'S'
            if (Serial.available() >= 2) {
                cmd[1] = Serial.read();
                cmd[2] = Serial.read();
                
                if (cmd[1] == 'T' && cmd[2] == 'R') {
                    // "STR" = Start streaming
                    _streaming = true;
                    _head = 0;
                    _tail = 0;
                    _cmdCount = 0;
                    // Send acknowledgment header
                    Serial.write("STR");
                    Serial.write((uint8_t)0x01); // Version 1
                    Serial.flush();  // Wait for send to complete
                    delay(50);  // Give browser time to process sync
                } else if (cmd[1] == 'T' && cmd[2] == 'P') {
                    // "STP" = Stop streaming
                    _streaming = false;
                    Serial.write("STP");
                    Serial.flush();
                }
            }
        }
    }
}

void ScreenStreaming::streamPending() {
    // No longer needed - writeToBuffer now writes directly to Serial
}

#endif // ENABLE_SCREEN_STREAMING
