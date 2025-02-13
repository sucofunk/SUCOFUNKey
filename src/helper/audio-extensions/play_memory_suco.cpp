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

   ---------------------------------------------------------------------------------------------- 
   
   This class is inspired by the original AudioPlayMemory from PJRC.

   */

#include <Arduino.h>
#include "play_memory_suco.h"

void AudioPlayMemorySUCO::play(const unsigned int *data)
{
    if (playing != 0) {
        stop();
    }

	uint32_t format;
	playing = 0;
	format = *data++;
	beginning = data;
	length = format & 0xFFFFFF;
	playing = format >> 24;    
    _startDelayRemainSamples = 0;
    _noteOff = false;
    _noteOffPercentage = 1.0f;
}


void AudioPlayMemorySUCO::playPitched(const unsigned int *data, byte baseNote, byte note, int startDelaySamples, boolean reverse)
{
    if (playing != 0) {
        stop();
    }

    setBaseMIDINote(baseNote);
    setFrequencyByMIDINote(note);

	uint32_t format;
	playing = 0;
	format = *data++;
	beginning = data;
	length = format & 0xFFFFFF;
	playing = format >> 24;    
    _startDelayRemainSamples = startDelaySamples;
    _reverse = reverse;

    _noteOff = false;
    _noteOffPercentage = 1.0f;

    if (reverse) _position = (length/2)-2;
}


void AudioPlayMemorySUCO::noteOff(void) {
    _noteOffPercentage = 1.0;
    _noteOff = true;
}


void AudioPlayMemorySUCO::stop(void)
{
	playing = 0;
     
     // reset play position and frequency data for next sample
    _position = 0.0f;
    _increment = 1.0f;
    _reverse = false;

    _noteOff = false;
    _noteOffPercentage = 1.0f;
}


void AudioPlayMemorySUCO::update(void)
{
    // adjust increment/speed for scratching sample
    if (playing && _playFaderPitched) {
        _faderValue = _keyboard->getFaderValue(-50,50);

        // left
        if (_faderValue < -2) {
            _reverse = true;
            _paused = false;
            _increment = ((_faderValue+2)*-0.0218);
            _keyboard->switchFaderLED(false);            
        } else {
            // right
            if (_faderValue > 2) {
                _paused = false;
                _reverse = false;
                _increment = ((_faderValue-2)*0.0218);
                _keyboard->switchFaderLED(false);
            } else {
                // middle position
                _paused = true;
                _reverse = false;
                _increment = 0.0f;
                _keyboard->switchFaderLED(true);
            }            
        }

    }


	audio_block_t *block;
	const unsigned int *in;
	int16_t *out;
	uint32_t tmp32;

	if (!playing) return;
	block = allocate();
	if (block == NULL) return;

	out = block->data;

//    int16_t p1;
//    int16_t p2;

    for (int i=0; i < AUDIO_BLOCK_SAMPLES; i += 2) {        

        // if delayed (swing) or paused, do not output anything
        if (_startDelayRemainSamples > 0 || _paused) {
            // decrease delay, if not paused                
            if (!_paused) { _startDelayRemainSamples--; };            
            *out++ = 0;
            *out++ = 0;
        } else {
            // .. or just play
            _positionInt = (int) _position;

            if ((uint32_t)_positionInt < length/2 && _position > -1) {
                in = beginning+_positionInt;
                tmp32 = *in;

// Bitcrushing
/*                    p1 = (int16_t)(tmp32 & 65535);
                    p2 = (int16_t)(tmp32 >> 16);   

                    p1 = (p1 & 65280);
                    p2 = (p2 & 65280);

                    *out++ = p1;
                    *out++ = p2;
*/

                if (_noteOff) {
                    // fade out..
                    _noteOffPercentage = _noteOffPercentage - _noteOffOffset;                    

                    if (_noteOffPercentage <= 0.0f) { 
                        playing = 0;
                        *out++ = 0;
                        *out++ = 0;
                    } else {
                        *out++ = (int16_t)((int16_t)(tmp32 & 65535))*_noteOffPercentage;
                        *out++ = (int16_t)((int16_t)(tmp32 >> 16))*_noteOffPercentage;
                    }
                } else {

                    if (_playFaderPitched && _keyboard->isScratchMuted()) {
                        *out++ = 0;
                        *out++ = 0;
                    } else {                        
                        *out++ = (int16_t)(tmp32 & 65535);
                        *out++ = (int16_t)(tmp32 >> 16);

                        // ToDo: add values to scratch visualization buffer.. might be a bit tricky
                    }

                }
            } else {
                *out++ = 0;
                *out++ = 0;
                playing = 0;
            }

            if (_reverse) {
                _position -= (_increment - _baseIncrementOffset);
            } else {
                _position += (_increment - _baseIncrementOffset);
            }        
        }
    }
	
    // reset play position and frequency data for next sample
    if (playing == 0) {
        _position = 0.0f;
        _increment = 1.0f;
        _baseIncrementOffset = 0.0f;
        _reverse = false;
        _noteOffPercentage = 1.0f;
        _noteOff = false;
    }

	transmit(block);
	release(block);    
}

void AudioPlayMemorySUCO::setBaseMIDINote(byte baseNote) {
    _baseNote = baseNote;
};

void AudioPlayMemorySUCO::setFrequencyByMIDINote(byte note) {
    _note = note;
    // offset for note change.. apply before pitching e.g. with fader..
    _baseIncrementOffset = 1.0 - pow(2.0, (_note - _baseNote)/12.0);
};

void AudioPlayMemorySUCO::adjustFrequencyByTick(int pitchChange) {
    _increment += (pitchChange*0.0075);
};

void AudioPlayMemorySUCO::reversePlayback() {
    _reverse = !_reverse;
};



void AudioPlayMemorySUCO::setKeyboardReference(Sucofunkey *keyboard) {
    _keyboard = keyboard;
    _isKeyboardSet = true;
    _playFaderPitched = false;    
}

void AudioPlayMemorySUCO::setPlayFaderPitched(boolean playFaderPitched) {
    _playFaderPitched = playFaderPitched;
};