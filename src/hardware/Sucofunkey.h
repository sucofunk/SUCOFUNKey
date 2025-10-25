/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2024 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#ifndef Sucofunkey_h
#define Sucofunkey_h

#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <cppQueue.h> // https://github.com/SMFSW/Queue
#include "Configuration.h"

// Version 3 for SUCOFUNKey V3 (PCB Revision)

class Sucofunkey {
    public:
        // PIN number on Teensy 4.1 where the fader is connected to
        static const uint8_t faderPin = 40;
        static const uint8_t faderLEDPin = 35;

        static const byte KEY_NOTE = 1;
        static const byte KEY_OPERATION = 2;
        static const byte KEY_FN_NOTE = 3;
        static const byte KEY_FN_OPERATION = 4;
        static const byte ENCODER = 7;
        static const byte KEY_MENU_OPERATION = 8;
        static const byte KEY_MENU_NOTE = 9;
        static const byte EVENT_APPLICATION = 10;

        static const byte INPUT_NONE = 0;
        static const byte INPUT_MIC = 1;
        static const byte INPUT_LINE = 2;
        static const byte INPUT_RESAMPLE = 3;        
        static const byte INPUT_MIC_RESAMPLE = 4;
        static const byte INPUT_LINE_RESAMPLE = 5;        

        typedef struct keyQueueStruct {
            int   index;
            bool  pressed;
            bool  ignore;
            byte  type;  // KEY_NOTE | KEY_OPERATION | KEY_FN_NOTE | KEY_FN_OPERATION | KEY_FREE | ENCODER | APPLICATION
            int   value; // can be used to return values from callbacks
            byte  data1;
            byte  data2;
            byte  data3;
        } Key;

        Sucofunkey(Configuration *config);
        boolean hasKeyPressed();

        boolean hasEvents();
        keyQueueStruct getNextEvent();
        void addApplicationEventToQueue(int eventId);
        void addApplicationEventWithDataToQueue(int eventId, byte data1, byte data2, byte data3);
        void addApplicationEventWithValueDataToQueue(int eventId, int value, byte data1, byte data2, byte data3);

        void setIgnoreKeys(bool state);
        void setLEDState(int led, bool state);

        void setBank(byte nr);
        byte setBankUp();
        byte setBankDown();
        byte getBank();

        void setInput(byte nr);
        void showInputLEDs();
        void toggleInput();
        byte getInput();

        int getContinuousFaderValue(int scaleMin, int scaleMax);
        void updateContinuousFaderValue();
        int getFaderValue(int scaleMin, int scaleMax);
        void switchFaderLED(bool on);

        // The following values for keys/leds correspond to the MCP PINs.
        // 0..79
        // 0=MCP1 A0, 1=MCP1 A1, ..., 79=MCP5 B7
        static const int FUNCTION = 18;
        static const int MENU = 17;
        static const int SET = 16;
        static const int ZOOM = 34;
        static const int PLAY = 33;
        static const int PAUSE = 32;
        static const int RECORD = 53;
        static const int INPUTSELECTOR = 51;
        static const int CURSOR_LEFT = 67;
        static const int CURSOR_UP = 66;
        static const int CURSOR_DOWN = 65;
        static const int CURSOR_RIGHT = 64;

        // FN + Key -> Offset 100 + key
        static const int FN_FUNCTION = 118; // hold FN for 3 Seconds 
        static const int FN_MENU = 117;
        static const int FN_SET = 116;
        static const int FN_ZOOM = 134;
        static const int FN_PLAY = 133;
        static const int FN_PAUSE = 132;
        static const int FN_RECORD = 153;
        static const int FN_INPUTSELECTOR = 151;
        static const int FN_CURSOR_LEFT = 167;
        static const int FN_CURSOR_UP = 166;
        static const int FN_CURSOR_DOWN = 165;
        static const int FN_CURSOR_RIGHT = 164;

