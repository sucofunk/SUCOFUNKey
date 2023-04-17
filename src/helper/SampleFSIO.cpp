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

#include "SampleFSIO.h"

SampleFSIO::SampleFSIO(unsigned int *extmemArray, long extmemSize, Screen *screen) {
  _extmemArray = extmemArray;
  _extmemSize = extmemSize;
  _screen = screen;

  // initialize empty fileName array
  for (int b = 0; b < 3; b++) {
    for (int s = 0; s < 24; s++) {
      for (int f = 0; f < 40; f++) {
        sampleFilename[b][s][f] = ' ';
      }      
    }
  }

  // initialize waveFormBuffer for all 72 possible samples -> ToDo: find a more progressive data structure?
  // waveFormBuffer for recorder is stored at position 72
  for (int s=0; s<73; s++) {
    pixelToWaveformSamples[s] = -1;
    waveFormBufferLength[s] = 0;
    sampleLengthMS[s] = 0;
    for (int i=0; i<320; i++) {
      waveFormBuffer[s][i][0] = 0;
      waveFormBuffer[s][i][1] = 0;
    }
  }
}

// initializsation steps to fill project sample information arrays
void SampleFSIO::setSongPath(char *songPath) {
  _songPath = songPath;

  char buff[31];
  char buffFilename[13];

  // generate correct paths to samples
  for (int b = 0; b < 3; b++) {
    for (int s = 0; s < 24; s++) {    
        strcpy(buff, _songPath);
        sprintf(buffFilename, "/SAMPLES/%d.RAW", s+1+(b*24));
        strcat(buff, buffFilename);
        strcpy(sampleFilename[b][s], buff);
    }
  }

  // generate correct path where the recorder will save a new record
  strcpy(buff, _songPath);
  strcat(buff, "/SAMPLES/RECORD.RAW");
  strcpy(recorderFilename, buff);
}

char * SampleFSIO::getSongPath() {
  return _songPath;
}

boolean SampleFSIO::isRecodingAvailable() {
  return SD.exists(recorderFilename);
}

void SampleFSIO::writeAllSamplesToWaveformBuffer() {
   readSampleBankStatusFromSD();
   float loadingBar = 0.0;

   for (int b=0; b<3; b++) {
    for (int s=0; s<24; s++) {
          loadingBar += 0.01;
          _screen->loadingScreen(loadingBar);

        if(sampleBanksStatus[b][s]) {
          generateWaveFormBufferForSample(b,s);
        };
    }
  }

  // generate waveform buffer for last recording (RECORD.RAW, saved at position 72 -> bank0 3, sample0 0)
  if (isRecodingAvailable()) {
    generateWaveFormBufferForSample(3, 0);
  }

};

void SampleFSIO::clearWaveFormBufferById(byte sampleId72) {
    for (int i=0; i<320; i++) {
      waveFormBuffer[sampleId72][i][0] = 0;
      waveFormBuffer[sampleId72][i][1] = 0;
    }  
}

