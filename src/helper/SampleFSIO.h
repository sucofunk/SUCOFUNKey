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

        void writeAllSamplesToWaveformBuffer();

        boolean copyFile(const char *f1, const char *f2);
        boolean copyFilePart(const char *f1, const char *f2, long byteStart, long byteEnd);
        boolean copyFilePart(const char *f1, const char *f2, long byteStart, long byteEnd, float volumeScaleFactor);        
        
        long copyRawFromSdToMemory(const char *filename, long startOffset);
        boolean addSampleToMemory(byte sampleNumber, boolean forceReload);
        boolean loadSamplesToMemory(boolean *sampleArray);

        long getExtmemOffset(byte sampleNumber);
        unsigned int *getExtmemAddress(byte sampleNumber);
        unsigned int *getExtmemAddressData(byte sampleNumber);

        void readSampleBankStatusFromSD();
        
        void deleteFile(const char *filename);

        long getByteCountFromMs(long ms);        
        byte getExtmemUsagePercent();

        void generateInstrument(byte sampleNumber, int baseNote);
        AudioSynthWavetable::instrument_data getInstrumentDataBySample(byte sampleNumber);

        boolean sampleBanksStatus[3][24] = {{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                            {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                            {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}};
        
        char sampleFilename[3][24][40];
        char recorderFilename[40]; // waveformbuffer is saved at position 72 (--> bank0=3, sample0=0)

        void setSongPath(char *songPath);
        char * getSongPath();

/*        struct sample_data {
            // SAMPLE VALUES
            int16_t* sample;
            bool LOOP;
            int INDEX_BITS;
            float PER_HERTZ_PHASE_INCREMENT;
            uint32_t MAX_PHASE;
            uint32_t LOOP_PHASE_END;
            uint32_t LOOP_PHASE_LENGTH;
            uint16_t INITIAL_ATTENUATION_SCALAR;
            
            // VOLUME ENVELOPE VALUES
            uint32_t DELAY_COUNT;
            uint32_t ATTACK_COUNT;
            uint32_t HOLD_COUNT;
            uint32_t DECAY_COUNT;
            uint32_t RELEASE_COUNT;
            int32_t SUSTAIN_MULT;

            // VIRBRATO VALUES
            uint32_t VIBRATO_DELAY;
            uint32_t VIBRATO_INCREMENT;
            float VIBRATO_PITCH_COEFFICIENT_INITIAL;
            float VIBRATO_PITCH_COEFFICIENT_SECOND;

            // MODULATION VALUES
            uint32_t MODULATION_DELAY;
            uint32_t MODULATION_INCREMENT;
            float MODULATION_PITCH_COEFFICIENT_INITIAL;
            float MODULATION_PITCH_COEFFICIENT_SECOND;
            int32_t MODULATION_AMPLITUDE_INITIAL_GAIN;
            int32_t MODULATION_AMPLITUDE_SECOND_GAIN;
        };

        struct instrument_data {
            uint8_t sample_count;
            uint8_t* sample_note_ranges;
            sample_data* samples;
        };

        sample_data sampleData[1];
        uint8_t ranges[1] = {127};
        instrument_data instrument{1, ranges, sampleData};
*/        
        void generateWaveFormBufferForSample(byte bank0, byte sampleId0);
        void clearWaveFormBufferById(byte sampleId72);
        boolean isRecodingAvailable();

        byte waveFormBuffer[73][320][2];
        long waveFormBufferLength[73];
        long pixelToWaveformSamples[73];
        long sampleLengthMS[73];

    private:
        unsigned int *_extmemArray;
        long _extmemSize;
        Screen *_screen;

        char *_songPath;

        long _nextOffset = 0;
        long _sampleOffsets[72] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

        uint8_t _completeRange[1] = {127};
        AudioSynthWavetable::sample_data _sampleData[72][1]{};
        AudioSynthWavetable::instrument_data _instrumentData[72]{};
};

#endif