        // Menu + Key -> Offset 200 + key
        static const int MENU_MENU = 217;// 3 Seconds MENU to go to home screen
        static const int MENU_SET = 216;
        static const int MENU_ZOOM = 234;
        static const int MENU_PLAY = 233;
        static const int MENU_PAUSE = 232;
        static const int MENU_RECORD = 253;
        static const int MENU_INPUTSELECTOR = 251;
        static const int MENU_CURSOR_LEFT = 267;
        static const int MENU_CURSOR_UP = 266;
        static const int MENU_CURSOR_DOWN = 265;
        static const int MENU_CURSOR_RIGHT = 264;

        // SET + Key -> Offset 600 + key
        static const int SET_CURSOR_RIGHT = 664;
        static const int SET_CURSOR_LEFT = 667;
        static const int SET_CURSOR_UP = 666;
        static const int SET_CURSOR_DOWN = 665;        

        static const int ENCODER_1_PUSH = 10;
        static const int ENCODER_2_PUSH = 6;
        static const int ENCODER_3_PUSH = 3;
        static const int ENCODER_4_PUSH = 0;

        static const int FN_ENCODER_1_PUSH = 110;
        static const int FN_ENCODER_2_PUSH = 106;
        static const int FN_ENCODER_3_PUSH = 103;
        static const int FN_ENCODER_4_PUSH = 100;

        static const int MENU_ENCODER_1_PUSH = 210;
        static const int MENU_ENCODER_2_PUSH = 206;
        static const int MENU_ENCODER_3_PUSH = 203;
        static const int MENU_ENCODER_4_PUSH = 200;

        static const int F_1 = 20;
        static const int FS_1 = 22;
        static const int G_1 = 24;
        static const int GS_1 = 28;
        static const int A_1 = 26;
        static const int AS_1 = 35;
        static const int B_1 = 30;
        static const int C_1 = 37;
        static const int CS_1 = 40;
        static const int D_1 = 41;
        static const int DS_1 = 45;
        static const int E_1 = 43;
        static const int F_2 = 46;
        static const int FS_2 = 54;
        static const int G_2 = 57;
        static const int GS_2 = 61;
        static const int A_2 = 59;
        static const int AS_2 = 62;
        static const int B_2 = 69;
        static const int C_2 = 70;
        static const int CS_2 = 75;
        static const int D_2 = 73;
        static const int DS_2 = 78;
        static const int E_2 = 77;

        // FN + Key -> Offset 100 + key
        static const int FN_F_1 = 120;
        static const int FN_FS_1 = 122;
        static const int FN_G_1 = 124;
        static const int FN_GS_1 = 128;
        static const int FN_A_1 = 126;
        static const int FN_AS_1 = 135;
        static const int FN_B_1 = 130;
        static const int FN_C_1 = 137;
        static const int FN_CS_1 = 140;
        static const int FN_D_1 = 141;
        static const int FN_DS_1 = 145;
        static const int FN_E_1 = 143;
        static const int FN_F_2 = 146;
        static const int FN_FS_2 = 154;
        static const int FN_G_2 = 157;
        static const int FN_GS_2 = 161;
        static const int FN_A_2 = 159;
        static const int FN_AS_2 = 162;
        static const int FN_B_2 = 169;
        static const int FN_C_2 = 170;
        static const int FN_CS_2 = 175;
        static const int FN_D_2 = 173;
        static const int FN_DS_2 = 178;
        static const int FN_E_2 = 177;

        // --- LED Pins ---

        static const int LED_PLAY = 7;
        static const int LED_RECORD = 52;
        
        static const int LED_INPUT_MIC = 50;
        static const int LED_INPUT_LINE = 49;
        static const int LED_INPUT_RESAMPLE = 48;

        static const int LED_BANK_1 = 15;
        static const int LED_BANK_2 = 14;
        static const int LED_BANK_3 = 13;

