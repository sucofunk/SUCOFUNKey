/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2023 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#ifndef AudioResources_h
#define AudioResources_h

#include <Arduino.h>
#include <Audio.h>
#include "audio-extensions/play_sd_raw_suco.h"
#include "audio-extensions/play_memory_suco.h"

class AudioResources {

    public:

        AudioResources();

        int activeInput = AUDIO_INPUT_LINEIN;
        
        const float maxVolume = 1.0;
        const int maxMicGain = 63;
        const byte maxLineInLevel = 15; 
        
        float currentVolume = 0.5;
        int currentMicGain = 20; // 0..63

        /*
            0: 3.12 Volts p-p
            1: 2.63 Volts p-p
            2: 2.22 Volts p-p
            3: 1.87 Volts p-p
            4: 1.58 Volts p-p
            5: 1.33 Volts p-p  (default)
            6: 1.11 Volts p-p
            7: 0.94 Volts p-p
            8: 0.79 Volts p-p
            9: 0.67 Volts p-p
            10: 0.56 Volts p-p
            11: 0.48 Volts p-p
            12: 0.40 Volts p-p
            13: 0.34 Volts p-p
            14: 0.29 Volts p-p
            15: 0.24 Volts p-p
            
            Source: Teensy Documentation
        */
        byte currentLineInLevel = 0;

        void increaseMicGain();
        void decreaseMicGain();
        void increaseLineInVolume();
        void decreaseLineInVolume();
        void muteInput();
        void unmuteInput();
        void muteResampling();
        void unmuteResampling();
        void setInputMic();
        void setInputLine();

        AudioPlayMemorySUCO          playMem;

        AudioPlayMemorySUCO          playMem1;
        AudioPlayMemorySUCO          playMem2;
        AudioPlayMemorySUCO          playMem3;
        AudioPlayMemorySUCO          playMem4;
        AudioPlayMemorySUCO          playMem5;
        AudioPlayMemorySUCO          playMem6;
        AudioPlayMemorySUCO          playMem7;
        AudioPlayMemorySUCO          playMem8;
        AudioPlayMemorySUCO          playMem9;
        AudioPlayMemorySUCO          playMem10;
        AudioPlayMemorySUCO          playMem11;
        AudioPlayMemorySUCO          playMem12;
        AudioPlayMemorySUCO          playMem13;
        AudioPlayMemorySUCO          playMem14;
        AudioPlayMemorySUCO          playMem15;
        AudioPlayMemorySUCO          playMem16;

        AudioPlayMemorySUCO          playMemLive1;
        AudioPlayMemorySUCO          playMemLive2;
        AudioPlayMemorySUCO          playMemLive3;
        AudioPlayMemorySUCO          playMemLive4;
        AudioPlayMemorySUCO          playMemLive5;
        AudioPlayMemorySUCO          playMemLive6;
        AudioPlayMemorySUCO          playMemLive7;
        AudioPlayMemorySUCO          playMemLive8;

        AudioPlaySdRawSUCO       playSdRaw;

        AudioInputI2S            audioInput;
        AudioOutputI2S           audioOutput;
        AudioControlSGTL5000     audioShield;
        AudioAnalyzePeak         peak1;
        AudioRecordQueue         queue1;

        AudioMixer4              mixerPreOutL;
        AudioMixer4              mixerPreOutR;

        AudioMixer4              recordMixer;

        AudioMixer4              mixerOutL;
        AudioMixer4              mixerOutR;

        AudioMixer4              mixerSDL;
        AudioMixer4              mixerSDR;

        AudioMixer4              mixerMemL;
        AudioMixer4              mixerMemR;

        AudioMixer4              mixerMemLiveL;
        AudioMixer4              mixerMemLiveR;

        AudioMixer4              mixerMem1L;
        AudioMixer4              mixerMem1R;
        AudioMixer4              mixerMem2L;
        AudioMixer4              mixerMem2R;
        AudioMixer4              mixerMem3L;
        AudioMixer4              mixerMem3R;
        AudioMixer4              mixerMem4L;
        AudioMixer4              mixerMem4R;
        AudioMixer4              mixerMem5L;
        AudioMixer4              mixerMem5R;
        AudioMixer4              mixerMem6L;
        AudioMixer4              mixerMem6R;

/*
        // from Open Audio for phaseshifter
        AudioConvert_I16toF32    cnvrt1;    // Convert to float
        AudioConvert_F32toI16    cnvrt2;

        RadioIQMixer_F32         iqmixer1;
        AudioFilter90Deg_F32     hilbert1;
        AudioMixer4_F32          sum1;      // Summing node for the SSB receiver
        AudioFilterFIR_F32       fir1;      // Low Pass Filter to frequency limit the SSB
        AudioEffectDelay_OA_F32  delay1;    // Pleasant and useful sound between L & R
*/
};

#endif