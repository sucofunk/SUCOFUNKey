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
   
#ifndef SongStructure_h
#define SongStructure_h

#include <Arduino.h>
#include <SD.h>

class SongStructure {
    public:
        SongStructure();

        enum shiftAction {
            UP = 1,
            RIGHT = 2,
            DOWN = 3,
            LEFT = 4
        };

        enum pointerType {
            UNDEFINED = 0,
            SAMPLE = 1,
            PARAMETER_CHANGE_SAMPLE = 2,
            MIDINOTE = 3,
            NOTE_OFF = 4
        };

        typedef struct  {
            byte sampleNumber = 255; // sample, 255 == nothing, 254 == stop sample playback, 253 = change velocity/panning
            //char displayText[4] = "---";
            byte stereoPosition = 64; // 0 = 100% left | 64 = center | 127 = 100% right 
            byte velocity = 64;      // 0..127 -> standard: 64, as defined in midi standard for keyboards without velocity            
            byte baseMidiNote = 60;
            byte pitchedNote = 60;
            byte probability = 100; // probability of sample to play 0..100
        } sampleStruct;

        typedef struct  {
            byte stereoPosition = 64; // 0 = 100% left | 64 = center | 127 = 100% right 
            byte velocity = 64;      // 0..127 -> standard: 64, as defined in midi standard for keyboards without velocity
        } parameterChangeSampleStruct;

        typedef struct  {
            // ToDo: struct for midi notes
        } midiNoteStruct;


        typedef struct {
            uint8_t columnOffset = 0;
            uint16_t successor = 0;
            pointerType type = UNDEFINED;
            uint16_t typeIndex; // index pointer to the corresponding type
        } samplePointerStruct;

        boolean isSomethingAt(uint8_t channel, uint16_t position);

        uint16_t getPositionPointerIndex(uint8_t channel, uint16_t position);
        samplePointerStruct getPosition(uint8_t channel, uint16_t position);
        
        sampleStruct getSampleFromBucketId(uint16_t sampleBucketIndex);
        parameterChangeSampleStruct getParameterChangeFromBucketId(uint16_t parameterChangeBucketIndex);

        boolean setSample(uint8_t channel, uint16_t position, sampleStruct sample);
        boolean setParameterChange(uint8_t channel, uint16_t position, parameterChangeSampleStruct parameterChange);
        
        uint16_t setNewPointerStructureAt(uint8_t channel, uint16_t position);
        boolean setNoteOff(uint8_t channel, uint16_t position);
        void removePosition(uint8_t channel, uint16_t position);
        
        void shiftPosition(uint8_t channel, uint16_t position, shiftAction action); // not implemented - useless?

        boolean copyPosition(uint8_t fromChannel, uint16_t fromPosition, uint8_t toChannel, uint16_t toPosition, boolean deleteAfterOperation);
        boolean movePosition(uint8_t fromChannel, uint16_t fromPosition, uint8_t toChannel, uint16_t toPosition);

        void setCurrentPosition(uint8_t channel, uint16_t position, boolean initial);

        void increaseVelocity(uint8_t channel, uint16_t position);
        void decreaseVelocity(uint8_t channel, uint16_t position);
        void setVelocity(uint8_t channel, uint16_t position, byte velocity);

        void stereoPositionTickLeft(uint8_t channel, uint16_t position);
        void stereoPositionTickRight(uint8_t channel, uint16_t position);
        void setStereoPosition(uint8_t channel, uint16_t position, byte stereoPosition); // 0 = left .. 64 = center .. 127=right

        void increasePitchByOne(uint8_t channel, uint16_t position);
        void decreasePitchByOne(uint8_t channel, uint16_t position);
        void setPitchByMidiNote(uint8_t channel, uint16_t position, byte note);

        void setBaseMidiNote(uint8_t channel, uint16_t position, byte note);

        void increaseProbability(uint8_t channel, uint16_t position);
        void decreaseProbability(uint8_t channel, uint16_t position);

        void setSongLength(uint16_t songLength);
        uint16_t getSongLength();
        void changeSongLengthByTick(boolean increase, byte tickAmount); // true = increase pattern lenght by tickAmount (depending on zoomlevel); false = decrease by ..

        uint16_t getMaxSongLength();

        void setSongResolution(byte res);
        byte getSongResolution();
        void changeSongResolutionByTick(boolean increase);


        boolean *getSamplesUsed();
        boolean samplesUsed[72] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

        void setPlayBackSpeed(float bpm);
        float getPlayBackSpeed();

        boolean loadFromSD(char *songPath);
        boolean saveToSD(char *songPath);

        void test();
        void testLog();
        void testLogSample(uint16_t channel, uint16_t position);
        void debugInfos();

    private:
        uint8_t BLOCKSIZE = 128;

        enum closestPointerType {
            NONE = 0,
            PREDECESSOR = 1,
            TAKEN = 2,
            SUCCESSOR = 3
        };

        typedef struct  {
            uint16_t position;
            uint8_t channel;
            uint16_t samplePointerIndex;
        } currentPositionPointerStruct;

        typedef struct  {
            uint16_t startPointer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            uint16_t startColumn;
            uint16_t endColumn;
        } blockStruct;

        typedef struct  {
            // TopDo: snippetName and id
            uint16_t startColumn;
            uint16_t endColumn;
            uint8_t startChannel;
            uint8_t endChannel; 
        } snippetStruct;

        typedef struct  {
            uint8_t snippetIndex;   // reference to the index of _snippets
            boolean isOriginal;     // original == true, reference to a snippet (non editable) == false
        } snippetReferenceStartpoint;


        typedef struct  {
            uint16_t samplePointerIndex;
            closestPointerType type;
        } closestPointer;

        // list of up to 72 snippets.. each snippet corresponds to a note key
        snippetStruct _snippets[72];
        blockStruct   _blocks[300];


        // this struct holds all the meta information needed to restore an old song status, the same way it was saved.. we just save to struct to file ;)
        typedef struct {
            // ToDo: use a stack or queue for reusablePointers to handle multiple delete operations in a row and not lose memory space
            uint16_t nextSamplePointerIndex = 0;
            uint16_t reusableSamplePointerIndex = 0;
            
            uint16_t nextSampleBucketIndex = 1;
            uint16_t reuseableSampleBucketIndex = 0;

            uint16_t nextParameterChangeBucketIndex = 1;
            uint16_t reuseableParameterChangeBucketIndex = 0;

            uint16_t songLength = 64; // 1, 2, 3, 4 .. * 16 steps = 64 steps = 1 time
            byte     songResolution = 4; // resulution at Zoomlevel NORMAL
            
            float    playbackSpeed = 80.0; // BPM
        } MetaInfos;

        MetaInfos _meta;

        uint16_t _getNextSamplePointerIndex();
        uint16_t _getNextSampleBucketIndex();
        uint16_t _getNextParameterChangeBucketIndex();

        samplePointerStruct _samplePointers[10000];
        sampleStruct _sampleBucket[5000];
        parameterChangeSampleStruct _parameterChangeSampleBucket[5000];

        uint16_t _maxSongLength = sizeof(_blocks)/sizeof(blockStruct) * BLOCKSIZE; // amount of buckets * blocksize

        currentPositionPointerStruct _currentPosition;

        closestPointer _getClosestPointerIndex(uint8_t channel, uint16_t position);
        uint16_t _getBlockIndex(uint16_t position);
        uint16_t _getBlockOffset(uint16_t position);

        void _clearPositionBucket(uint16_t samplePointerIndex);
};

#endif