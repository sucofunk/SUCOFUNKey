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
        boolean getSongName(int number, char *lineBuffer);

    //private:

};

#endif
