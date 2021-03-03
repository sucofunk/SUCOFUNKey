#include <Arduino.h>
#include <Audio.h>
#include "AudioResources.h"

AudioResources::AudioResources() {};


void AudioResources::increaseMicGain() {
    if (currentMicGain < maxMicGain) {
        currentMicGain++;
    } 
    audioShield.micGain(currentMicGain);
};

void AudioResources::decreaseMicGain() {
    if (currentMicGain > 0) {
        currentMicGain--;
    } 
    audioShield.micGain(currentMicGain);
};

void AudioResources::increaseLineInVolume() {    
    if (currentLineInLevel < maxLineInLevel) {
        currentLineInLevel++;
    } 
    audioShield.lineInLevel(currentLineInLevel);
};

void AudioResources::decreaseLineInVolume() {
    if (currentLineInLevel > 0) {
        currentLineInLevel--;
    } 
    audioShield.lineInLevel(currentLineInLevel);
};

void AudioResources::muteInput() {
    recordMixer.gain(0, 0.0);
    recordMixer.gain(1, 0.0);
    mixerOutL.gain(0, 0.0);
    mixerOutR.gain(0, 0.0);
};

void AudioResources::unmuteInput() {
    recordMixer.gain(0, 0.8);
    recordMixer.gain(1, 0.8);
    mixerOutL.gain(0, 0.8);
    mixerOutR.gain(0, 0.8);

    audioShield.inputSelect(activeInput);

    audioShield.lineInLevel(currentLineInLevel);
    audioShield.micGain(currentMicGain);        
};

void AudioResources::muteResampling() {
    recordMixer.gain(2, 0.0);
    recordMixer.gain(3, 0.0);    
};

void AudioResources::unmuteResampling() {
    recordMixer.gain(2, 0.8);
    recordMixer.gain(3, 0.8);        
};

void AudioResources::setInputMic() {
    activeInput = AUDIO_INPUT_MIC;    
    audioShield.inputSelect(activeInput);
    audioShield.micGain(currentMicGain);
}

void AudioResources::setInputLine() {
    activeInput = AUDIO_INPUT_LINEIN;
    audioShield.inputSelect(activeInput);
}
