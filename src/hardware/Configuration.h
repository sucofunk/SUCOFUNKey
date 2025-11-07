/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2025 by Marc Berendes (marc @ sucofunk.com)
    
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

#ifndef Configuration_h
#define Configuration_h

#define VERSIONNUMBER "0.9.7.3 - Aufholjagd"

// Display configuration -----------------------------

// 2.4 inch screen waveshare: SCREEN_ILI9341 with DMA Framebuffer
#define SCREEN_ILI9341_DMA

// 2.4 inch screen waveshare: SCREEN_ILI9341 with Adafruit driver (no Framebuffer)
//#define SCREEN_ILI9341

// 2 inch screen waveshare: SCREEN_ST7789
//#define SCREEN_ST7789

// PINs for the display 
// MOSI == DIN
#define PIN_SCREEN_SCK (27)
#define PIN_SCREEN_MOSI (26)
#define PIN_SCREEN_CS (38)
#define PIN_SCREEN_DC (39)
#define PIN_SCREEN_RST (37)
#define PIN_SCREEN_BL (36)

// ---------------------------------------------------

// PINs on the Teensy 4.1 to receive Interrupts from the 5 MCP23017 chips used as port expanders to receive updated from the 36 Buttons and 4 rotary encoders
// The MCP23017s control the 32 LEDs in the SUCOFUNKey, too
#define PIN_SUCOKEY_INT_1 (28)
#define PIN_SUCOKEY_INT_2 (29) 
#define PIN_SUCOKEY_INT_3 (30) 
#define PIN_SUCOKEY_INT_4 (31)
#define PIN_SUCOKEY_INT_5 (32) 

// Analog PIN where volume potentiometer is connected to
#define PIN_VOLUME (22)


class Configuration {
    public:
        enum ValueType {
            TBD = 0,
            MIDICHANNEL = 1,
            TRUEFALSE = 2
        };


        typedef struct {
            int version = 2; // configuration type.. to check, if there should be problems in the future, when more options are available

            int midiChannelPlay = 1; // MIDI channel to receive notes for play/live mode
            ValueType midiChannelPlayValueType = MIDICHANNEL;

            int midiChannelPiano = 2; // MIDI channel to receive notes for piano mode
            ValueType midiChannelPianoValueType = MIDICHANNEL;

            bool sendMidiMasterClock = 1; // send MIDI master clock when playing in sequencer, arranger or live? // 1 == true 0 == false
            ValueType sendMidiMasterClockValueType = TRUEFALSE;

            bool sendMidiStartStop = 1; // send MIDI start/stop?
            ValueType sendMidiStartStopValueType = TRUEFALSE;

            bool receiveUSBAudio = 0; // will replace the line-in with usb oudio in
            ValueType receiveUSBAudioValueType = TRUEFALSE;

        } ConfigurationValues;
        

        Configuration() {};

        ConfigurationValues configurationValues;
};

#endif