        static const int LED_F_1 = 19;
        static const int LED_FS_1 = 21;
        static const int LED_G_1 = 23;
        static const int LED_GS_1 = 27;
        static const int LED_A_1 = 25;
        static const int LED_AS_1 = 31;
        static const int LED_B_1 = 29;
        static const int LED_C_1 = 36;
        static const int LED_CS_1 = 39;
        static const int LED_D_1 = 38;
        static const int LED_DS_1 = 44;
        static const int LED_E_1 = 42;
        static const int LED_F_2 = 47;
        static const int LED_FS_2 = 55;
        static const int LED_G_2 = 56;
        static const int LED_GS_2 = 60;
        static const int LED_A_2 = 58;
        static const int LED_AS_2 = 63;
        static const int LED_B_2 = 68;
        static const int LED_C_2 = 71;
        static const int LED_CS_2 = 74;
        static const int LED_D_2 = 72;
        static const int LED_DS_2 = 79;
        static const int LED_E_2 = 76;

        static const int ENCODER_1_1 = 12;
        static const int ENCODER_1_2 = 11;

        static const int ENCODER_2_1 = 9;
        static const int ENCODER_2_2 = 8;

        static const int ENCODER_3_1 = 5;
        static const int ENCODER_3_2 = 4;

        static const int ENCODER_4_1 = 2;
        static const int ENCODER_4_2 = 1;

        // Encoder events, will be pre-processed from ENCODER_N_1 and ENCODER_N_2
        static const int ENCODER_1 = 300;
        static const int ENCODER_2 = 301;
        static const int ENCODER_3 = 302;
        static const int ENCODER_4 = 303;

        static const int FN_ENCODER_1 = 400;
        static const int FN_ENCODER_2 = 401;
        static const int FN_ENCODER_3 = 402;
        static const int FN_ENCODER_4 = 403;


        // Application specific events (starting at 310)
        static const int SONGSELECTED = 310; // callback for song selector
        static const int INPUT_SET    = 311; // SET value at input field
        static const int INPUT_CANCEL = 312; // FN+SET to cancel a input
        static const int BOTTOM_NAV_ITEM1 = 313; 
        static const int BOTTOM_NAV_ITEM2 = 314;
        static const int BOTTOM_NAV_ITEM3 = 315;
        static const int RECORDED = 316;
        static const int RECORDER_CANCEL = 317;
        static const int CHECKREQUEST = 318; // system check for buttons, encoders, fader, led
        static const int SAMPLE_LIBRARY_SELECTED = 319; // sample in sampleselector selected -> used to select a sample from sample library at /SAMPLES/ on SD
        static const int SAMPLE_LIBRARY_CANCEL = 320;
        static const int SAMPLE_LIBRARY_PRELISTEN = 321;
        static const int SAMPLE_LIBRARY_STOP_PRELISTEN = 322;
        static const int SAMPLE_LIBRARY_REDRAW = 323;
        static const int MENU_BACK = 324;
        static const int ARRANGEMENT_PLAY_INDICATOR_CELL = 325;

        static const int BLACKKEY_NAV_ITEM1 = 340;
        static const int BLACKKEY_NAV_ITEM2 = 341;
        static const int BLACKKEY_NAV_ITEM3 = 342;                
        static const int BLACKKEY_NAV_ITEM4 = 343;
        static const int BLACKKEY_NAV_ITEM5 = 344;
        static const int BLACKKEY_NAV_ITEM6 = 345;
        static const int BLACKKEY_NAV_ITEM7 = 346;
        static const int BLACKKEY_NAV_ITEM8 = 347;        
        static const int BLACKKEY_NAV_ITEM9 = 348;
        static const int BLACKKEY_NAV_ITEM10 = 349;

        static const int MIDI_SEND_NOTE_ON = 500;
        static const int MIDI_SEND_NOTE_OFF = 501;
        static const int MIDI_SEND_ALL_NOTE_OFF = 502;
        static const int MIDI_SEND_ALL_NOTE_OFF_ALL_CHANNELS = 503;
        static const int MIDI_SEND_START = 504; // value = clock speed in microseconds
        static const int MIDI_SEND_STOP = 505;
        static const int MIDI_CHANGE_CLOCK_SPEED = 506; // value = clock speed in microseconds

