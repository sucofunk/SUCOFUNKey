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

#ifndef play_memory_suco_h_
#define play_memory_suco_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "../../hardware/Sucofunkey.h"

class AudioPlayMemorySUCO : public AudioStream
{
public:
	AudioPlayMemorySUCO(void) : AudioStream(0, NULL), playing(0) { }
	void play(const unsigned int *data);
    void playPitched(const unsigned int *data, byte baseNote, byte note, int startDelaySamples, boolean reverse);
	void stop(void);
	void noteOff(void);
	bool isPlaying(void) { return playing == 0 ? false : true ; }
	virtual void update(void);

    void setBaseMIDINote(byte baseNote);
    void setFrequencyByMIDINote(byte note);
	void adjustFrequencyByTick(int pitchChange);
    void reversePlayback(); // if it is already playing backward, it will play forward afterwards

    void setKeyboardReference(Sucofunkey *keyboard);
    void setPlayFaderPitched(boolean playFaderPitched);

private:
    Sucofunkey* _keyboard;
    int _faderValue = 0;
    boolean _isKeyboardSet = false;
    boolean _playFaderPitched = false;
    boolean _paused = false;

	const unsigned int *next;
	const unsigned int *beginning;
	const unsigned int *_sampleData;

	uint32_t length;
	volatile uint8_t playing;

    double _position = 1.0;
    uint32_t _positionInt = 1;
    double _increment = 1.0;
    double _baseIncrementOffset = 0.0f;

    byte _baseNote = 60;
    byte _note = 60;

    boolean _reverse = false;
    boolean _noteOff = false;
    double _noteOffPercentage = 1.0;
    double _noteOffOffset = 0.0005;

    int _startDelayRemainSamples = 0;
};

#endif
