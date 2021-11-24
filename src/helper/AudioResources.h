#ifndef AudioResources_h
#define AudioResources_h

#include <Arduino.h>
#include <Audio.h>
#include "audio-extensions/play_sd_raw_suco.h"

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
        byte currentLineInLevel = 1;

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

        AudioSynthWavetable      wavetable1;
        AudioSynthWavetable      wavetable2;
        AudioSynthWavetable      wavetable3;
        AudioSynthWavetable      wavetable6;
        AudioSynthWavetable      wavetable7;
        AudioSynthWavetable      wavetable8;
        AudioPlayMemory          playMem7;
        AudioSynthWavetable      wavetable4;
        AudioPlayMemory          playMem1;
        AudioPlayMemory          playMem8;
        AudioSynthWavetable      wavetable5;
        AudioPlayMemory          playMem2;
        AudioPlayMemory          playMem3;
        AudioPlayMemory          playMem4;
        AudioPlayMemory          playMem5;
        AudioPlayMemory          playMem6;
        AudioInputI2S            audioInput;
        AudioPlaySdRawSUCO       playSdRaw;
        AudioPlayMemory          playMem;
        AudioMixer4              recordMixer;
        AudioMixer4              mixerSDL;
        AudioMixer4              mixerSDR;
        AudioMixer4              mixerMem2L;
        AudioMixer4              mixerMem2R;
        AudioMixer4              mixerMem1L;
        AudioMixer4              mixerMem1R;
        AudioMixer4              mixerWav2L;
        AudioMixer4              mixerWav2R;
        AudioMixer4              mixerWav1L;
        AudioMixer4              mixerWav1R;
        AudioAnalyzePeak         peak1;
        AudioRecordQueue         queue1;
        AudioMixer4              mixerMemL;
        AudioMixer4              mixerMemR;
        AudioMixer4              mixerWavL;
        AudioMixer4              mixerWavR;
        AudioMixer4              mixerPreOutL;
        AudioMixer4              mixerPreOutR;
        AudioMixer4              mixerOutL;
        AudioMixer4              mixerOutR;
        AudioOutputI2S           audioOutput;
        AudioControlSGTL5000     audioShield;
};

#endif