        static const int LIVE_SNIPPET_START = 600;
        static const int LIVE_SNIPPET_STOP = 601;
        static const int LIVE_SNIPPET_WAITING_CHAINED = 602;
        static const int LIVE_SNIPPET_CANCEL_CHAINED = 603;

        static const int CHANGE_CONTEXT_TO_SYNTHCOPY = 701;
        static const int CHANGE_CONTEXT_TO_SETTINGS = 702;
        
        static const int SYNTHCOPY_START_NOTE = 720;
        static const int SYNTHCOPY_STOP_NOTE = 721;
        static const int SYNTHCOPY_START_RECORDING = 722;
        static const int SYNTHCOPY_NEXT_NOTE_REQUEST = 723;
        static const int SYNTHCOPY_STOP_RECORDING = 724;
        static const int SYNTHCOPY_NOTE_MARKER = 725;

        static const int ROUTE_LINE_IN_THROUGH = 801;
        static const int SETUP_LINE_INPUT_FROM_CONFIG = 802;

        static const int TIMER_GENERAL_START = 900;
        static const int TIMER_GENERAL_STOP = 901;        

        // just for debugging to Serial
        void printQueue();

        void switchLEDsOff();

        byte getSampleIdByEventKey(byte eventKey);
        byte getWhiteKeyByEventKey(byte eventKey);
        byte getLEDPinBySampleId(byte sampleId1);
        byte getLEDPinByEventKey(byte eventKey);
        byte getLEDPinByWhiteKey(byte whiteKey); // 1..14
        char getCharByEventKey(byte eventKey, byte index);        
        char getFilenameCharByEventKey(byte eventKey, byte index);    
        boolean isEventBlackKey(byte eventKey); // a black key from the piano keyboard.. false does not mean that it is a white key.. might be anything else
        boolean isEventWhiteKey(byte eventKey); // a white key from the piano keyboard.. false does not mean that it is a black key.. might be anything else
        byte getBlackKey1To10FromEventKey(byte eventKey); // returns which black key 1..10 is pressed

        String getMIDINoteName(byte note);
        void scanI2C(); // for debugging reasons

        void setScratchMute(boolean muted);
        boolean isScratchMuted();

        Configuration* getConfig();

    private:
        Configuration *_config;
       
        int _lastFaderReading = 0;
        int _lastFaderReadings[5] = {0, 0, 0, 0, 0};
        int _faderReadingPosition = 0;
        int _faderReading = 0;

        int _pseudoWatchdogCount = 0;

        Adafruit_MCP23017 _mcp1;
        Adafruit_MCP23017 _mcp2;
        Adafruit_MCP23017 _mcp3;
        Adafruit_MCP23017 _mcp4;
        Adafruit_MCP23017 _mcp5;

        // Is the Function Key held for a key combination?
        boolean _fnKeyHold;
        boolean _fnKeyInterrupted;
        unsigned long _fnKeyMillis;

        // Is the Menu Key held for a key combination?
        boolean _menuKeyHold;
        boolean _menuKeyInterrupted;
        unsigned long _menuKeyMillis;

        // Is the Set Key held for a key combination?
        boolean _setKeyHold;
        boolean _setKeyInterrupted;
        unsigned long _setKeyMillis;

        // needs to be true to e.g. not change menu context while recording
        boolean _ignoreKeys;
        boolean _isRecording;

        // are pins for Input (true) or Output (false) pins are: A0,..,A7,B0,..,B7
        // 0..15                       A0      A1      A2      A3      A4      A5      A6      A7      B0      B1      B2      B3      B4      B5      B6      B7
        bool _input_mcp1[16]      =  {true,   true,   true,   true,   true,   true,   true,   false,  true,   true,   true,   true,   true,   false,  false,  false};
        byte _input_type_mcp1[16] =  {KEY_OPERATION, ENCODER, ENCODER, KEY_OPERATION, ENCODER, ENCODER, KEY_OPERATION, 0, ENCODER, ENCODER, KEY_OPERATION, ENCODER, ENCODER, 0, 0, 0};
        //byte _input_type_mcp1[16] =  {KEY_NOTE, KEY_OPERATION, ENCODER, 0};

