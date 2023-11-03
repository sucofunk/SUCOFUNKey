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

   ---------------------------------------------------------------------------------------------- */
   
#ifndef SampleFSIO_h
#define SampleFSIO_h

#include <Arduino.h>
#include <SPI.H>
#include <SD.h>
#include <Audio.h>
#include "../gui/Screen.h"

class SampleFSIO {

    public:
        SampleFSIO(unsigned int *extmemArray, long extmemSize, Screen *screen);    


        typedef struct  {
            char name[40]; // display name
            uint32_t envelopeA = 0;
            uint32_t envelopeD = 0;
            uint32_t envelopeS = 0;
            uint32_t envelopeR = 0;
            uint32_t loopStart = 0;
            uint32_t loopEnd = 0;
            byte baseMidiNote = 60;
            boolean loop = false;
        } sampleInfosStruct;
        

        void writeAllSamplesToWaveformBuffer();

        boolean copyFile(const char *f1, const char *f2);
        boolean copyFilePart(const char *f1, const char *f2, long byteStart, long byteEnd);
        boolean copyFilePart(const char *f1, const char *f2, long byteStart, long byteEnd, float volumeScaleFactor);        
        
        long copyRawFromSdToMemory(const char *filename, long startOffset);
        boolean addSampleToMemory(byte bank1, byte sampleId1, boolean forceReload);
        boolean addSampleToMemory(byte sampleId);
        void removeSampleFromMemory(byte sampleId);
        boolean loadSamplesToMemory(boolean *sampleArray);

        long getExtmemOffset(byte sampleNumber);
        unsigned int *getExtmemAddress(byte sampleNumber);
        unsigned int *getExtmemAddressData(byte sampleNumber);

        void readSampleBankStatusFromSD();
        
        void deleteFile(const char *filename);

        long getByteCountFromMs(long ms);        
        byte getExtmemUsagePercent();

        void generateInstrument(byte sampleNumber, int baseNote);
        void changeInstrumentParameters(byte sampleNumber, boolean loop, uint8_t delay_count, uint8_t attack_count, uint8_t hold_count, uint8_t decay_count, uint8_t release_count, uint8_t sustain_mult, uint8_t vibrato_delay, uint8_t vibrato_increment);

        boolean sampleBanksStatus[3][24] = {{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                            {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                            {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}};
        
        // ToDo: save RAM and delete these two arrays.. will become obsolete with _sampleInfos, but needs to be refactored!
        char sampleFilename[3][24][40];
        //char sampleDisplayTitle[3][24][40];
        char recorderFilename[40]; // waveformbuffer is saved at position 72 (--> bank0=3, sample0=0)

        void setSongPath(char *songPath);
        char * getSongPath();
   
        void generateWaveFormBufferForSample(byte bank0, byte sampleId0);
        void clearWaveFormBufferById(byte sampleId72);
        boolean isRecodingAvailable();

        byte waveFormBuffer[73][320][2]; // ToDo: move to DMAMEM to save primary RAM
        long waveFormBufferLength[73];
        long pixelToWaveformSamples[73];
        long sampleLengthMS[73];
        
        boolean sampleAvailable(byte sampleId); // 0..71
        boolean sampleInMemory(byte sampleId); // 0..71
        
        void clearSampleMemory();

        // -------- SampleInfos --------
        boolean loadSampleInfosFromSD();
        boolean saveSampleInfosToSD();
        void setSampleInfosName(int sampleId1, String name);
        char * getSampleInfosName(int sampleId1);
        void getSampleInfosName(int sampleId1, int maxLength, char* returnArray);
        void resetSampleInfos(int sampleId1);
        // -----------------------------

        void debugInfos();

    private:
        unsigned int *_extmemArray;
        long _extmemSize;
        Screen *_screen;

        char *_songPath;

        long _nextOffset = 0;
        long _sampleOffsets[72] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

        sampleInfosStruct _sampleInfos[72];
};

#endif
