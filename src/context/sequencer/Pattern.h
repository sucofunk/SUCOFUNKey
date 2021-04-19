#ifndef Pattern_h
#define Pattern_h

#include <Arduino.h>
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"

class Pattern {
    public:
        Pattern(uint16_t patternLength, SampleFSIO *sfsio, AudioResources *audioResources);

        byte channels = 4;

        typedef struct sampleStruct {
            byte sampleNumber = 255; // sample, 255 == nothing, 254 == stop sample playback
            char displayText[4] = "---";
            byte stereoPosition = 64; // 0 = 100% left | 64 = center | 127 = 100% right 
            byte velocity = 64;      // 0..127 -> standard: 64, as defined in midi standard for keyboards without velocity
        } Sample;

        void setPatternLength(uint16_t patternLength);
        void setPatternSpeed(uint16_t patternSpeed);
        uint16_t getPatternLength();
        uint16_t getPatternSpeed();
        void setSampleAt(byte column, uint16_t row, byte sampleNumber, byte stereoPosition, byte velocity);
        void unsetSampleAt(byte column, uint16_t row);
        sampleStruct getSampleAt(byte column, uint16_t row);

        void increaseVelocity(byte column, byte row);
        void decreaseVelocity(byte column, byte row);

        void stereoPositionTickLeft(byte column, byte row);
        void stereoPositionTickRight(byte column, byte row);

        boolean *getSamplesUsed();
        boolean samplesUsed[72] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
                

    private:
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;

        uint8_t _patternLength = 128;
        uint8_t _patternSpeed = 80; // BPM
        
        sampleStruct _channel[4][128];

        void _clearPattern();
};

#endif