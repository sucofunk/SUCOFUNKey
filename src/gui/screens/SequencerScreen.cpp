#include "SequencerScreen.h"

SequencerScreen::SequencerScreen(){};

SequencerScreen::SequencerScreen(Sucofunkey *keyboard, Screen *screen, SampleFSIO *sfsio, AudioResources *audioResources, Zoom *zoom) {
    _keyboard = keyboard;
    _screen = screen;    
    _sfsio = sfsio;
    _audioResources = audioResources;
    _zoom = zoom;
}



void SequencerScreen::initializeGrid(SongStructure *song, uint16_t cursorPosition) {
  _song = song;
  _screen->fillArea(_screen->AREA_CONTENT, _screen->C_BLACK);
  
  // calculate xPositionOffset for the current cursorPosition at the given zoomLevel
  
  // case 1: cursor is at the beginning
  if (cursorPosition == 0) _xPositionOffset = 0;


  // case 2: cursor is somewhere in the middle, but the whole grid does not fill the entire screen
  //          - adjust xpositionoffset and center cursor at the middle of the screen
  if ((_song->getSongLength()-cursorPosition)/_zoom->getZoomlevelOffset() > _xPositionCapacity) {
    // move grid to cursor position at left side of the screen
    _xPositionOffset = cursorPosition;

    // check if centering is possible and center
    uint16_t temp = static_cast<uint16_t>(_xPositionCapacity / 2) * _zoom->getZoomlevelOffset();
    if (_xPositionOffset - temp >= 0) {
      _xPositionOffset = _xPositionOffset - temp;
    }

  }

  // case 3: cursor is somewhere at the end and the rest of the grid from cursor position to end does not fill the entire screen
  //         - xpositionoffset will be adjusted to a level where the end of the grid will be the end of the screen. cursor will be somewhere on the screen
  if ((_song->getSongLength()-cursorPosition)/_zoom->getZoomlevelOffset() <= _xPositionCapacity) {
    int tempOffset = _song->getSongLength() - (_xPositionCapacity*_zoom->getZoomlevelOffset());
    _xPositionOffset = tempOffset > 0 ? tempOffset : 0;
  }

  // draw initial sequencer grid
  drawBPM(_song->getPlayBackSpeed());
  drawGrid(INIT);
}


void SequencerScreen::drawGrid(LastAction action) {

  uint16_t maxWidth = _screen->AREA_SCREEN.x2-2;

  if ((_song->getSongLength()/_zoom->getZoomlevelOffset()) < _xPositionCapacity) maxWidth = (_song->getSongLength()/_zoom->getZoomlevelOffset())*15+1; 

  // horizontal lines
  for (int i = 0; i <= 8; i++) {
    _screen->drawFastHLine(0, i*15+_screen->AREA_CONTENT.y1+1, maxWidth, _screen->C_GRID_BRIGHT);
  }

  // showTail -> draw the end of the song
  if (action == APPEND || action == SHORTEN) {
    if (_xPositionCapacity * _zoom->getZoomlevelOffset() >= _song->getSongLength()) {
      _xPositionOffset = 0;
    } else {
      _xPositionOffset = _song->getSongLength() - (_xPositionCapacity * _zoom->getZoomlevelOffset());
    }
  }

  // vertical lines  
  uint8_t lc = ((_xPositionOffset/_zoom->getZoomlevelOffset()) % static_cast<uint8_t>(_song->getSongResolution()*_zoom->getZoomlevelFactor())) + 1;

  int amountOfGridcellsToDraw = _amountOfGridCellsToDraw();

  for (int i=0; i<=amountOfGridcellsToDraw; i++) {
    // 120 -> channels * 15
    _screen->drawFastVLine(i*15, _screen->AREA_CONTENT.y1+1, 120, lc == 1 ?_screen->C_WHITE : _screen->C_GRID_BRIGHT);

    lc = (lc == _song->getSongResolution()*_zoom->getZoomlevelFactor()) ? 1 : lc+1;
  }

  if ((action == SHORTEN || action == SCALE)  && (amountOfGridcellsToDraw < _xPositionCapacity)) {
    _screen->fillRect((amountOfGridcellsToDraw)*15+1, _screen->AREA_CONTENT.y1+1, _screen->AREA_SCREEN.x2-(amountOfGridcellsToDraw*15)+2, 121, _screen->C_BLACK);
  }

  drawSamples();
}


