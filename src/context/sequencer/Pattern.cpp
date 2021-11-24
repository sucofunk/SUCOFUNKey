#include <Arduino.h>
#include "Pattern.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"

Pattern::Pattern(uint16_t patternLength, SampleFSIO *sfsio, AudioResources *audioResources)
{
    _patternLength = patternLength;
    _sfsio = sfsio;
    _clearPattern();
    _audioResources = audioResources;    
};

void Pattern::setPatternLength(uint16_t patternLength)
{
    _patternLength = patternLength;
};

void Pattern::setPatternSpeed(float patternSpeed)
{
    if (patternSpeed < 30.0) {
        _patternSpeed = 30.0;
        return;
    }

    if (patternSpeed > 300.0) { 
        _patternSpeed = 300.0;
        return;
    }

    _patternSpeed = patternSpeed;
};

uint16_t Pattern::getPatternLength()
{
    return _patternLength;
};

float Pattern::getPatternSpeed()
{
    return _patternSpeed;
};


void Pattern::setPatternResolution(byte res) {
    _patternResolution = res;
};

byte Pattern::getPatternResolution() {
    return _patternResolution;
};

void Pattern::changePatternResolutionByTick(boolean increase) {
    if (increase && _patternResolution < 16) {
        _patternResolution++;
    } 

    if (!increase && _patternResolution > 2) {
        _patternResolution--;
    }     
};


void Pattern::changePatternLengthByTick(boolean increase) {
    if (increase && _patternLength < _maxPatternLength) {
        _patternLength++;
    } 

    if (!increase && _patternLength > 1) {
        _patternLength--;
    } 
}

boolean Pattern::shiftResolution(boolean up) {
    // patternlength would exceed max pattern length.. another option would be deleting the samples that would not fit into the pattern
    if (up && _patternResolution*2 > _maxPatternLength) return false;

    // some resolutions cannot be shifted down.. 7 / 2 = 3.5 or 2/2 = 1 (too small)
    if (!up && (_patternResolution%2 != 0 || _patternResolution/2 < 2)) return false;

    if (up) {
        // shifting up
        for (int c=0; c<channels; c++) {    
            for (int s=_patternLength-1; s>=0; s--) {
                moveSample(c, s, c, s*2);
            }
        }
        _patternLength = _patternLength * 2;
        _patternResolution = _patternResolution * 2;
    } else {
        // shifting down
        for (int c=0; c<channels; c++) {    
            for (int s=0; s<_patternLength; s=s+2) {
                moveSample(c, s, c, s/2);
            }
        }
        _patternLength = _patternLength / 2;
        _patternResolution = _patternResolution / 2;
    }

    return true;
}

void Pattern::moveSample(byte fromChannel, uint16_t fromPosition, byte toChannel, uint16_t toPosition) {
    // moving to itself is not possible!
    if (fromChannel == toChannel && fromPosition==toPosition) return;

    // copy data
    _channelData[toChannel][toPosition].sampleNumber = _channelData[fromChannel][fromPosition].sampleNumber;
    _channelData[toChannel][toPosition].stereoPosition = _channelData[fromChannel][fromPosition].stereoPosition;
    _channelData[toChannel][toPosition].velocity = _channelData[fromChannel][fromPosition].velocity;
    _channelData[toChannel][toPosition].pixelWidth = _channelData[fromChannel][fromPosition].pixelWidth;
    _channelData[toChannel][toPosition].probability = _channelData[fromChannel][fromPosition].probability;
    
    // reset from to standard values
    _channelData[fromChannel][fromPosition].sampleNumber = 255;
    _channelData[fromChannel][fromPosition].stereoPosition = 64;
    _channelData[fromChannel][fromPosition].velocity = 64;
    _channelData[fromChannel][fromPosition].pixelWidth = 15;
    _channelData[fromChannel][fromPosition].probability = 100;
}


void Pattern::setSampleAt(byte channel, uint16_t position, byte sampleNumber, byte stereoPosition, byte velocity)
{    
    samplesUsed[sampleNumber - 1] = true;   

    _channelData[channel][position].sampleNumber = sampleNumber;
//    sprintf(_channelData[column - 1][row].displayText, "S%02d", sampleNumber);
    _channelData[channel][position].stereoPosition = stereoPosition;
    _channelData[channel][position].velocity = velocity;
};

void Pattern::unsetSampleAt(byte channel, uint16_t position) {
    // if no sample is set, set the stop sample flag (SampleNumber 254)
    if (_channelData[channel][position].sampleNumber == 255) {
        _channelData[channel][position].sampleNumber = 254;
//        sprintf(_channelData[column - 1][row].displayText, " ~");
        _channelData[channel][position].stereoPosition = 64;
        _channelData[channel][position].velocity = 64;
    } else {
        // just remove the sample
        _channelData[channel][position].sampleNumber = 255;
//        sprintf(_channelData[column - 1][row].displayText, " ");
        _channelData[channel][position].stereoPosition = 64;
        _channelData[channel][position].velocity = 64;
    } 
};


Pattern::sampleStruct Pattern::getSampleAt(byte channel, uint16_t position)
{
    return _channelData[channel][position];
};

boolean * Pattern::getSamplesUsed()
{
    for (byte i = 0; i < 72; i++) {
        samplesUsed[i] = false;
    }
    
    for (byte c = 0; c < channels; c++)
    {
        for (byte i = 0; i < 128; i++)
        {
            if (_channelData[c][i].sampleNumber != 255) {
                samplesUsed[_channelData[c][i].sampleNumber-1] = true;
            }
        }
    }

    return samplesUsed;
};


void Pattern::increaseVelocity(byte channel, uint16_t position) {
    if (_channelData[channel][position].velocity < 126) {
        _channelData[channel][position].velocity += 2;
    }
};

void Pattern::decreaseVelocity(byte channel, uint16_t position) {
    if (_channelData[channel][position].velocity > 1) {
        _channelData[channel][position].velocity -= 2;
    }
};        

void Pattern::stereoPositionTickLeft(byte channel, uint16_t position) {
    if (_channelData[channel][position].stereoPosition > 1) {
        _channelData[channel][position].stereoPosition -= 2;
    }
};

void Pattern::stereoPositionTickRight(byte channel, uint16_t position) {
    if (_channelData[channel][position].stereoPosition < 126) {
        _channelData[channel][position].stereoPosition += 2;
    }
};

void Pattern::increaseProbability(byte channel, uint16_t position) {
    if (_channelData[channel][position].probability < 100) {
        _channelData[channel][position].probability++;
    }
};

void Pattern::decreaseProbability(byte channel, uint16_t position) {
    if (_channelData[channel][position].probability > 0) {
        _channelData[channel][position].probability--;
    }
};        



void Pattern::_clearPattern()
{
/*    for (byte c = 0; c < channels; c++)
    {
        for (int i = 0; i < 128; i++)
        {
            _channel[c][i].sampleNumber = 255;
            sprintf(_channel[c][i].displayText, " ");
            _channel[c][i].stereoPosition = 64;
            _channel[c][i].velocity = 64;
        }
    }
*/
};
