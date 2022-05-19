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
   
#include <SD.h>
#include "FSIO.h"
#include <string.h>

FSIO::FSIO() {
}

void FSIO::listAllFiles() {
  Serial.println("Files on SD Card:");
  File root;
  root = SD.open("/");
  Serial.println("------------------------------------");
  listDirectory(root, 0);
  Serial.println("------------------------------------");
  root.close();
}


void FSIO::listDirectory(File dir, int numTabs) {   
   while(true) {
     
     File entry = dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     
     if (entry.isDirectory()) {
       Serial.print(entry.name());
       Serial.println("/");
       listDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print(entry.name());
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}


void FSIO::readLibrarySamplesFromSD(LibrarySample *librarySamples) {
    Serial.println("reading sample library");
    _librarySamples = librarySamples;

        int sc = 0;

        if (!SD.exists("/SAMPLES/")) return;

        File sampleDir;
        sampleDir = SD.open("/SAMPLES/");
        
        String tempString;

        if (sampleDir.isDirectory()) {
            File nextFile = sampleDir.openNextFile();
            
            // todo: implement directories for browsing, too

            while (nextFile) {
                if (!nextFile.isDirectory() && nextFile.name()[0] != '.' ) {
                    tempString = nextFile.name();                        
                    tempString.toCharArray(_librarySamples[sc].sampleFilename, tempString.length()+1);
                    sc++;                    
                }
                nextFile = sampleDir.openNextFile();
            }
        }

        _librarySamplesCount = sc;

        sampleDir.close();
}



int FSIO::getSongCount() {
    int sc = 0;

    if (!SD.exists("/SONGS/")) return -1;

    File songDir;
    songDir = SD.open("/SONGS/");

    if (songDir.isDirectory()) {
        File nextFile = songDir.openNextFile();
        
        while (nextFile) {
            if (nextFile.isDirectory()) {
                sc++;
                //Serial.println(nextFile.name());
            }
            nextFile = songDir.openNextFile();
        }
    }

    songDir.close();
    return sc;
}


int FSIO::getSamplesCount() {
    return _librarySamplesCount;;
}


// returnvalues:
// 0 = should never happen
// 1 = ok
// 2 = directory already existing
// 3 = directory already existing after shortening
// 4 = no name provided

byte FSIO::createSong(String filename) {
    Serial.print("Songname::");
    Serial.println(filename);

    // check if a general sample library exists. if not, create a /SAMPLES directory on the SD card.
    // it is not the optimal position to do this, but there is no initial first start of the device to create the file structure on the sd card
    // if the first song is created, this directory will already exist.
    if (!SD.exists("/SAMPLES")) {
      SD.mkdir("/SAMPLES");
    }

    boolean shortened = false;

    if (filename.length() < 1) {
        return 4;
    }

    if (filename.length() > 8) {
        filename = filename.substring(0,8);
        shortened = true;
    }

    String basePath = "/SONGS/";
    basePath.append(filename);
    
    int s = basePath.length();
    char buffer[30];
    basePath.toCharArray(buffer, s+1);
    
    // check if directory already exists and return error code
    if (SD.exists(buffer)) {
      return (shortened ? 3 : 2);
    }

    String samplesPath = basePath;
    String patternPath = basePath;

    if (SD.mkdir(buffer)) {
        // create samples directory
        samplesPath.append("/samples");
        s = samplesPath.length();
        samplesPath.toCharArray(buffer, s+1);
        SD.mkdir(buffer);

        // create pattern directory
        patternPath.append("/pattern");
        s = patternPath.length();
        patternPath.toCharArray(buffer, s+1);
        SD.mkdir(buffer);
        return 1;
    };

    return 0;
}


boolean FSIO::getSongName(int number, char *lineBuffer) {
    int sc = 0;
    if (!SD.exists("/SONGS/")) return false;

    File songDir;
    songDir = SD.open("/SONGS/");

    if (songDir.isDirectory()) {
        File nextFile = songDir.openNextFile();
        
        while (nextFile) {
            if (nextFile.isDirectory()) {
                if (sc == number) {                    
                    strcpy(lineBuffer, nextFile.name());
                    songDir.close();
                    return true;
                    }
                sc++;
            }
            nextFile = songDir.openNextFile();
        }
    }
    songDir.close();
    return false;
}

boolean FSIO::getSampleName(int number, char *lineBuffer) {
    if (number < _librarySamplesCount) {
        String t = _librarySamples[number].sampleFilename;
        t.toCharArray(lineBuffer, t.length()+1);
        return true;
    }

    return false;
}

String FSIO::getSampleName(int number) {
    String t = _librarySamples[number].sampleFilename;
    return t;
}


void FSIO::setSelectedSamplePathFromSD(int number) {
    // get sample name and add samplelibrary path to _selectedSamplePathFromSD
    String basePath = "/SAMPLES/";
    char filename[40];
    getSampleName(number, filename);
    basePath.append(filename);    
    basePath.toCharArray(_selectedSamplePathFromSD, basePath.length()+1);  
};

void FSIO::setSelectedSamplePathFromSD(char *sampleName) {
    String basePath = "/SAMPLES/";
    basePath.append(sampleName);    
    basePath.toCharArray(_selectedSamplePathFromSD, basePath.length()+1);
};


char* FSIO::getSelectedSamplePathFromSD() {
  return _selectedSamplePathFromSD;
};