void SampleFSIO::generateWaveFormBufferForSample(byte bank0, byte sampleId0) {
    int heightDivisor = floor(32767/60); // assumes the display area is 120px (2*60px) high
    File sample;
    
    byte bufferPosition = (24*bank0)+sampleId0;

    // clear waveformBuffer for Sample
    for (int i=0; i<320; i++) {
      waveFormBuffer[bufferPosition][i][0] = 0;
      waveFormBuffer[bufferPosition][i][1] = 0;
    }

    char * filename = (bufferPosition == 72 ? recorderFilename : sampleFilename[bank0][sampleId0]);

    if (SD.exists(filename)) {
      sample = SD.open(filename);
      if (!sample) {
        Serial.println("unable to open sample file");
        return;
      }      
    } else { 
      Serial.println("Sample does not exist!");
      return;
    }

    long fsize = sample.size();
    long samples = fsize/2;
    int16_t peak;

    int tempMin = 0;
    int tempMax = 0;

    long pxIterations = samples/320;

    // defines how many samples will be aggregated to one pixel - used for cropping markers 
    // to get the right positions to cut from the original RAW file
    pixelToWaveformSamples[bufferPosition] = (pxIterations == 0 ? 1 : pxIterations);
    waveFormBufferLength[bufferPosition] = 320;
    sampleLengthMS[bufferPosition] = floor(samples/44.1);

    if (pxIterations > 1) {
      for (int pi=0; pi<320; pi++) {
        tempMin = 0;
        tempMax = 0;
        for (long i=0; i<pxIterations; i++) {
          if (sample.read(&peak, sizeof(peak)) != 0) {
            if (peak < tempMin) {
              tempMin = peak;
            }
            if (peak > tempMax) {
              tempMax = peak;  
            }          
          }
        }
          waveFormBuffer[bufferPosition][pi][0] = abs(floor(tempMin/heightDivisor));
          waveFormBuffer[bufferPosition][pi][1] = abs(floor(tempMax/heightDivisor));
      }
    } else {
      // write values straight to waveFormBuffer
      int i = 0;     
      while (sample.available()) {
          sample.read(&peak, sizeof(peak));
          if (peak >= 0) {
            waveFormBuffer[bufferPosition][i][0] = 0;
            waveFormBuffer[bufferPosition][i][1] = abs(floor(peak/heightDivisor));
          } else {
            waveFormBuffer[bufferPosition][i][0] = abs(floor(peak/heightDivisor));
            waveFormBuffer[bufferPosition][i][1] = 0;
          }
          i++;
      }
      waveFormBufferLength[bufferPosition] = i+1;
    }

    sample.close();    
}


boolean SampleFSIO::copyFile(const char *f1, const char *f2) {     
  if (SD.exists(f2)) {
    Serial.println("removing destination file");
    SD.remove(f2);
  }

  File from;
  File to;
  byte buffer[2048];
  int bufferSize = sizeof(buffer);

  from = SD.open(f1);
  to = SD.open(f2, FILE_WRITE);

  if (!from) {
    Serial.println("unable to open source file");
    return false;
  }      

  if (!to) {
    Serial.println("unable to open destination file");
    from.close();
    return false;
  }

  // File smaller than buffer? switch down to 1 byte per r/w operation
  // ToDo: same for from.available < bufferSize????
  if (from.size() < sizeof(buffer)) {
    bufferSize = 1;
  } 

  while (from.available()) {
    from.read(buffer,bufferSize);
    to.write(buffer,bufferSize);
  }    

  from.close();
  to.close();
  return true;
}




boolean SampleFSIO::copyFilePart(const char *f1, const char *f2, long byteStart, long byteEnd, float volumeScaleFactor) {

  // ToDo: after updating a slot, check if the sample is already in sampleram and update it, too
  //       - another option would be forcing a reload in the sampler after using this function -> implement defragmentation!

  if (SD.exists(f2)) {
    Serial.println("removing destination file");
    SD.remove(f2);
  }

  File from;
  File to;

  from = SD.open(f1);
  to = SD.open(f2, FILE_WRITE);

  if (!from) {
    Serial.println("unable to open source file");
    to.close();
    return false;
  }      

  if (!to) {
    Serial.println("unable to open destination file");
    from.close();
    return false;
  }

  long fromSize = from.size();
  
  if (byteEnd == 0) {
      byteEnd = from.size();
  }

  if (byteEnd > fromSize) {
    byteEnd = fromSize;
  }

  if (fromSize <  byteStart || byteStart >= byteEnd) {
      Serial.println("operation not possible as we cannot shift back in time");
      return false;
  }

  from.seek(byteStart);
  
  int16_t intBuff = 0;
  byte byteBuffer[2048];
  char t1;
  char t2;
  long offset = 0;
  int bufferCount = 0;

  while (from.available() && byteStart+offset < byteEnd) {
    from.read(&intBuff, sizeof(intBuff));

    intBuff = intBuff*volumeScaleFactor;

    // convert to little endian by dividing the 16 bit value..
    t1 = intBuff >> 8;    
    t2 = intBuff & 0xff;    
    // .. and setting it in the opposite order
    byteBuffer[bufferCount] = t2;
    byteBuffer[bufferCount+1] = t1;

    if (bufferCount == sizeof(byteBuffer)) {
      to.write(byteBuffer,sizeof(byteBuffer));
      bufferCount = 0;
    } else {
      bufferCount += 2;
    }
    offset += 2;
  }

  if (bufferCount > 0) {
    to.write(byteBuffer, bufferCount);
  }

  from.close();
  to.close();

  return true;
}


