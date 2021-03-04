#ifndef Sucofunkey_h
#define Sucofunkey_h

#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <cppQueue.h> // https://github.com/SMFSW/Queue

class Sucofunkey {
    public:
        static const byte KEY_NOTE = 1;
        static const byte KEY_OPERATION = 2;
        static const byte KEY_FN_NOTE = 3;
        static const byte KEY_FN_OPERATION = 4;
        static const byte KEY_FREE = 5; // free to use MCP3 (GPB3-7) PINs
        static const byte KEY_FN_FREE = 6; // free to use MCP3 (GPB3-7) PINs
        static const byte ENCODER = 7;
        static const byte KEY_MENU_SELECTOR = 8;
        static const byte KEY_MENU_NOTE = 9;
        static const byte EVENT_APPLICATION = 10;

        static const byte INPUT_NONE = 0;
        static const byte INPUT_MIC = 1;
        static const byte INPUT_LINE = 2;
        static const byte INPUT_RESAMPLE = 3;        
        static const byte INPUT_MIC_RESAMPLE = 4;
        static const byte INPUT_LINE_RESAMPLE = 5;

        typedef struct keyQueueStruct {
            byte  index;
            bool  pressed;
            bool  ignore;
            byte  type;  // KEY_NOTE | KEY_OPERATION | KEY_FN_NOTE | KEY_FN_OPERATION | KEY_FREE | ENCODER | APPLICATION
            int   value; // can be used to return values from callbacks
        } Key;

        Sucofunkey(int intKeyPin1, int intEncPin, int intKeyPin2, int intKeyPin3);
        boolean hasKeyPressed();

        boolean hasEvents();
        boolean hasEncoderChange();
        keyQueueStruct getNextEvent();
        void addApplicationEventToQueue(byte eventId);
        void setIgnoreKeys(bool state);
        void setLEDState(byte led, bool state);

        void setBank(byte nr);
        void setBankUp();
        void setBankDown();
        byte getBank();

        void setInput(byte nr);
        void toggleInput();
        byte getInput();


        // MCP 1/2/3 for Keys -> Encoders are handled separately

        static const byte FUNCTION = 40;
        static const byte MENU = 41;
        static const byte SET = 42;
        static const byte PLAY = 16;
        static const byte PAUSE = 17;
        static const byte RECORD = 18;
        static const byte INPUTSELECTOR = 19;
        static const byte CURSOR_LEFT = 20;
        static const byte CURSOR_UP = 21;
        static const byte CURSOR_DOWN = 22;
        static const byte CURSOR_RIGHT = 23;

        static const byte FN_FUNCTION = 90; // 3 Seconds FN to go to settings
        static const byte FN_MENU = 91;
        static const byte FN_SET = 92;
        static const byte FN_PLAY = 66;
        static const byte FN_PAUSE = 67;
        static const byte FN_RECORD = 68;
        static const byte FN_INPUTSELECTOR = 69;
        static const byte FN_CURSOR_LEFT = 70;
        static const byte FN_CURSOR_UP = 71;
        static const byte FN_CURSOR_DOWN = 72;
        static const byte FN_CURSOR_RIGHT = 73;

        static const byte MENU_MENU = 141; // 3 Seconds MENU to go to home screen
        static const byte MENU_SET = 142;
        static const byte MENU_PLAY = 116;
        static const byte MENU_PAUSE = 117;
        static const byte MENU_RECORD = 118;
        static const byte MENU_INPUTSELECTOR = 119;
        static const byte MENU_CURSOR_LEFT = 120;
        static const byte MENU_CURSOR_UP = 121;
        static const byte MENU_CURSOR_DOWN = 122;
        static const byte MENU_CURSOR_RIGHT = 123;

        static const byte ENCODER_1_PUSH = 43;
        static const byte ENCODER_2_PUSH = 44;
        static const byte ENCODER_3_PUSH = 45;
        static const byte ENCODER_4_PUSH = 46;
        static const byte GPB7 = 47;

        static const byte FN_ENCODER_1_PUSH = 93;
        static const byte FN_ENCODER_2_PUSH = 94;
        static const byte FN_ENCODER_3_PUSH = 95;
        static const byte FN_ENCODER_4_PUSH = 96;
        static const byte FN_GPB7 = 97;

