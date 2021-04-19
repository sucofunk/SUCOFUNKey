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

void Pattern::setPatternSpeed(uint16_t patternSpeed)
{
    _patternSpeed = patternSpeed;
};

uint16_t Pattern::getPatternLength()
{
    return _patternLength;
};

uint16_t Pattern::getPatternSpeed()
{
    return _patternSpeed;
};

void Pattern::setSampleAt(byte column, uint16_t row, byte sampleNumber, byte stereoPosition, byte velocity)
{    
    samplesUsed[sampleNumber - 1] = true;   

    _channel[column - 1][row].sampleNumber = sampleNumber;
    sprintf(_channel[column - 1][row].displayText, "S%02d", sampleNumber);
    _channel[column - 1][row].stereoPosition = stereoPosition;
    _channel[column - 1][row].velocity = velocity;
};

void Pattern::unsetSampleAt(byte column, uint16_t row) {
    // if no sample is set, set the stop sample flag (SampleNumber 254)
    if (_channel[column - 1][row].sampleNumber == 255) {
        _channel[column - 1][row].sampleNumber = 254;
        sprintf(_channel[column - 1][row].displayText, " ~");
        _channel[column - 1][row].stereoPosition = 64;
        _channel[column - 1][row].velocity = 64;
    } else {
        // just remove the sample
        _channel[column - 1][row].sampleNumber = 255;
        sprintf(_channel[column - 1][row].displayText, " ");
        _channel[column - 1][row].stereoPosition = 64;
        _channel[column - 1][row].velocity = 64;
    } 
};


Pattern::sampleStruct Pattern::getSampleAt(byte column, uint16_t row)
{
    return _channel[column - 1][row];
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
            if (_channel[c][i].sampleNumber != 255) {
                samplesUsed[_channel[c][i].sampleNumber-1] = true;
            }
        }
    }

    return samplesUsed;
};


void Pattern::increaseVelocity(byte column, byte row) {
    if (_channel[column - 1][row].velocity < 126) {
        _channel[column - 1][row].velocity += 2;
    }
};

void Pattern::decreaseVelocity(byte column, byte row) {
    if (_channel[column - 1][row].velocity > 1) {
        _channel[column - 1][row].velocity -= 2;
    }
};        

void Pattern::stereoPositionTickLeft(byte column, byte row) {
    if (_channel[column - 1][row].stereoPosition > 0) {
        _channel[column - 1][row].stereoPosition--;
    }
};

void Pattern::stereoPositionTickRight(byte column, byte row) {
    if (_channel[column - 1][row].stereoPosition < 127) {
        _channel[column - 1][row].stereoPosition++;
    }
};


void Pattern::_clearPattern()
{
    for (byte c = 0; c < channels; c++)
    {
        for (int i = 0; i < 128; i++)
        {
            _channel[c][i].sampleNumber = 255;
            sprintf(_channel[c][i].displayText, " ");
            _channel[c][i].stereoPosition = 64;
            _channel[c][i].velocity = 64;
        }
    }
};