// -------------------------------------------------------------------------------
// reads a sample from sd card and copy it to extmem
// returns offset for next sample
// -------------------------------------------------------------------------------
long SampleFSIO::copyRawFromSdToMemory(const char *filename, long startOffset) {
  File f;

  if (SD.exists(filename)) {
    f = SD.open(filename, O_READ);  
  } else {
    Serial.println("File does not exist!");
    return -1;
  }

  // from playMem for uncompressed 44100 hz mono uint
  uint32_t fileType = 0x8100 << 16;

  uint32_t header = 0;
  header = fileType;

  uint32_t fs = f.size() >> 1; // same as divided by 2
  long c = f.size()/4;
  //uint16_t buff[2];
  int16_t buff[2];

  long maxLength = _extmemSize/sizeof(buff);

  if (startOffset+c >= maxLength) {
    Serial.println("Out of memory!");
    return -1;
  }
  
  header = header + fs;
  _extmemArray[startOffset] = header;

  for (long i=1; i<c+1; i++) {
    f.read(buff, 4);
    _extmemArray[startOffset+i] = (buff[0] << 16) + (buff[1]);
  }

  // extend sample to to prevent clipping with the next sample in memory, when playing pitched samples
  // A VERY DIRTY HACK AND WASTES A LOT OF EXTMEM, but otherwise the wavetable syth object needs to be rewritten. Any volunt here? ;)
  for (long i=0; i<10240; i++) {
    _extmemArray[startOffset+c+i] = 0;
  }

  f.close();

  return startOffset+c+2+10240;
}

// sampleNumber according to array 0..71
boolean SampleFSIO::addSampleToMemory(byte bank1, byte sampleId1, boolean forceReload) {
  byte sample72 = (bank1-1)*24+sampleId1-1;
 
  if (_sampleOffsets[sample72] == -1 || forceReload) {
      long offset = 0;

      offset = _nextOffset;

      _nextOffset = copyRawFromSdToMemory(sampleFilename[bank1-1][sampleId1-1], offset);

      if (_nextOffset == -1) {
        _nextOffset = offset;
        _sampleOffsets[sample72] = -1;
        return false;
      } else {
        _sampleOffsets[sample72] = offset;
        return true;
      }
  } 
  return true;
}


// sampleId = 0..71
void SampleFSIO::removeSampleFromMemory(byte sampleId) {

  // sample to remove is not in memory.. nothing to do..
  if (_sampleOffsets[sampleId] == -1) return;


  // find sampleIDoffset
  long sampleOffsetToRemove = _sampleOffsets[sampleId];

  byte nextOffsetId = 72;
  long nextOffset = LONG_MAX;
  // find next sampleIdOffset
  for (int i=0; i<72; i++) {
    if (_sampleOffsets[i] > sampleOffsetToRemove && _sampleOffsets[i] < sampleOffsetToRemove && _sampleOffsets[i] < nextOffset) {
      nextOffsetId = i;
    }
  }

  if (nextOffsetId < 72) {
    // calculate offset and move all remaining bits in extmem down    
    unsigned long offset = nextOffset - sampleOffsetToRemove;

    unsigned long o = nextOffset;
    
    // shift everything in extmem at a higher position down
    while (o < sizeof(_extmemArray)/4) {
      _extmemArray[o] = _extmemArray[o - offset];
      o++;
    }

    // update all other _sampleOffsets which are larger than the offset to remove

    for (int i=0; i<72; i++) {
      if (_sampleOffsets[i] > sampleOffsetToRemove) {
        _sampleOffsets[i] = _sampleOffsets[i] - offset;
      }
    }

    _nextOffset = _nextOffset - offset;

  } else {
    // no other offset? -> sample to remove is the last one in extmem. set _nextOffset to the current position
    _nextOffset = sampleOffsetToRemove;
  }

  _sampleOffsets[sampleId] = -1;
}


