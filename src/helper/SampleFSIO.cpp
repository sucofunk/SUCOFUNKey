#include "SampleFSIO.h"
#include "audio-extensions/synth_wavetable_suco.h"

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
  byte buffer[256];
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

  Serial.print("File size: ");
  Serial.println(fromSize);

  if (fromSize <  byteStart || byteStart >= byteEnd) {
      Serial.println("operation not possible as we cannot shift back in time");
      return false;
  }

  from.seek(byteStart);
  
  int16_t intBuff = 0;
  byte byteBuffer[256];
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
  
Serial.print("copyRawFromSdToMemory::");
Serial.println(filename);

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

  // extend sample to be a multiple of the audio buffer to prevent clipping with the next sample in memory, when playing pitched samples
/*  uint8_t gap = c % AUDIO_BLOCK_SAMPLES;

  Serial.print("C::");
  Serial.println(c);
  Serial.print("AUDIO_BLOCK_SAMPLES::");
  Serial.println(AUDIO_BLOCK_SAMPLES);

  if (gap > 0) {
    Serial.print("GAP::");
    Serial.println(gap);
    for (int i=0; i<gap; i++) {
      _extmemArray[startOffset+c+i] = 0;
    }
  }
*/

  f.close();

  return startOffset+c+2;
}

// sampleNumber according to array 0..71
boolean SampleFSIO::addSampleToMemory(byte bank1, byte sampleId1, boolean forceReload) {
  byte sample72 = (bank1-1)*24+sampleId1-1;

  if (_sampleOffsets[sample72] == -1 || forceReload) {
      long offset = 0;

      offset = _nextOffset;

      _nextOffset = copyRawFromSdToMemory(sampleFilename[bank1-1][sampleId1-1], offset);
      Serial.println(_nextOffset);

      if (_nextOffset == -1) {
        _nextOffset = offset;
        _sampleOffsets[sample72] = -1;
        return false;
      } else {
        _sampleOffsets[sample72] = offset;

        // generate wavetable
        generateInstrument(sample72+1, 60);

        return true;
      }
  } 
  return true;   
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

// sampleNUmber 1..72
void SampleFSIO::generateInstrument(byte sampleNumber, int baseNote) {
  // retrieve sample size/length from extmem (first 32 bits -> 8 Bits file format 0x81, followed by 24 bits sample size)
  unsigned int header = getExtmemAddress(sampleNumber)[0];
  Serial.println(header, BIN);
  uint32_t s = getExtmemAddress(sampleNumber)[1];
  Serial.println(s, BIN);

  uint32_t header_format = 0x8100 << 16;
  
  //was: uint32_t sample_length = header - header_format; // length in samples (16 bit) --> file size in byte would be double
  uint32_t sample_length = header - header_format;

Serial.print("sampleLength::");
Serial.println(sample_length);

  int16_t *sampleData16 = (int16_t*) getExtmemAddressData(sampleNumber);

  int LENGTH_BITS = 0;
  // was: int LENGTH = sample_length*2;
  int LENGTH = sample_length*2;

  // "calculate" LENGHT_BITS from LENGTH
  // source: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
  //while (LENGTH >>= 1) { LENGTH_BITS++; }
  while (LENGTH >>= 1) { LENGTH_BITS++; }

  int LOOPEND = (LENGTH/2) -1;
  int LOOPSTART = 0;

  _sampleData[sampleNumber-1][0].sample = (int16_t*)sampleData16;
  _sampleData[sampleNumber-1][0].LOOP = false;
  _sampleData[sampleNumber-1][0].INDEX_BITS = LENGTH_BITS;

  _sampleData[sampleNumber-1][0].PER_HERTZ_PHASE_INCREMENT = (1 << (32 - LENGTH_BITS)) * AUDIO_SAMPLE_RATE_EXACT / WAVETABLE_NOTE_TO_FREQUENCY(baseNote) / AUDIO_SAMPLE_RATE_EXACT + 0.5;

  //_sampleData[sampleNumber-1][0].PER_HERTZ_PHASE_INCREMENT = ((0x80000000 >> (LENGTH_BITS-1)) * 1.0 * (44100.0 / AUDIO_SAMPLE_RATE_EXACT)) / WAVETABLE_NOTE_TO_FREQUENCY(baseNote) + 0.5;
  _sampleData[sampleNumber-1][0].MAX_PHASE = ((uint32_t)LENGTH - 1) << (32 - LENGTH_BITS);
  _sampleData[sampleNumber-1][0].LOOP_PHASE_END = ((uint32_t)LOOPEND - 1) << (32 - LENGTH_BITS);
  _sampleData[sampleNumber-1][0].LOOP_PHASE_LENGTH = (((uint32_t)LOOPEND - 1) << (32 - LENGTH_BITS)) - (((uint32_t)LOOPSTART - 1) << (32 - LENGTH_BITS));
  _sampleData[sampleNumber-1][0].INITIAL_ATTENUATION_SCALAR = uint16_t(UINT16_MAX * WAVETABLE_DECIBEL_SHIFT(-0 / 100.0));

  _sampleData[sampleNumber-1][0].DELAY_COUNT = uint32_t(0 * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / 8.0 + 0.5);
  _sampleData[sampleNumber-1][0].ATTACK_COUNT = uint32_t(0 * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / 8.0 + 0.5);
  _sampleData[sampleNumber-1][0].HOLD_COUNT = uint32_t(0 * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / 8.0 + 0.5);
  _sampleData[sampleNumber-1][0].DECAY_COUNT = uint32_t(0 * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / 8.0 + 0.5);
  _sampleData[sampleNumber-1][0].RELEASE_COUNT = uint32_t(0 * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / 8.0 + 0.5);
  _sampleData[sampleNumber-1][0].SUSTAIN_MULT = int32_t(0 * AudioSynthWavetableSUCO::UNITY_GAIN);

  _sampleData[sampleNumber-1][0].VIBRATO_DELAY = uint32_t(0 * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / (2 * AudioSynthWavetableSUCO::LFO_PERIOD));
  _sampleData[sampleNumber-1][0].VIBRATO_INCREMENT = uint32_t(0 / 1000.0 * AudioSynthWavetableSUCO::LFO_PERIOD * (UINT32_MAX / AUDIO_SAMPLE_RATE_EXACT));
  _sampleData[sampleNumber-1][0].VIBRATO_PITCH_COEFFICIENT_INITIAL = (WAVETABLE_CENTS_SHIFT(-0 / 1000.0) - 1.0) * 4;
  _sampleData[sampleNumber-1][0].VIBRATO_PITCH_COEFFICIENT_SECOND = (1.0 - WAVETABLE_CENTS_SHIFT(0 / 1000.0)) * 4;

  _sampleData[sampleNumber-1][0].MODULATION_DELAY = uint32_t(0 * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / (2 * AudioSynthWavetableSUCO::LFO_PERIOD));
  _sampleData[sampleNumber-1][0].MODULATION_INCREMENT = uint32_t(0 / 1000.0 * AudioSynthWavetableSUCO::LFO_PERIOD * (UINT32_MAX / AUDIO_SAMPLE_RATE_EXACT));
  _sampleData[sampleNumber-1][0].MODULATION_PITCH_COEFFICIENT_INITIAL = (WAVETABLE_CENTS_SHIFT(-0 / 1000.0) - 1.0) * 4;
  _sampleData[sampleNumber-1][0].MODULATION_PITCH_COEFFICIENT_SECOND = (1.0 - WAVETABLE_CENTS_SHIFT(0 / 1000.0)) * 4;
  _sampleData[sampleNumber-1][0].MODULATION_AMPLITUDE_INITIAL_GAIN = int32_t(UINT16_MAX * (WAVETABLE_DECIBEL_SHIFT(-0.1) - 1.0)) * 4;
  _sampleData[sampleNumber-1][0].MODULATION_AMPLITUDE_SECOND_GAIN = int32_t(UINT16_MAX * (1.0 - WAVETABLE_DECIBEL_SHIFT(0.1))) * 4;

  _instrumentData[sampleNumber-1].sample_count = 1;
  _instrumentData[sampleNumber-1].sample_note_ranges = _completeRange;
  _instrumentData[sampleNumber-1].samples = _sampleData[sampleNumber-1];
};

void SampleFSIO::changeInstrumentParameters(byte sampleNumber, boolean loop, uint8_t delay_count, uint8_t attack_count, uint8_t hold_count, uint8_t decay_count, uint8_t release_count, uint8_t sustain_mult, uint8_t vibrato_delay, uint8_t vibrato_increment) {
  _sampleData[sampleNumber-1][0].LOOP = loop;

  _sampleData[sampleNumber-1][0].DELAY_COUNT = uint32_t(delay_count * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / AudioSynthWavetableSUCO::ENVELOPE_PERIOD + 0.5);
  _sampleData[sampleNumber-1][0].ATTACK_COUNT = uint32_t(attack_count * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / AudioSynthWavetableSUCO::ENVELOPE_PERIOD + 0.5);
  _sampleData[sampleNumber-1][0].HOLD_COUNT = uint32_t(hold_count * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / AudioSynthWavetableSUCO::ENVELOPE_PERIOD + 0.5);
  _sampleData[sampleNumber-1][0].DECAY_COUNT = uint32_t(decay_count * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / AudioSynthWavetableSUCO::ENVELOPE_PERIOD + 0.5);
  _sampleData[sampleNumber-1][0].RELEASE_COUNT = uint32_t(release_count * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / AudioSynthWavetableSUCO::ENVELOPE_PERIOD + 0.5);
  _sampleData[sampleNumber-1][0].SUSTAIN_MULT = int32_t(sustain_mult * AudioSynthWavetableSUCO::UNITY_GAIN);

  _sampleData[sampleNumber-1][0].VIBRATO_DELAY = uint32_t(vibrato_delay * AudioSynthWavetableSUCO::SAMPLES_PER_MSEC / (2 * AudioSynthWavetableSUCO::LFO_PERIOD));
  _sampleData[sampleNumber-1][0].VIBRATO_INCREMENT = uint32_t(vibrato_increment / 1000.0 * AudioSynthWavetableSUCO::LFO_PERIOD * (UINT32_MAX / AUDIO_SAMPLE_RATE_EXACT));
  _sampleData[sampleNumber-1][0].VIBRATO_PITCH_COEFFICIENT_INITIAL = (WAVETABLE_CENTS_SHIFT(-release_count / 1000.0) - 1.0) * 4;
  _sampleData[sampleNumber-1][0].VIBRATO_PITCH_COEFFICIENT_SECOND = (1.0 - WAVETABLE_CENTS_SHIFT(sustain_mult / 1000.0)) * 4;
}


// sampleNumber 1..72
AudioSynthWavetableSUCO::instrument_data SampleFSIO::getInstrumentDataBySample(byte sampleNumber) {
  // ToDo: check if instrumentData is available. if not, create it with standard parameters
  return _instrumentData[sampleNumber-1];
};
