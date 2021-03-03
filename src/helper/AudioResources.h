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
        
        float currentVolume = 0.6;
        int currentMicGain = 12;
        byte currentLineInLevel = 11;

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
        AudioPlaySdRawSUCO       playSdRaw1;
        AudioPlaySdRawSUCO       playSdRaw2;
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