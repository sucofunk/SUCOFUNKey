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
   
#ifndef AudioResources_h
#define AudioResources_h

#include <Arduino.h>
#include <Audio.h>
#include "audio-extensions/play_sd_raw_suco.h"
#include "audio-extensions/synth_wavetable_suco.h"


class AudioResources {

    public:

        AudioResources();

        int activeInput = AUDIO_INPUT_LINEIN;
        
        const float maxVolume = 1.0;
        const int maxMicGain = 64;
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

        AudioSynthWavetableSUCO      wavetable1;
        AudioSynthWavetableSUCO      wavetable2;
        AudioSynthWavetableSUCO      wavetable3;
        AudioSynthWavetableSUCO      wavetable4;
        AudioSynthWavetableSUCO      wavetable5;
        AudioSynthWavetableSUCO      wavetable6;
        AudioSynthWavetableSUCO      wavetable7;
        AudioSynthWavetableSUCO      wavetable8;
        
        AudioPlayMemory          playMem;
        AudioPlayMemory          playMem1;
        AudioPlayMemory          playMem2;
        AudioPlayMemory          playMem3;
        AudioPlayMemory          playMem4;
        AudioPlayMemory          playMem5;
        AudioPlayMemory          playMem6;
        AudioPlayMemory          playMem7;
        AudioPlayMemory          playMem8;

        AudioPlayMemory          playMemLive1;
        AudioPlayMemory          playMemLive2;
        AudioPlayMemory          playMemLive3;
        AudioPlayMemory          playMemLive4;
        AudioPlayMemory          playMemLive5;
        AudioPlayMemory          playMemLive6;
        AudioPlayMemory          playMemLive7;
        AudioPlayMemory          playMemLive8;

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

        AudioMixer4              mixerWavL;
        AudioMixer4              mixerWavR;

        AudioMixer4              mixerWav1L;
        AudioMixer4              mixerWav1R;
        AudioMixer4              mixerWav2L;
        AudioMixer4              mixerWav2R;

        AudioMixer4              mixerMemL;
        AudioMixer4              mixerMemR;
        
        AudioMixer4              mixerMem1L;
        AudioMixer4              mixerMem1R;
        AudioMixer4              mixerMem2L;
        AudioMixer4              mixerMem2R;
        AudioMixer4              mixerMem3L;
        AudioMixer4              mixerMem3R;
        AudioMixer4              mixerMem4L;
        AudioMixer4              mixerMem4R;


        // Synth section
        AudioSynthWavetableSUCO      wavetableSynth5;
        AudioSynthWavetableSUCO      wavetableSynth6;
        AudioSynthWavetableSUCO      wavetableSynth4;
        AudioSynthWavetableSUCO      wavetableSynth2;
        AudioSynthWavetableSUCO      wavetableSynth3;
        AudioSynthWavetableSUCO      wavetableSynth1;
        AudioMixer4              mixerSynth1;    
        AudioMixer4              mixerSynth2;   
        AudioMixer4              mixerSynth;    
};

#endif