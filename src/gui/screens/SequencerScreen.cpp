#include "SequencerScreen.h"

SequencerScreen::SequencerScreen(){};

SequencerScreen::SequencerScreen(Sucofunkey *keyboard, Screen *screen, SampleFSIO *sfsio, AudioResources *audioResources) {
    _keyboard = keyboard;
    _screen = screen;    
    _sfsio = sfsio;
    _audioResources = audioResources;
}



void SequencerScreen::initializeGrid(Pattern *pattern) {
  _pattern = pattern;
  _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);
  // draw initial sequencer grid
  drawBPM(_pattern->getPatternSpeed());
  drawGrid(INIT);
}


void SequencerScreen::drawGrid(LastAction action) {

  uint16_t maxWidth = _screen->AREA_SCREEN.x2-2;

  if (_pattern->getPatternLength() < _xPositionCapacity) maxWidth = _pattern->getPatternLength()*15+1; 

  // horizontal lines
  for (int i=0; i<=_pattern->channels; i++) {
    _screen->drawFastHLine(0, i*15+_screen->AREA_CONTENT.y1+1, maxWidth, _screen->C_GRID_BRIGHT);
  }

  // showTail -> draw the end of the pattern (scrolled to the end)
  if (action == APPEND || action == SHORTEN) {
    _xPositionOffset = _pattern->getPatternLength() > _xPositionCapacity ? _pattern->getPatternLength() - _xPositionCapacity : 0;
  }

  // vertical lines
  uint8_t lc = (_xPositionOffset%_pattern->getPatternResolution())+1;

  int amountOfGridcellsToDraw = _amountOfGridCellsToDraw();

  for (int i=0; i<=amountOfGridcellsToDraw; i++) {
    _screen->drawFastVLine(i*15, _screen->AREA_CONTENT.y1+1, _pattern->channels*15, lc == 1 ?_screen->C_WHITE : _screen->C_GRID_BRIGHT);
    lc = lc == _pattern->getPatternResolution() ? 1 : lc+1;
  }

  if ((action == SHORTEN || action == SCALE)  && (amountOfGridcellsToDraw < _xPositionCapacity)) {
    _screen->fillRect((amountOfGridcellsToDraw)*15+1, _screen->AREA_CONTENT.y1+1, _screen->AREA_SCREEN.x2-(amountOfGridcellsToDraw*15)+2, _pattern->channels*15+1, _screen->C_BLACK);
  }

  drawSamples();
}


void SequencerScreen::drawCursorAt(byte channel, uint16_t position, boolean draw) {
  // draw cursor if it is within the viewport and displayed range
  if ((position >= _xPositionOffset && position < _xPositionOffset+_xPositionCapacity) && position < _pattern->getPatternLength()) {
    _screen->fillRect((position-_xPositionOffset)*15+1, channel*15+_screen->AREA_CONTENT.y1+2, 14, 14, draw ? _screen->C_TRIM_START : _screen->C_BLACK);
  }  

  drawSample(channel, position, false);
};


void SequencerScreen::drawSample(byte channel, uint16_t position, boolean drawBackground) {
  _tempSample = _pattern->getSampleAt(channel, position);
  
  if (_tempSample.sampleNumber < 253) {
      _screen->fillRect((position-_xPositionOffset)*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_WHITE); 
      showSampleInfos(channel, position, true);       
  } else {
    hideSampleInfos();
    
    if (_tempSample.sampleNumber == 255 && drawBackground) {
      _screen->fillRect((position-_xPositionOffset)*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_BLACK);
    }
    if (_tempSample.sampleNumber == 254) {
      _screen->fillRect((position-_xPositionOffset)*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_DARK);
    }
    if (_tempSample.sampleNumber == 253) {
      _screen->fillRect((position-_xPositionOffset)*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_BRIGHT);
      showSampleInfos(channel, position, false);
    }        
  }
}



void SequencerScreen::drawSamples() {
  for (int c = 0; c < _pattern->channels; c++) {
    for (int p = 0; p < _amountOfGridCellsToDraw(); p++) {
      _tempSample = _pattern->getSampleAt(c, _xPositionOffset + p);
      
      if (_tempSample.sampleNumber < 253) {
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_WHITE);        
      } else {
        if (_tempSample.sampleNumber == 255) {
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_BLACK);
        }
        if (_tempSample.sampleNumber == 254) {
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_DARK);
        }
        if (_tempSample.sampleNumber == 253) {
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_BRIGHT);
        }        
      }
    }
  }  
}

uint8_t SequencerScreen::_amountOfGridCellsToDraw() {
  return (_pattern->getPatternLength() - _xPositionOffset) <= _xPositionCapacity ? _pattern->getPatternLength() - _xPositionOffset : _xPositionCapacity;
}



// returns true, if the grid needs to be repainted
boolean SequencerScreen::viewportCheckUpdate(byte channel, uint16_t position, LastAction action) {
  switch(action) {
    case LEFT:          
          if (position > 0 && position == _xPositionOffset-1) {
            _xPositionOffset--;            
            return true;            
          } else {
            if (position == 0 && _xPositionOffset == 1) {
              _xPositionOffset--;
              return true;            
            }
          }          
          break;
    case RIGHT:
          if (position < _pattern->getPatternLength() && position - _xPositionOffset == _xPositionCapacity) { 
            _xPositionOffset++;
            return true;
          }
          break;
    case SCROLL_LEFT:
          if (_xPositionOffset > 0) {      
            _xPositionOffset--;
            return true;
          }
          break;
    case SCROLL_RIGHT:
          if (_pattern->getPatternLength() > _xPositionOffset + _xPositionCapacity) {
            _xPositionOffset++;
            return true;
          }
          break;          
    default:
          break;
  }

  return false;
}

