#ifndef FSIO_h
#define FSIO_h

#include <Arduino.h>
#include <SD.h>
#include <string.h>

class FSIO {

    public:
        typedef struct {
            char sampleFilename[13] = "            ";
        } LibrarySample;

        FSIO();
        void listAllFiles();
        void listDirectory(File dir, int numTabs);
        
        void readLibrarySamplesFromSD(LibrarySample *librarySamples);

        byte createSong(String filename);
        int getSongCount();
        int getSamplesCount(); // counts files in /SAMPLES directory -> global sample library
        boolean getSongName(int number, char *lineBuffer);
        boolean getSampleName(int number, char *lineBuffer);
        String getSampleName(int number);

        void setSelectedSamplePathFromSD(int number);
        void setSelectedSamplePathFromSD(char *sampleName);
        char * getSelectedSamplePathFromSD();

    private:
        char _selectedSamplePathFromSD[40];
        
        // ToDo: work with subdirectories
        // all samples from the samplelibrary on the SD card at /SAMPLES
        LibrarySample *_librarySamples;
        uint16_t _librarySamplesCount = 0;
        
};

#endif