void SequencerScreen::drawCursorAt(byte channel, uint16_t position, boolean draw) {
  // draw cursor if it is within the viewport and displayed range
  uint16_t offset;

  if (position >= _xPositionOffset && position < _xPositionOffset+(_xPositionCapacity*_zoom->getZoomlevelOffset()) && position < _song->getSongLength()) {
    offset = static_cast<uint16_t>((position - _xPositionOffset)/_zoom->getZoomlevelOffset());
    _screen->fillRect(offset*15+1, channel*15+_screen->AREA_CONTENT.y1+2, 14, 14, draw ? _screen->C_TRIM_START : _screen->C_BLACK);
  } 

  drawSample(channel, position, false);
};


void SequencerScreen::drawSample(byte channel, uint16_t position, boolean drawBackground) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);

  switch (sps.type) {
    case SongStructure::NOTE_OFF:
        hideSampleInfos();
        _screen->fillRect((position-_xPositionOffset)/_zoom->getZoomlevelOffset()*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_DARK);      
      break;
    case SongStructure::SAMPLE:
      _screen->fillRect((position-_xPositionOffset)/_zoom->getZoomlevelOffset()*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_WHITE); 
      showSampleInfos(channel, position, true);             
      break;
    case SongStructure::PARAMETER_CHANGE_SAMPLE:
      hideSampleInfos();    
      _screen->fillRect((position-_xPositionOffset)/_zoom->getZoomlevelOffset()*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_BRIGHT);
      showSampleInfos(channel, position, false);
      break;
    default:
      hideSampleInfos();
      if (drawBackground) {
              _screen->fillRect((position-_xPositionOffset)/_zoom->getZoomlevelOffset()*15+3, channel*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_BLACK);
      }      
      break;
  }
}


void SequencerScreen::drawSamples() {
  SongStructure::samplePointerStruct sps;

  for (int c = 0; c < 8; c++) {
    for (int p = 0; p < _amountOfGridCellsToDraw(); p++) {
      sps = _song->getPosition(c, _xPositionOffset + p*_zoom->getZoomlevelOffset());

      switch (sps.type) {
        case SongStructure::NOTE_OFF:
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_DARK);
          break;
        case SongStructure::SAMPLE:
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_WHITE);    
          break;
        case SongStructure::PARAMETER_CHANGE_SAMPLE:
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_GRID_BRIGHT);
          break;
        default:
          _screen->fillRect(p*15+3, c*15+_screen->AREA_CONTENT.y1+4, 10, 10, _screen->C_BLACK);
          break;
      }
    }
  }  
}

uint8_t SequencerScreen::_amountOfGridCellsToDraw() {  
  // calculate the amount of cells to draw for the current zoomlevel
  uint16_t right = _song->getSongLength() - _xPositionOffset; // darzustellender rest des songs in beats
  uint16_t rightCellsInCurrentZoomLevel = static_cast<uint16_t>(right / _zoom->getZoomlevelOffset());

  return rightCellsInCurrentZoomLevel <= _xPositionCapacity ? rightCellsInCurrentZoomLevel : _xPositionCapacity;
}


// returns true, if the grid needs to be repainted
boolean SequencerScreen::viewportCheckUpdate(byte channel, uint16_t position, LastAction action) {
  switch(action) {
    case LEFT:          
          if (position >= _zoom->getZoomlevelOffset() && position == _xPositionOffset-_zoom->getZoomlevelOffset()) {
            _xPositionOffset = _xPositionOffset - _zoom->getZoomlevelOffset();
            return true;            
          } else {
            if (position == 0 && _xPositionOffset == _zoom->getZoomlevelOffset()) {
              _xPositionOffset = 0;
              return true;            
            }
          }          
          break;
    case RIGHT:
          if (position < (_song->getSongLength() - _zoom->getZoomlevelOffset()+1) && (position - _xPositionOffset)/_zoom->getZoomlevelOffset() == _xPositionCapacity) { 
            _xPositionOffset = _xPositionOffset + _zoom->getZoomlevelOffset();
            return true;
          }
          break;
    case SCROLL_LEFT:
          if (_xPositionOffset/_zoom->getZoomlevelOffset() > 0) {
            _xPositionOffset = _xPositionOffset - _zoom->getZoomlevelOffset();
            return true;
          }
          break;
    case SCROLL_RIGHT:
          if (_song->getSongLength() > (_xPositionOffset + (_xPositionCapacity * _zoom->getZoomlevelOffset()))) {
            _xPositionOffset = _xPositionOffset + _zoom->getZoomlevelOffset();
            return true;
          }
          break;          
    default:
          break;
  }

  return false;
}