        // 16 .. 31
        bool _input_mcp2[16]      =  {true,   true,   true,   false,  true,   false,  true,   false,  true,   false,  true,   false,  true,   false,  true,   false};
        byte _input_type_mcp2[16] =  {KEY_OPERATION, KEY_OPERATION, KEY_OPERATION, 0, KEY_NOTE, 0, KEY_NOTE, 0, KEY_NOTE, 0, KEY_NOTE, 0, KEY_NOTE, 0, KEY_NOTE, 0};        

        // 32 .. 47
        bool _input_mcp3[16]      =  {true,   true,   true,   true,   false,  true,   false,  false,  true,   true,   false,  true,   false,  true,   true,  false};        
        byte _input_type_mcp3[16] =  {KEY_OPERATION, KEY_OPERATION, KEY_OPERATION, KEY_NOTE, 0, KEY_NOTE, 0, 0, KEY_NOTE, KEY_NOTE, 0, KEY_NOTE, 0, KEY_NOTE, KEY_NOTE, 0};

        // 48 .. 63
        bool _input_mcp4[16]      =  {false,  false,  false,  true,   false,  true,   true,   false,  false,  true,   false,  true,   false,  true,   true,   false};
        byte _input_type_mcp4[16] =  {0, 0, 0, KEY_OPERATION, 0, KEY_OPERATION, KEY_NOTE, 0, 0, KEY_NOTE, 0, KEY_NOTE, 0, KEY_NOTE, KEY_NOTE, 0};

        // 64 .. 71
        bool _input_mcp5[16]      =  {true,   true,   true,   true,   false,  true,   true,   false,  false,  true,   false,  true,   false,  true,   true,   false};
        byte _input_type_mcp5[16] =  {KEY_OPERATION, KEY_OPERATION, KEY_OPERATION, KEY_OPERATION, 0, KEY_NOTE, KEY_NOTE, 0, 0, KEY_NOTE, 0, KEY_NOTE, 0, KEY_NOTE, KEY_NOTE, 0};        
        
        uint16_t _values_mcp1_last = 0;
        uint16_t _values_mcp2_last = 0;
        uint16_t _values_mcp3_last = 0;
        uint16_t _values_mcp4_last = 0;
        uint16_t _values_mcp5_last = 0;

        uint16_t _values_mcp1_current = 0;
        uint16_t _values_mcp2_current = 0;
        uint16_t _values_mcp3_current = 0;
        uint16_t _values_mcp4_current = 0;
        uint16_t _values_mcp5_current = 0;
       
        // sampleIDs start at 1 -> F1 = 1 ... E2 = 32

        typedef struct lookUpTableStruct {
            int  sampleId;
            String name;
            char character[2];
            char filenameCharacter[2];
            byte LED_PIN;
            byte whiteKeyNr;
        } LookUpTableEntry;

        typedef struct {
            int noteKeysLookupTableINDEX;
            byte LED_PIN;            
        } WhiteKeysTable;


        typedef struct midiNoteLookUpTableStruct {
            String name;
        } MidiNoteLookUpTableEntry;

        //                                        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15
        //                                        16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31
        //                                        32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47
        //                                        48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63
        //                                        64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79