        static const byte MENU_GPB3 = 143;
        static const byte MENU_GPB4 = 144;
        static const byte MENU_GPB5 = 145;
        static const byte MENU_GPB6 = 146;
        static const byte MENU_GPB7 = 147;

        static const byte F_1 = 8;
        static const byte FS_1 = 9;
        static const byte G_1 = 10;
        static const byte GS_1 = 11;
        static const byte A_1 = 12;
        static const byte AS_1 = 13;
        static const byte B_1 = 14;
        static const byte C_1 = 15;
        static const byte CS_1 = 0;
        static const byte D_1 = 1;
        static const byte DS_1 = 2;
        static const byte E_1 = 3;
        static const byte F_2 = 4;
        static const byte FS_2 = 5;
        static const byte G_2 = 6;
        static const byte GS_2 = 7;
        static const byte A_2 = 24;
        static const byte AS_2 = 25;
        static const byte B_2 = 26;
        static const byte C_2 = 27;
        static const byte CS_2 = 28;
        static const byte D_2 = 29;
        static const byte DS_2 = 30;
        static const byte E_2 = 31;

        // Offset 50 for Function Key + Note Keys
        static const byte FN_F_1 = 58;
        static const byte FN_FS_1 = 59;
        static const byte FN_G_1 = 60;
        static const byte FN_GS_1 = 61;
        static const byte FN_A_1 = 62;
        static const byte FN_AS_1 = 63;
        static const byte FN_B_1 = 64;
        static const byte FN_C_1 = 65;
        static const byte FN_CS_1 = 50;
        static const byte FN_D_1 = 51;
        static const byte FN_DS_1 = 52;
        static const byte FN_E_1 = 53;
        static const byte FN_F_2 = 54;
        static const byte FN_FS_2 = 55;
        static const byte FN_G_2 = 56;
        static const byte FN_GS_2 = 57;
        static const byte FN_A_2 = 74;
        static const byte FN_AS_2 = 75;
        static const byte FN_B_2 = 76;
        static const byte FN_C_2 = 77;
        static const byte FN_CS_2 = 78;
        static const byte FN_D_2 = 79;
        static const byte FN_DS_2 = 80;
        static const byte FN_E_2 = 81;

        // MCP 4 & 5 for LEDs
        static const byte LED_PLAY = 7;
        static const byte LED_RECORD = 6;
        
        static const byte LED_INPUT_MIC = 5;
        static const byte LED_INPUT_LINE = 4;
        static const byte LED_INPUT_RESAMPLE = 3;

        static const byte LED_BANK_1 = 2;
        static const byte LED_BANK_2 = 1;
        static const byte LED_BANK_3 = 0;

        static const byte LED_F_1 = 24;
        static const byte LED_FS_1 = 25;
        static const byte LED_G_1 = 26;
        static const byte LED_GS_1 = 27;
        static const byte LED_A_1 = 28;
        static const byte LED_AS_1 = 29;
        static const byte LED_B_1 = 30;
        static const byte LED_C_1 = 31;
        static const byte LED_CS_1 = 16;
        static const byte LED_D_1 = 17;
        static const byte LED_DS_1 = 19;
        static const byte LED_E_1 = 18;
        static const byte LED_F_2 = 20;
        static const byte LED_FS_2 = 21;
        static const byte LED_G_2 = 22;
        static const byte LED_GS_2 = 23;
        static const byte LED_A_2 = 8;
        static const byte LED_AS_2 = 9;
        static const byte LED_B_2 = 10;
        static const byte LED_C_2 = 11;
        static const byte LED_CS_2 = 12;
        static const byte LED_D_2 = 13;
        static const byte LED_DS_2 = 14;
        static const byte LED_E_2 = 15;

        static const byte ENCODER_1 = 200;
        static const byte ENCODER_2 = 201;
        static const byte ENCODER_3 = 202;
        static const byte ENCODER_4 = 203;                        

        // Application specific events
        static const byte SONGSELECTED = 210; // callback for song selector
        static const byte INPUT_SET    = 211; // SET value at input field
        static const byte INPUT_CANCEL = 212; // FN+SET to cancel a input
        static const byte BOTTOM_NAV_ITEM1 = 213; 
        static const byte BOTTOM_NAV_ITEM2 = 214;
        static const byte BOTTOM_NAV_ITEM3 = 215;
        static const byte RECORDED = 216;

