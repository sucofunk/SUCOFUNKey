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
    recordMixer.gain(0, 0.6);
    recordMixer.gain(1, 0.6);
    mixerOutL.gain(0, 0.6);
    mixerOutR.gain(0, 0.6);
};

void AudioResources::muteResampling() {
    recordMixer.gain(2, 0.0);
    recordMixer.gain(3, 0.0);    
};

void AudioResources::unmuteResampling() {
    recordMixer.gain(2, 0.6);
    recordMixer.gain(3, 0.6);        
};

void AudioResources::setInputMic() {
    activeInput = AUDIO_INPUT_MIC;        
    audioShield.inputSelect(activeInput);
    audioShield.micGain(currentMicGain);
}

void AudioResources::setInputLine() {
    activeInput = AUDIO_INPUT_LINEIN;
    audioShield.inputSelect(activeInput);
    audioShield.lineInLevel(currentLineInLevel);
}