        const byte _eventKeyToLookUpTable[80] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 
                                                  127, 127, 127, 127, 0,   127, 1,   127, 2,   127, 4,   127, 3,   127, 6,   127,
                                                  127, 127, 127, 5,   127, 7,   127, 127, 8,   9,   127, 11,  127, 10,  12,  127,
                                                  127, 127, 127, 127, 127, 127, 13,  127, 127, 14,  127, 16,  127, 15,  17,  127,
                                                  127, 127, 127, 127, 127, 18,  19,  127, 127, 21,  127, 20,  127, 23,  22,  127 };


        const LookUpTableEntry _noteKeysLookUpTable[24] = {
            {  1, "F1",  {'A', 'B'}, {'A', 'B'}, LED_F_1, 1},
            {  2, "F#1", {'1', '!'}, {'1', '!'}, LED_FS_1, 0},
            {  3, "G1",  {'C', 'D'}, {'C', 'D'}, LED_G_1, 2},
            {  4, "G#1", {'2', '?'}, {'2', '2'}, LED_GS_1, 0},
            {  5, "A1",  {'E', 'F'}, {'E', 'F'}, LED_A_1, 3},
            {  6, "A#1", {'3', '-'}, {'3', '-'}, LED_AS_1, 0},
            {  7, "B1",  {'G', 'H'}, {'G', 'H'}, LED_B_1, 4},
            {  8, "C1",  {'I', 'J'}, {'I', 'J'}, LED_C_1, 5},
            {  9, "C#1", {'4', '_'}, {'4', '_'}, LED_CS_1, 0},
            { 10, "D1",  {'K', 'L'}, {'K', 'L'}, LED_D_1, 6},
            { 11, "D#1", {'5', '.'}, {'5', '5'}, LED_DS_1, 0},
            { 12, "E1",  {'M', 'N'}, {'M', 'N'}, LED_E_1, 7},
            { 13, "F2",  {'O', 'P'}, {'O', 'P'}, LED_F_2, 8},
            { 14, "F#2", {'6', ','}, {'6', '6'}, LED_FS_2, 0},
            { 15, "G2",  {'Q', 'R'}, {'Q', 'R'}, LED_G_2, 9},
            { 16, "G#2", {'7', ':'}, {'7', ':'}, LED_GS_2, 0},
            { 17, "A2",  {'S', 'T'}, {'S', 'T'}, LED_A_2, 10},
            { 18, "A#2", {'8', ';'}, {'8', '8'}, LED_AS_2, 0},
            { 19, "B2",  {'U', 'V'}, {'U', 'V'}, LED_B_2, 11},
            { 20, "C2",  {'W', 'X'}, {'W', 'X'}, LED_C_2, 12},
            { 21, "C#2", {'9', '#'}, {'9', '9'}, LED_CS_2, 0},
            { 22, "D2",  {'Y', 'Z'}, {'Y', 'Z'}, LED_D_2, 13},
            { 23, "D#2", {'0', '+'}, {'0', '0'}, LED_DS_2, 0},
            { 24, "E2",  {' ', ' '}, {' ', ' '}, LED_E_2, 14}
        };

        const WhiteKeysTable _whiteKeysTable[14] = {
            {0, LED_F_1},
            {2, LED_G_1},
            {4, LED_A_1},
            {6, LED_B_1},
            {7, LED_C_1},
            {9, LED_D_1},
            {11, LED_E_1},
            {12, LED_F_2},
            {14, LED_G_2},
            {16, LED_A_2},
            {18, LED_B_2},
            {19, LED_C_2},
            {21, LED_D_2},
            {23, LED_E_2}
        };

        // one entry for each rotary encoder
        uint8_t _encoderTempValues[4] = {0, 0, 0, 0};

        static void _intCallBack1();
        static void _intCallBack2();
        static void _intCallBack3();
        static void _intCallBack4();
        static void _intCallBack5();                
        static void _intCallBackEnc();

        void _setupInterrupts();

        void _handleKeyPressed();
        void _mcpKeysPressedToQueueEvents(uint16_t mcpValuesCurrent, uint16_t mcpValuesOld, uint16_t mcpOffset, bool* mcpInput, byte* mcpInputTypes);
        int _readRotaryEncoder(byte encoderNr, uint8_t valuePinL, uint8_t valuePinR, int eventId);

        bool getBooleanValueFrom16BitInt(uint16_t values, int pos);  

        byte _activeBank = 0;  // 1..3 defines the active bank (LED); 0 == all LEDs turned off
        byte _activeInput = INPUT_NONE;

        // "global" status variable -> only accessible via getter/setter
        boolean _scratchMute = false;
};

#endif