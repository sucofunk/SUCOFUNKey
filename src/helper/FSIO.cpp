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


// returnvalues:
// 0 = should never appen
// 1 = ok
// 2 = directory already existing
// 3 = directory already existing after shortening
// 4 = no name provided

byte FSIO::createSong(String filename) {
    Serial.print("Songname::");
    Serial.println(filename);

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