        // just for debugging to Serial
        void printQueue();

        void switchLEDsOff();

        byte getSampleIdByEventKey(byte eventKey);
        byte getLEDPinBySampleId(byte sampleId1);
        byte getLEDPinByEventKey(byte eventKey);
        char getCharByEventKey(byte eventKey, byte index);        
        char getFilenameCharByEventKey(byte eventKey, byte index);        

    private:
        // Interrupt Pins on Teensy from Sucokey
        int _intKeyPin1;
        int _intKeyPin2;
        int _intKeyPin3;
        int _intEncPin;
       
        Adafruit_MCP23017 _mcp1;
        Adafruit_MCP23017 _mcp2;
        Adafruit_MCP23017 _mcp3;
        Adafruit_MCP23017 _mcp4;
        Adafruit_MCP23017 _mcp5;

        // Is the Function Key held for a key combination?
        boolean _fnKeyHold;
        boolean _fnKeyInterrupted;
        boolean _menuKeyHold;
        boolean _menuKeyInterrupted;
        unsigned long  _fnKeyMillis;
        unsigned long  _menuKeyMillis;
        
        // needs to be true to e.g. not change menu context while recording
        boolean _ignoreKeys;
        boolean _isRecording;
        // Sucokey current values, used to identify status changes
        bool _values_mcp1[16] =  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
        bool _values_mcp2[16] =  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
        bool _values_mcp3[16] =  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
       
        // sampleIDs start at 1 -> F1 = 1 ... E2 = 32
        const byte _eventKeyToSampleId[32] = {  9, 10, 11, 12, 13, 14, 15, 16,  1,  2, 
                                                3,  4,  5,  6,  7,  8,  0,  0,  0,  0, 
                                                0,  0,  0,  0, 17, 18, 19, 20, 21, 22, 
                                               23, 24 };
        const byte _sampleIdToLEDPIN[25]  =  { 0, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 19, 18, 20, 21, 22, 23, 8, 9, 10, 11, 12, 13, 14, 15 };

        const char _eventKeyToChar[32][2] = {{'4', '_'},{'K', 'L'},{'5', '.'},{'M', 'N'},{'O', 'P'},{'6', ','},{'Q', 'R'},{'7', ':'},{'A', 'B'},{'1', '!'},
                                             {'C', 'D'},{'2', '?'},{'E', 'F'},{'3', '-'},{'G', 'H'},{'I', 'J'},{' ', ' '},{' ', ' '},{' ', ' '},{' ', ' '},
                                             {' ', ' '},{' ', ' '},{' ', ' '},{' ', ' '},{'S', 'T'},{'8', ';'},{'U', 'V'},{'W', 'X'},{'9', '#'},{'Y', 'Z'},
                                             {'0', '+'},{' ', ' '}};
        const char _eventKeyToCharFilename[32][2] = {
                                            {'4', '_'},{'K', 'L'},{'5', '5'},{'M', 'N'},{'O', 'P'},{'6', '6'},{'Q', 'R'},{'7', ':'},{'A', 'B'},{'1', '!'},
                                             {'C', 'D'},{'2', '2'},{'E', 'F'},{'3', '-'},{'G', 'H'},{'I', 'J'},{' ', ' '},{' ', ' '},{' ', ' '},{' ', ' '},
                                             {' ', ' '},{' ', ' '},{' ', ' '},{' ', ' '},{'S', 'T'},{'8', '8'},{'U', 'V'},{'W', 'X'},{'9', '#'},{'Y', 'Z'},
                                             {'0', ' '},{' ', ' '}};


        // one entry for each rotary encoder
        uint8_t _encoderTempValues[4] = {0, 0, 0, 0};

        static void _intCallBack1();
        static void _intCallBack2();
        static void _intCallBack3();
        static void _intCallBackEnc();

        void _setupInterrupts();

        void _handleKeyPressed();

        void _handleEncoderUpdate();
        int _readRotaryEncoder(byte encoderNr, uint8_t valuePinL, uint8_t valuePinR, byte eventId);

        byte _activeBank = 0;  // 1..3 defines the active bank (LED); 0 == all LEDs turned off
        byte _activeInput = INPUT_NONE;
};

#endif
