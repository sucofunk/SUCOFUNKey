#ifndef FSIO_h
#define FSIO_h

#include <Arduino.h>
#include <SD.h>
#include <string.h>

class FSIO {

    public:
        FSIO();    
        void listAllFiles();
        void listDirectory(File dir, int numTabs);
        
        byte createSong(String filename);
        int getSongCount();
        int getSamplesCount(); // counts files in /SAMPLES directory -> global sample library
        boolean getSongName(int number, char *lineBuffer);
        boolean getSampleName(int number, char *lineBuffer);

        void setSelectedSamplePathFromSD(int number);
        void setSelectedSamplePathFromSD(char *sampleName);
        char * getSelectedSamplePathFromSD();

    private:
        char _selectedSamplePathFromSD[40];
};

#endif
