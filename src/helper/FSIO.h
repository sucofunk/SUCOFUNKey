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
