#ifndef Pattern_h
#define Pattern_h

#include <Arduino.h>
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"

class Pattern {
    public:
        Pattern(uint16_t patternLength, SampleFSIO *sfsio, AudioResources *audioResources);

        static const uint8_t channels = 8;

        typedef struct sampleStruct {
            byte sampleNumber = 255; // sample, 255 == nothing, 254 == stop sample playback, 253 = change velocity/panning
            //char displayText[4] = "---";
            byte stereoPosition = 64; // 0 = 100% left | 64 = center | 127 = 100% right 
            byte velocity = 64;      // 0..127 -> standard: 64, as defined in midi standard for keyboards without velocity
            
            byte baseMidiNote = 60;
            byte pitchedNote = 60;

            byte pixelWidth = 15; // calculate display width of sample length and zoom level
            byte probability = 100; // probability of sample to play 0..100
        } Sample;

        void setPatternLength(uint16_t patternLength);
        void setPatternSpeed(float patternSpeed);
        uint16_t getPatternLength();
        float getPatternSpeed();

        void setPatternResolution(byte res);
        byte getPatternResolution();
        void changePatternResolutionByTick(boolean increase);

        boolean shiftResolution(boolean up); // false == down; returns if change happened to redraw grid
        void moveSample(byte fromChannel, uint16_t fromPosition, byte toChannel, uint16_t toPosition);

        void changePatternLengthByTick(boolean increase); // true = increase pattern lenght by 1; false = decrease by 1

        void setSampleAt(byte channel, uint16_t position, byte sampleNumber, byte stereoPosition, byte velocity);
        void unsetSampleAt(byte channel, uint16_t position);
        sampleStruct getSampleAt(byte channel, uint16_t position);

        void increaseVelocity(byte channel, uint16_t position);
        void decreaseVelocity(byte channel, uint16_t position);
        void setVelocity(byte channel, uint16_t position, byte velocity);

        void stereoPositionTickLeft(byte channel, uint16_t position);
        void stereoPositionTickRight(byte channel, uint16_t position);
        void setStereoPosition(byte channel, uint16_t position, byte stereoPosition); // 0 = left .. 64 = center .. 127=right

        void increasePitchByOne(byte channel, uint16_t position);
        void decreasePitchByOne(byte channel, uint16_t position);
        void setPitchByMidiNote(byte channel, uint16_t position, byte note);

        void setBaseMidiNote(byte channel, uint16_t position, byte note);

        void increaseProbability(byte channel, uint16_t position);
        void decreaseProbability(byte channel, uint16_t position);

        boolean *getSamplesUsed();
        boolean samplesUsed[72] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
                

    private:
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;

        uint16_t _patternLength = 16;
        byte     _patternResolution = 4;
        uint16_t _maxPatternLength = 256;
        float    _patternSpeed = 80.0; // BPM

        sampleStruct _channelData[channels][256];

        void _clearPattern();
};

#endif