void SequencerScreen::drawPlayStepIndicator(uint16_t position, boolean draw) {

  if (!draw) {
    // delete old indicator.. to save calculation time and plane scrolling bux, just draw a black line over it ;)
    _screen->drawFastHLine(_screen->AREA_SCREEN.x1, 23, _screen->AREA_SCREEN.x2, _screen->C_BLACK);
    return;
  }

  if (position % _zoom->getZoomlevelOffset() == 0) {
    // is current position in viewport?
    if (draw && position >= _xPositionOffset && position < _xPositionOffset + (_xPositionCapacity * _zoom->getZoomlevelOffset())) {
      
      // delete old indicator..
      if (position == 0) {
        _screen->drawFastHLine(_screen->AREA_SCREEN.x1, 23, _screen->AREA_SCREEN.x2, _screen->C_BLACK);
      }

      // draw new one
      _screen->drawFastHLine(static_cast<uint16_t>((position - _xPositionOffset)/_zoom->getZoomlevelOffset()) * 15, 23, 15, _screen->C_ORANGE);
    } else {
      _screen->drawFastHLine(_screen->AREA_SCREEN.x1, 23, _screen->AREA_SCREEN.x2, _screen->C_BLACK);
    }    
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
    SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
    if (sps.type == SongStructure::SAMPLE) {
      sprintf(_cBuff3, "%d", _song->getSampleFromBucketId(sps.typeIndex).sampleNumber );
      _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTIONS_SAMPLENAME, _screen->TEXTPOSITION_LEFT_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff3);
    }
  }

  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION1, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "vol");
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION2, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "pan");

  if (fullInfo) {
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "pitch");
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION4, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "prob");
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
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  if (sps.type == SongStructure::SAMPLE) {
    _tempInt = static_cast<int>(_song->getSampleFromBucketId(sps.typeIndex).velocity/2);
  }
  if (sps.type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
    _tempInt = static_cast<int>(_song->getParameterChangeFromBucketId(sps.typeIndex).velocity/2);  
  }

  _screen->fillRect(_screen->AREA_SEQUENCER_OPTION1_VOLUME.x1+1+_tempInt, _screen->AREA_SEQUENCER_OPTION1_VOLUME.y1+1, 63-_tempInt, 7, _screen->C_BLACK);
  _screen->fillRect(_screen->AREA_SEQUENCER_OPTION1_VOLUME.x1+1, _screen->AREA_SEQUENCER_OPTION1_VOLUME.y1+1, _tempInt, 7, _screen->C_TRIM_START);
    
//  sprintf(_cBuff5_1, "%d", _pattern->getSampleAt(channel, position).velocity );
//  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION1_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_1);
};

void SequencerScreen::updateSampleInfoPanning(byte channel, uint16_t position) {

  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  if (sps.type == SongStructure::SAMPLE) {
    _tempInt = static_cast<int>(_song->getSampleFromBucketId(sps.typeIndex).stereoPosition/2);
  }
  if (sps.type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
    _tempInt = static_cast<int>(_song->getParameterChangeFromBucketId(sps.typeIndex).stereoPosition/2);  
  }

  _screen->fillArea(_screen->AREA_SEQUENCER_OPTION2_PANNING, _screen->C_BLACK);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION2_PANNING, 0.5, _screen->C_GRID_BRIGHT);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION2_PANNING, 0.5, _screen->C_GRID_BRIGHT);
  _screen->drawFastVLine(_screen->AREA_SEQUENCER_OPTION2_PANNING.x1+_tempInt, _screen->AREA_SEQUENCER_OPTION2_PANNING.y1, 8, _screen->C_WHITE);

//  sprintf(_cBuff5_2, "%d", _pattern->getSampleAt(channel, position).stereoPosition );  
//  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION2_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_2);
};

void SequencerScreen::updateSampleInfoPitch(byte channel, uint16_t position) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  if (sps.type == SongStructure::SAMPLE) {
    sprintf(_cBuff5_3, "%d", _song->getSampleFromBucketId(sps.typeIndex).pitchedNote);
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_3);
  }
};

void SequencerScreen::updateSampleInfoProbability(byte channel, uint16_t position) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  if (sps.type == SongStructure::SAMPLE) {
    sprintf(_cBuff5_4, "%d%%", _song->getSampleFromBucketId(sps.typeIndex).probability );      
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION4_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_4);
  }  
};


void SequencerScreen::hideSampleInfos() {
  if (_sampleInfosVisible) _screen->fillArea(_screen->AREA_SEQUENCER_OPTIONS, _screen->C_BLACK);
}

void SequencerScreen::drawExtMemPercentage(byte percent) {
  _screen->fillRect(0, 1, 100, 1, _screen->C_MEMORY_FREE);
  _screen->fillRect(0, 1, percent, 1, _screen->C_MEMORY_USED);
}


void SequencerScreen::drawBPM(float bpm) {
  sprintf(_cBuff10, "%g BPM", bpm);
  _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_RIGHT_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff10);
}