void SequencerScreen::drawPlayStepIndicator(uint16_t position, boolean draw) {
  // delete old indicator.. to save calculation time and plane scrolling bux, just draw a black line over it ;)
  _screen->drawFastHLine(_screen->AREA_SCREEN.x1, 23, _screen->AREA_SCREEN.x2, _screen->C_BLACK);

  // is current position in viewport?
  if (draw && position >= _xPositionOffset && position < _xPositionOffset + _xPositionCapacity) {    
    _screen->drawFastHLine((position - _xPositionOffset) * 15, 23, 15, _screen->C_ORANGE);
  }
}

// fullInfo = velocity, panning, pitch, probability
// !fullInfo = velocity, panning
void SequencerScreen::showSampleInfos(byte channel, uint16_t position, boolean fullInfo) {
  _sampleInfosVisible = true;

  _screen->hr(_screen->AREA_SEQUENCER_OPTIONS_SAMPLENAME, 1, _screen->C_GRID_DARK);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1, 1, _screen->C_GRID_DARK);  
  _screen->vr(_screen->AREA_SEQUENCER_OPTION2, 1, _screen->C_GRID_DARK);  
  _screen->vr(_screen->AREA_SEQUENCER_OPTION3, 1, _screen->C_GRID_DARK);  
   
  if (fullInfo) {
    sprintf(_cBuff3, "%d", _pattern->getSampleAt(channel, position).sampleNumber );
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTIONS_SAMPLENAME, _screen->TEXTPOSITION_LEFT_VCENTER, true, _screen->TEXTSIZE_SMALL, false, _screen->C_WHITE, _cBuff3);
  }

  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION1, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_SMALL, false, _screen->C_LIGHTGREY, "vol");
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION2, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_SMALL, false, _screen->C_LIGHTGREY, "pan");

  if (fullInfo) {
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_SMALL, false, _screen->C_LIGHTGREY, "pitch");
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION4, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_SMALL, false, _screen->C_LIGHTGREY, "prob");
  }

  // draw frame for volume bar
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 0.0, _screen->C_WHITE);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 1.0, _screen->C_WHITE);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 0.0, _screen->C_WHITE);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 1.0, _screen->C_WHITE);

  updateSampleInfoVolume(channel, position);
  updateSampleInfoPanning(channel, position);

  if (fullInfo) {
    updateSampleInfoPitch(channel, position);  
    updateSampleInfoProbability(channel, position);
  }
}


void SequencerScreen::updateSampleInfoVolume(byte channel, uint16_t position) {
    // draw volume/velocity bar
    _tempInt = static_cast<int>(_pattern->getSampleAt(channel, position).velocity/2);
    _screen->fillRect(_screen->AREA_SEQUENCER_OPTION1_VOLUME.x1+1+_tempInt, _screen->AREA_SEQUENCER_OPTION1_VOLUME.y1+1, 63-_tempInt, 7, _screen->C_BLACK);
    _screen->fillRect(_screen->AREA_SEQUENCER_OPTION1_VOLUME.x1+1, _screen->AREA_SEQUENCER_OPTION1_VOLUME.y1+1, _tempInt, 7, _screen->C_TRIM_START);
    
//  sprintf(_cBuff5_1, "%d", _pattern->getSampleAt(channel, position).velocity );
//  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION1_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_1);
};

void SequencerScreen::updateSampleInfoPanning(byte channel, uint16_t position) {

  _tempInt = static_cast<int>(_pattern->getSampleAt(channel, position).stereoPosition/2);
  _screen->fillArea(_screen->AREA_SEQUENCER_OPTION2_PANNING, _screen->C_BLACK);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION2_PANNING, 0.5, _screen->C_GRID_BRIGHT);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION2_PANNING, 0.5, _screen->C_GRID_BRIGHT);
  _screen->drawFastVLine(_screen->AREA_SEQUENCER_OPTION2_PANNING.x1+_tempInt, _screen->AREA_SEQUENCER_OPTION2_PANNING.y1, 8, _screen->C_WHITE);

//  sprintf(_cBuff5_2, "%d", _pattern->getSampleAt(channel, position).stereoPosition );  
//  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION2_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_2);
};

void SequencerScreen::updateSampleInfoPitch(byte channel, uint16_t position) {
  sprintf(_cBuff5_3, "%d", _pattern->getSampleAt(channel, position).pitchedNote);
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_3);
};

void SequencerScreen::updateSampleInfoProbability(byte channel, uint16_t position) {
  sprintf(_cBuff5_4, "%d%%", _pattern->getSampleAt(channel, position).probability );      
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION4_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_4);
};


void SequencerScreen::hideSampleInfos() {
  if (_sampleInfosVisible) _screen->fillArea(_screen->AREA_SEQUENCER_OPTIONS, _screen->C_BLACK);
}

void SequencerScreen::drawExtMemPercentage(byte percent) {
  //_screen->fillRect(219, 1, 100, 1, _screen->C_MEMORY_FREE);
  //_screen->fillRect(219, 1, percent, 1, _screen->C_MEMORY_USED);
}


void SequencerScreen::drawBPM(float bpm) {
  sprintf(_cBuff10, "%g BPM", bpm);
  _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_RIGHT_TOP, true, _screen->TEXTSIZE_SMALL, false, _screen->C_WHITE, _cBuff10);
}



/*
void SequencerScreen::drawExtMemPercentage(byte percent) {
  _screen->fillRect(219, 1, 100, 1, _screen->C_MEMORY_FREE);
  _screen->fillRect(219, 1, percent, 1, _screen->C_MEMORY_USED);
};
*/