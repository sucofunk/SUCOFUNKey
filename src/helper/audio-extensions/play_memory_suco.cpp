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

    if (reverse) _position = (length/2)-2;
}


void AudioPlayMemorySUCO::stop(void)
{
	playing = 0;
     
     // reset play position and frequency data for next sample
    _position = 0.0f;
    _increment = 1.0f;
    _reverse = false;
}


void AudioPlayMemorySUCO::update(void)
{
	audio_block_t *block;
	const unsigned int *in;
	int16_t *out;
	uint32_t tmp32;

	if (!playing) return;
	block = allocate();
	if (block == NULL) return;

	out = block->data;

    for (int i=0; i < AUDIO_BLOCK_SAMPLES; i += 2) {        

        if (_startDelayRemainSamples > 0) {
            _startDelayRemainSamples--;
            *out++ = 0;
            *out++ = 0;
        } else {
            _positionInt = (int) _position;

            if ((uint32_t)_positionInt < length/2 && _position > -1) {
                in = beginning+_positionInt;
                tmp32 = *in;

                *out++ = (int16_t)(tmp32 & 65535);
                *out++ = (int16_t)(tmp32 >> 16);
            } else {
                *out++ = 0;
                *out++ = 0;
                playing = 0;                
            }

            if (_reverse) {
                _position -= _increment;
            } else {
                _position += _increment;
            }        
        }
    }
	
    // reset play position and frequency data for next sample
    if (playing == 0) {
        _position = 0.0f;
        _increment = 1.0f;
        _reverse = false;
    }

	transmit(block);
	release(block);    
}

void AudioPlayMemorySUCO::setBaseMIDINote(byte baseNote) {
    _baseNote = baseNote;
};

void AudioPlayMemorySUCO::setFrequencyByMIDINote(byte note) {
    _note = note;
    _increment = pow(2.0, (_note - _baseNote)/12.0);
};

void AudioPlayMemorySUCO::adjustFrequencyByTick(int pitchChange) {
    _increment += (pitchChange*0.01);
};

void AudioPlayMemorySUCO::reversePlayback() {
    _reverse = !_reverse;
};