boolean SampleFSIO::loadSamplesToMemory(boolean *sampleArray) {
  
  // ToDo: check memory status -> handle extmem overflow!

  _nextOffset = 0; // reset extmem start pointer

  for (int i = 0; i < 72; i++) {
    _sampleOffsets[i] = -1;
  }

  for (byte b=0; b<3; b++) {
    for (byte s=0; s<24; s++) {
      if (sampleArray[b*24+s]) {
        addSampleToMemory(b+1, s+1, true);
      }
    }
  }
  return true;
}

long SampleFSIO::getExtmemOffset(byte sampleNumber) {
    return _sampleOffsets[sampleNumber-1];
}

unsigned int *SampleFSIO::getExtmemAddress(byte sampleNumber) {
    if (_sampleOffsets[sampleNumber-1] != -1) {
      return _extmemArray + _sampleOffsets[sampleNumber-1];
    }

    return 0;
}

unsigned int *SampleFSIO::getExtmemAddressData(byte sampleNumber) {
    if (_sampleOffsets[sampleNumber-1] != -1) {
      return _extmemArray + _sampleOffsets[sampleNumber-1] + 1;
    }

    return 0;
}

byte SampleFSIO::getExtmemUsagePercent() {
  float f = (static_cast<float>(_nextOffset) / static_cast<float>(_extmemSize/4)) * 100.0;
  return round(f);
};


// 0..71 checks if the sample is available on sd card
boolean SampleFSIO::sampleAvailable(byte sampleId) {      
  return sampleBanksStatus[sampleId/24][sampleId%24];
};

 // 0..71 checks if the sample is loaded into extmem
boolean SampleFSIO::sampleInMemory(byte sampleId) {
  return _sampleOffsets[sampleId] == -1 ? false : true;
};



// ------------------------------------------------------------------------------
//  Checks, if Samples, named from 1.WAV to 72.WAV are available on the SD card
//  and updates sampleBanksStatus[3][24].
// ------------------------------------------------------------------------------

void SampleFSIO::readSampleBankStatusFromSD() {
 for (int b=0; b<3; b++) {
    for (int s=0; s<24; s++) {
      if (SD.exists(sampleFilename[b][s])) {
        sampleBanksStatus[b][s] = true;
      } else {
        sampleBanksStatus[b][s] = false;
      }
    }
  }
};

void SampleFSIO::deleteFile(const char *filename) {

    // ToDo: check if it is still playing -> stop playing

    if (SD.exists(filename)) {
        SD.remove(filename);
    }    
}

long SampleFSIO::getByteCountFromMs(long ms) {
  // assumes format is 44.1 khz mono raw audio 16 bit signed int
  // *2 ??? -> int = 16 Bit = 2Byte
  return round(44.1*ms*2);
};


void SampleFSIO::clearSampleMemory() {
  // set the whole sample values to 0 .. silence ;)
  for (unsigned long i=0; i<4194304; i++) {
      _extmemArray[i] = 0;
  }

  // set offset to the beginning
  _nextOffset = 0;

  // clear all sample offsets
  for (int i=0; i<72; i++) {
    _sampleOffsets[i] = -1;
  }
};


void SampleFSIO::debugInfos() {
/*    Serial.println("----------");
    for (int i=0; i<72; i++) {
      Serial.print("Offset ");
      Serial.print(i);
      Serial.print("::");
      Serial.println(_sampleOffsets[i]);
    }
*/
}