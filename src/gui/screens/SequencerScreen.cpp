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
   
#include "SequencerScreen.h"

SequencerScreen::SequencerScreen(){};

SequencerScreen::SequencerScreen(Sucofunkey* keyboard, Screen* screen, SampleFSIO* sfsio, AudioResources* audioResources, Zoom* zoom, Play* play, Selection* selection) {
    _keyboard = keyboard;
    _screen = screen;    
    _sfsio = sfsio;
    _audioResources = audioResources;
    _zoom = zoom;
    _play = play;
    _song = _play->getSong();
    _swing = _song->getSwing();
    _selection = selection;
}


// -----------------------------------------------------------------------------------------------------------------------------------------
// --- On Grid -----------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------


void SequencerScreen::initializeGrid(SongStructure *song, uint16_t cursorPosition) {
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
  if (action == INIT) {
      _screen->fillArea(_screen->AREA_GRID, _screen->C_BLACK);
  }

  int amountOfGridcellsToDraw = _amountOfGridCellsToDraw();

  uint16_t maxWidth = _cellWidth*amountOfGridcellsToDraw+1;

  if ((_song->getSongLength()/_zoom->getZoomlevelOffset()) <= _xPositionCapacity) {
    maxWidth = (_song->getSongLength()/_zoom->getZoomlevelOffset())*_cellWidth+1;
  } 

  // horizontal lines
  for (int i = 0; i <= 8; i++) {
    _screen->drawFastHLine(0, i*_cellHeight+_screen->AREA_CONTENT.y1+1, maxWidth, _screen->C_GRID_BRIGHT);
  }

  // vertical lines  
  uint8_t lc = ((_xPositionOffset/_zoom->getZoomlevelOffset()) % static_cast<uint8_t>(_song->getSongResolution()*_zoom->getZoomlevelFactor())) + 1;
  uint8_t h = _cellHeight * 8; // 8 Channels

  for (int i=0; i<=amountOfGridcellsToDraw; i++) {
    _screen->drawFastVLine(i*_cellWidth, _screen->AREA_CONTENT.y1+1, h, lc == 1 ?_screen->C_WHITE : _screen->C_GRID_BRIGHT);

    lc = (lc == _song->getSongResolution()*_zoom->getZoomlevelFactor()) ? 1 : lc+1;
  }


  if ((action == SHORTEN || action == SCALE)  && (amountOfGridcellsToDraw < _xPositionCapacity)) {
    _screen->fillRect((amountOfGridcellsToDraw)*_cellWidth+1, _screen->AREA_CONTENT.y1+1, _screen->AREA_SCREEN.x2-(amountOfGridcellsToDraw*_cellWidth)+2, h+1, _screen->C_BLACK);
  }


  // draw sheet divider lines..
  int previousSheet = _song->getPreviousSheetDividerPosition(_xPositionOffset);
  int nextSheet = _xPositionOffset;
  int viewPortEnd = _xPositionOffset + (_xPositionCapacity * _zoom->getZoomlevelOffset()); 
  boolean cont = true;

  if (previousSheet == _xPositionOffset) {
    // start cell is start of sheet -> draw sheet divider line
    _screen->drawFastVLine(0, _screen->AREA_CONTENT.y1+1, h, _screen->C_SHEET);
  }

  // iterate to check if there are multiple sheet divider visible in the viewport
  while (cont) {
    nextSheet = _song->getNextSheetDividerPosition(nextSheet);
    if (nextSheet <= viewPortEnd) {
      // next sheet divider line is in viewport
      int x = (nextSheet - _xPositionOffset) / _zoom->getZoomlevelOffset();
      _screen->drawFastVLine(x * _cellWidth, _screen->AREA_CONTENT.y1+1, h, _screen->C_SHEET);
    } else {
      cont = false;
    }
  }
  // END sheet divider lines


  if (action != SELECTION) {
    drawSamples();

    if (_selection->isActive()) {
      _drawSelection();
    }
  } 

  drawSnippets();
}



void SequencerScreen::drawGridAtPosition(uint16_t position) {
  _xPositionOffset = position - (position % _zoom->getZoomlevelOffset());
  drawGrid(INIT);
};



void SequencerScreen::drawCursorAt(byte channel, uint16_t position, boolean draw) {
  // draw cursor if it is within the viewport and displayed range
  uint16_t offset;

  if (position >= _xPositionOffset && position < _xPositionOffset+(_xPositionCapacity*_zoom->getZoomlevelOffset()) && position < _song->getSongLength()) {
    offset = static_cast<uint16_t>((position - _xPositionOffset)/_zoom->getZoomlevelOffset());
    _screen->fillRect(offset*_cellWidth+1, channel*_cellHeight+_screen->AREA_CONTENT.y1+2, _cellWidth-1, _cellHeight-1, draw ? _screen->C_CURSOR : _screen->C_BLACK);
    
    if (draw) {
      // save cursor position for cursor background drawing in drawSample
      _cursorChannel = channel; 
      _cursorPosition = position;
    } else {
      _cursorChannel = -1; 
      _cursorPosition = -1;      
    }
  } 

  drawSample(channel, position, false);
};


void SequencerScreen::drawSample(byte channel, uint16_t position, boolean drawBackground) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  int xRectStart = (position-_xPositionOffset)/_zoom->getZoomlevelOffset()*_cellWidth+3;
  int yRectStart = channel*_cellHeight+_screen->AREA_CONTENT.y1+4;
  int w = _cellWidth - 5;
  int h = _cellHeight - 5;
  boolean drawCursorInBackground = false;

  if (_cursorChannel == channel && _cursorPosition == position) {
    drawCursorInBackground = true;
  }

  switch (sps.type) {
    case SongStructure::NOTE_OFF:
        hideSampleInfos();
        _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_WHITE);
        _screen->fillRect(xRectStart+(w/2)-3, yRectStart+(h/2)-3, 6, 6, _screen->C_NOTEOFF_INNER);
      break;
    case SongStructure::SAMPLE:
      _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_WHITE); 

      // SWING Indicator
      if (_song->getSampleFromBucketId(sps.typeIndex).swing != 0) {
        byte group = _swing->getGroupFromBinarySwingExpression(_song->getSampleFromBucketId(sps.typeIndex).swing);
        _screen->drawLine(xRectStart+w-3, yRectStart, xRectStart+w-3, yRectStart+h-1, _screen->C_SWING_GROUPS[group]);
        _screen->drawLine(xRectStart+w-4, yRectStart, xRectStart+w-4, yRectStart+h-1, _screen->C_SWING_GROUPS[group]);
        _screen->drawLine(xRectStart+w-5, yRectStart, xRectStart+w-5, yRectStart+h-1, _screen->C_SWING_GROUPS[group]);
      }

      if (_song->getSampleFromBucketId(sps.typeIndex).reverse) {
        _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart, xRectStart, yRectStart, true, drawCursorInBackground ? _screen->C_CURSOR : _screen->C_BLACK);
        _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart+h-1, xRectStart, yRectStart+h-1, true, drawCursorInBackground ? _screen->C_CURSOR : _screen->C_BLACK);
      }

      showSampleInfos(channel, position, true);             
      break;
    case SongStructure::PARAMETER_CHANGE_SAMPLE:
      hideSampleInfos();    
      _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_GRID_BRIGHT);

      if (_song->getParameterChangeFromBucketId(sps.typeIndex).reverse) {
        _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart, xRectStart, yRectStart, true, drawCursorInBackground ? _screen->C_CURSOR : _screen->C_BLACK);
        _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart+h-1, xRectStart, yRectStart+h-1, true, drawCursorInBackground ? _screen->C_CURSOR : _screen->C_BLACK);
      }

      showSampleInfos(channel, position, false);
      break;
    case SongStructure::MIDINOTE:
      hideSampleInfos();
      _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_MIDINOTE); 

      if (_song->getMidiNoteFromBucketId(sps.typeIndex).velocity == 0) {
          _screen->fillRect(xRectStart+(w/2)-3, yRectStart+(h/2)-3, 6, 6, _screen->C_NOTEOFF_INNER);
      }
      showMidiNoteInfos(channel, position);
      break;      
    default:
      hideSampleInfos();
      if (drawBackground) {
              _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_BLACK);
      }      
      break;
  }
}


void SequencerScreen::drawSamples() {
  SongStructure::samplePointerStruct sps;
  int xRectStart, yRectStart;
  int w = _cellWidth - 5;
  int h = _cellHeight - 5;

  for (int c = 0; c < 8; c++) {
    for (int p = 0; p < _amountOfGridCellsToDraw(); p++) {
      sps = _song->getPosition(c, _xPositionOffset + p*_zoom->getZoomlevelOffset());

      xRectStart = p*_cellWidth+3;
      yRectStart = c*_cellHeight+_screen->AREA_CONTENT.y1+4;

      switch (sps.type) {
        case SongStructure::NOTE_OFF:
          _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_WHITE);
          _screen->fillRect(xRectStart+(w/2)-3, yRectStart+(h/2)-3, 6, 6, _screen->C_NOTEOFF_INNER);
          break;
        case SongStructure::SAMPLE:
          _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_WHITE);    

          if (_song->getSampleFromBucketId(sps.typeIndex).swing != 0) {
            byte group = _swing->getGroupFromBinarySwingExpression(_song->getSampleFromBucketId(sps.typeIndex).swing);
            _screen->drawLine(xRectStart+w-3, yRectStart, xRectStart+w-3, yRectStart+h-1, _screen->C_SWING_GROUPS[group]);
            _screen->drawLine(xRectStart+w-4, yRectStart, xRectStart+w-4, yRectStart+h-1, _screen->C_SWING_GROUPS[group]);
            _screen->drawLine(xRectStart+w-5, yRectStart, xRectStart+w-5, yRectStart+h-1, _screen->C_SWING_GROUPS[group]);        
          }

          if (_song->getSampleFromBucketId(sps.typeIndex).reverse) {
            _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart, xRectStart, yRectStart, true, _screen->C_BLACK);
            _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart+h-1, xRectStart, yRectStart+h-1, true, _screen->C_BLACK);
          }

          break;
        case SongStructure::PARAMETER_CHANGE_SAMPLE:
          _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_GRID_BRIGHT);

          if (_song->getParameterChangeFromBucketId(sps.typeIndex).reverse) {
            _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart, xRectStart, yRectStart, true, _screen->C_BLACK);
            _screen->drawTriangle(xRectStart, yRectStart+(h/2)-1, xRectStart+4, yRectStart+h-1, xRectStart, yRectStart+h-1, true, _screen->C_BLACK);
          }

          break;

        case SongStructure::MIDINOTE:
          _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_MIDINOTE);    
          
          if (_song->getMidiNoteFromBucketId(sps.typeIndex).velocity == 0) {
              _screen->fillRect(xRectStart+(w/2)-3, yRectStart+(h/2)-3, 6, 6, _screen->C_NOTEOFF_INNER);
          }
          break;

        default:
          _screen->fillRect(xRectStart, yRectStart, w, h, _screen->C_BLACK);
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
    // delete old indicator.. to save calculation time and plane scrolling box, just draw a black line over it ;)
    _screen->drawFastHLine(_screen->AREA_SCREEN.x1, 23, _screen->AREA_SCREEN.x2, _screen->C_BLACK);
    return;
  }

  if (position % _zoom->getZoomlevelOffset() == 0) {
    // is current position in viewport?
    if (draw && position >= _xPositionOffset && position < _xPositionOffset + (_xPositionCapacity * _zoom->getZoomlevelOffset())) {
      
      // delete old indicator..
      // normal play start || playing selection from start 
      if (position == 0 || (_selection->isActive() && position == _selection->getNormalizedSelection().startX)) {
        _screen->drawFastHLine(_screen->AREA_SCREEN.x1, 23, _screen->AREA_SCREEN.x2, _screen->C_BLACK);
      }

      // draw new one
      _screen->drawFastHLine(static_cast<uint16_t>((position - _xPositionOffset)/_zoom->getZoomlevelOffset()) * _cellWidth, 23, _cellWidth, _screen->C_ORANGE);
    } else {
      _screen->drawFastHLine(_screen->AREA_SCREEN.x1, 23, _screen->AREA_SCREEN.x2, _screen->C_BLACK);
    }    
  } 
}



// -----------------------------------------------------------------------------------------------------------------------------------------
// --- Off Grid ----------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------



// fullInfo = velocity, panning, pitch, probability
// !fullInfo = velocity, panning, pitch
void SequencerScreen::showSampleInfos(byte channel, uint16_t position, boolean fullInfo) {
  _sampleInfosVisible = true;

  //_screen->hr(_screen->AREA_SEQUENCER_OPTIONS_SAMPLENAME, 1, _screen->C_GRID_DARK);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1, 1, _screen->C_GRID_DARK);  
  _screen->vr(_screen->AREA_SEQUENCER_OPTION2, 1, _screen->C_GRID_DARK);  
  _screen->vr(_screen->AREA_SEQUENCER_OPTION3, 1, _screen->C_GRID_DARK);  
   
  if (fullInfo) {
    SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
    if (sps.type == SongStructure::SAMPLE) {
      sprintf(_cBuff3, "%d", _song->getSampleFromBucketId(sps.typeIndex).sampleNumber);
      _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTIONS_SAMPLENAME, _screen->TEXTPOSITION_LEFT_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff3);
      
      drawSwingInfoFromExpression(_song->getSampleFromBucketId(sps.typeIndex).swing);
    }
  }

  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION1, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "vol");
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION2, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "pan");
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "pitch");

  if (fullInfo) {    
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION4, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "prob");
  }

  // draw frame for volume bar
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 0.0, _screen->C_WHITE);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 1.0, _screen->C_WHITE);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 0.0, _screen->C_WHITE);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 1.0, _screen->C_WHITE);

  updateSampleInfoVolume(channel, position);
  updateSampleInfoPanning(channel, position);
  updateSampleInfoPitch(channel, position);  

  if (fullInfo) {      
    updateSampleInfoProbability(channel, position);
  }
}


void SequencerScreen::showMidiNoteInfos(byte channel, uint16_t position) {
  _sampleInfosVisible = true;

  _screen->hr(_screen->AREA_SEQUENCER_OPTIONS_SAMPLENAME, 1, _screen->C_GRID_DARK);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1, 1, _screen->C_GRID_DARK);  
  _screen->vr(_screen->AREA_SEQUENCER_OPTION2, 1, _screen->C_GRID_DARK);  
  _screen->vr(_screen->AREA_SEQUENCER_OPTION3, 1, _screen->C_GRID_DARK);  
   
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTIONS_SAMPLENAME, _screen->TEXTPOSITION_LEFT_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "MIDI Out");

  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION1, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "velocity");
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION2, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "channel");
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "note");
  _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION4, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "prob");

  // draw frame for volume bar
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 0.0, _screen->C_WHITE);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 1.0, _screen->C_WHITE);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 0.0, _screen->C_WHITE);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION1_VOLUME, 1.0, _screen->C_WHITE);

  updateSampleInfoVolume(channel, position);
  updateMidiChannel(channel, position);

  updateSampleInfoPitch(channel, position);
  updateSampleInfoProbability(channel, position);  
}

void SequencerScreen::updateMidiChannel(byte channel, uint16_t position) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  if (sps.type == SongStructure::MIDINOTE) {
    sprintf(_cBuff5_3, "%d", _song->getMidiNoteFromBucketId(sps.typeIndex).channel);
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION2_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_3);
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
  if (sps.type == SongStructure::MIDINOTE) {
    _tempInt = static_cast<int>(_song->getMidiNoteFromBucketId(sps.typeIndex).velocity/2);
  }

  _screen->fillRect(_screen->AREA_SEQUENCER_OPTION1_VOLUME.x1+1+_tempInt, _screen->AREA_SEQUENCER_OPTION1_VOLUME.y1+1, 63-_tempInt, 7, _screen->C_BLACK);
  _screen->fillRect(_screen->AREA_SEQUENCER_OPTION1_VOLUME.x1+1, _screen->AREA_SEQUENCER_OPTION1_VOLUME.y1+1, _tempInt, 7, _screen->C_TRIM_START);
};

void SequencerScreen::updateSampleInfoPanning(byte channel, uint16_t position) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  if (sps.type == SongStructure::SAMPLE) {
    _tempInt = static_cast<int>(_song->getSampleFromBucketId(sps.typeIndex).stereoPosition/2);
  }
  if (sps.type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
    _tempInt = static_cast<int>(_song->getParameterChangeFromBucketId(sps.typeIndex).stereoPosition/2);  
  }
  if (sps.type == SongStructure::MIDINOTE) {
    updateMidiChannel(channel, position);
    return;
  }  

  _screen->fillArea(_screen->AREA_SEQUENCER_OPTION2_PANNING, _screen->C_BLACK);
  _screen->hr(_screen->AREA_SEQUENCER_OPTION2_PANNING, 0.5, _screen->C_GRID_BRIGHT);
  _screen->vr(_screen->AREA_SEQUENCER_OPTION2_PANNING, 0.5, _screen->C_GRID_BRIGHT);
  _screen->drawFastVLine(_screen->AREA_SEQUENCER_OPTION2_PANNING.x1+_tempInt, _screen->AREA_SEQUENCER_OPTION2_PANNING.y1, 8, _screen->C_WHITE);
};

void SequencerScreen::updateSampleInfoPitch(byte channel, uint16_t position) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
  if (sps.type == SongStructure::SAMPLE) {
    _keyboard->getMIDINoteName(_song->getSampleFromBucketId(sps.typeIndex).pitchedNote).toCharArray(_cBuff5_4, 4);
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_4);    
  }
  if (sps.type == SongStructure::MIDINOTE) {
    _keyboard->getMIDINoteName(_song->getMidiNoteFromBucketId(sps.typeIndex).note).toCharArray(_cBuff5_4, 4);
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_4);
  }  
  if (sps.type == SongStructure::PARAMETER_CHANGE_SAMPLE) {
    sprintf(_cBuff5_4, "%d", _song->getParameterChangeFromBucketId(sps.typeIndex).pitchChange-127);
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION3_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_4);
  }
};

void SequencerScreen::updateSampleInfoProbability(byte channel, uint16_t position) {
  SongStructure::samplePointerStruct sps = _song->getPosition(channel, position);
    
  if (sps.type == SongStructure::SAMPLE) {
    sprintf(_cBuff5_4, "%d%%", _song->getSampleFromBucketId(sps.typeIndex).probability );  
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTION4_BAR, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5_4);
  }  

  if (sps.type == SongStructure::MIDINOTE) {
    sprintf(_cBuff5_4, "%d%%", _song->getMidiNoteFromBucketId(sps.typeIndex).probability );  
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
  _screen->drawTextInArea(_screen->AREA_BPM, _screen->TEXTPOSITION_RIGHT_TOP, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff10);
}

void SequencerScreen::drawSwingInfoFromExpression(byte expression) {
  byte group = _swing->getGroupFromBinarySwingExpression(expression);
  byte level = 0;
  if (group == 0) {
    level = _swing->getLevelFromBinarySwingExpression(expression);
  } else {
    level = _song->getSwingGroupLevel(group);
  }
  drawSwingInfo(level, group);
}


void SequencerScreen::drawSwingInfo(byte level, byte group) {

  // raster bar |x|x|x|x| | | | | | | (color == group)

  uint16_t barColor = _screen->C_SWING_GROUPS[group];

  if (level == 0) {
    _screen->fillArea(_screen->AREA_SEQUENCER_OPTIONS_SWING_LABEL, _screen->C_BLACK);
    _screen->drawTextInArea(_screen->AREA_SEQUENCER_OPTIONS_SWING_LABEL, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_LIGHTGREY, "no swing");
  } else {
    _screen->fillArea(_screen->AREA_SEQUENCER_OPTIONS_SWING_LABEL, _screen->C_BLACK);
    _screen->fillArea(_screen->AREA_SEQUENCER_OPTIONS_SWING_BAR, _screen->C_BLACK);

    _screen->vr(_screen->AREA_SEQUENCER_OPTIONS_SWING_BAR, 0.0, _screen->C_WHITE);
    _screen->vr(_screen->AREA_SEQUENCER_OPTIONS_SWING_BAR, 1.0, _screen->C_WHITE);
    _screen->hr(_screen->AREA_SEQUENCER_OPTIONS_SWING_BAR, 0.0, _screen->C_WHITE);
    _screen->hr(_screen->AREA_SEQUENCER_OPTIONS_SWING_BAR, 1.0, _screen->C_WHITE);

    for (int i=0; i<11; i++) {
      _screen->fillRect(_screen->AREA_SEQUENCER_OPTIONS_SWING_BAR.x1+1+(i*6), _screen->AREA_SEQUENCER_OPTIONS_SWING_BAR.y1+1, 5, 6, i < level ? barColor : _screen->C_BLACK);
    }
  }
}




void SequencerScreen::drawSelection(Selection* selection) {
  _selection = selection;
  _drawSelection();
};

void SequencerScreen::_drawSelection() {
  drawGrid(SELECTION);
  
  if (!_selection->isActive()) return;

  int vpStart = _xPositionOffset;
  int vpEnd = (_zoom->getZoomlevelOffset()*(_xPositionCapacity-1)) + _xPositionOffset;

  boolean startIsLeftOfVP = false;
  boolean startIsInVP = false;
  boolean startIsRightOfVP = false;
  boolean endIsLeftOfVP = false;
  boolean endIsInVP = false;
  boolean endIsRightOfVP = false;

  // check if in viewport
  // options: 1. complete in viewport
  //          2. starts left from vp and ends in vp
  //          3. starts in vp and ends right of viewport
  //          4. starts left from vp and ends right from vp

  if (_selection->getNormalizedSelection().startX >= vpStart && _selection->getNormalizedSelection().startX <= vpEnd) {
    startIsInVP = true;
  } else {
    if(_selection->getNormalizedSelection().startX < vpStart) startIsLeftOfVP = true;
    if(_selection->getNormalizedSelection().startX > vpEnd) startIsRightOfVP = true;
  }

  if (_selection->getNormalizedSelection().endX >= vpStart && _selection->getNormalizedSelection().endX <= vpEnd) {
    endIsInVP = true;
  } else {
    if(_selection->getNormalizedSelection().endX < vpStart) endIsLeftOfVP = true;
    if(_selection->getNormalizedSelection().endX > vpEnd) endIsRightOfVP = true; // irgendwie mit zomm offset arbeiten..
  }

/*  Serial.println("LIR LIR");
  Serial.print(startIsLeftOfVP);
  Serial.print(startIsInVP);
  Serial.print(startIsRightOfVP);
  Serial.print(" ");
  Serial.print(endIsLeftOfVP);
  Serial.print(endIsInVP);
  Serial.println(endIsRightOfVP);*/
  
  // not in viewport? dismiss and return
  if (startIsLeftOfVP && endIsLeftOfVP) return;
  if (startIsRightOfVP && endIsRightOfVP) return;  

  // start drawing one of the possible cases..
  int upperLeftX = ((_selection->getNormalizedSelection().startX-_xPositionOffset)/_zoom->getZoomlevelOffset())*_cellWidth;
  int upperLeftY = _selection->getNormalizedSelection().startY*_cellHeight+_screen->AREA_CONTENT.y1+1;
  
  int height = (_selection->getNormalizedSelection().endY-_selection->getNormalizedSelection().startY+1)*_cellHeight;
  int width = ((_selection->getNormalizedSelection().endX - _selection->getNormalizedSelection().startX + _zoom->getZoomlevelOffset())/_zoom->getZoomlevelOffset())*_cellWidth;

  if (startIsInVP && endIsInVP) {
    // draw rect top border
    _screen->drawFastHLine(upperLeftX, upperLeftY,  width, _screen->C_SELECTION);

    // draw rect bottom border
    _screen->drawFastHLine(upperLeftX, upperLeftY+height,  width, _screen->C_SELECTION);

    // draw rect left border
    _screen->drawFastVLine(upperLeftX, upperLeftY, height, _screen->C_SELECTION);

    // draw rect right border
    _screen->drawFastVLine(upperLeftX+width, upperLeftY, height, _screen->C_SELECTION);
  }

  if (startIsInVP && endIsRightOfVP) {    
    // top and bottom border start to end of grid
    _screen->drawFastHLine(upperLeftX, upperLeftY, (_xPositionCapacity*_cellWidth)-upperLeftX+1, _screen->C_SELECTION);
    _screen->drawFastHLine(upperLeftX, upperLeftY+height, (_xPositionCapacity*_cellWidth)-upperLeftX+1, _screen->C_SELECTION);

    // draw rect left border
    _screen->drawFastVLine(upperLeftX, upperLeftY, height, _screen->C_SELECTION);
  }

  if (startIsLeftOfVP && endIsInVP) {
    // top and votom from 0 to end of selection
    width = ((_selection->getNormalizedSelection().endX-_xPositionOffset)/_zoom->getZoomlevelOffset()+1)*_cellWidth;
    _screen->drawFastHLine(0, upperLeftY,  width, _screen->C_SELECTION);
    _screen->drawFastHLine(0, upperLeftY+height,  width, _screen->C_SELECTION);

    // right border
    _screen->drawFastVLine(width, upperLeftY, height, _screen->C_SELECTION);    
  }

  if (startIsLeftOfVP && endIsRightOfVP) {
    // top and bottom from 0 to end of grid
    _screen->drawFastHLine(0, upperLeftY, _xPositionCapacity*_cellWidth+1, _screen->C_SELECTION);
    _screen->drawFastHLine(0, upperLeftY+height, _xPositionCapacity*_cellWidth+1, _screen->C_SELECTION);   
  }

}


void SequencerScreen::drawSnippets() {    
  for (int i=1; i<=14; i++) {
    if (!_song->isSnippetSlotFree(i)) {
      _drawSnippet(_song->getSnippet(i));
    }    
  }  
};

void SequencerScreen::_drawSnippet(Selection::SelectionStruct snippet) {
  int vpStart = _xPositionOffset;
  int vpEnd = (_zoom->getZoomlevelOffset()*(_xPositionCapacity-1)) + _xPositionOffset;

  boolean startIsLeftOfVP = false;
  boolean startIsInVP = false;
  boolean startIsRightOfVP = false;
  boolean endIsLeftOfVP = false;
  boolean endIsInVP = false;
  boolean endIsRightOfVP = false;

  // check if in viewport
  // options: 1. complete in viewport
  //          2. starts left from vp and ends in vp
  //          3. starts in vp and ends right of viewport
  //          4. starts left from vp and ends right from vp

  if (snippet.startX >= vpStart && snippet.startX <= vpEnd) {
    startIsInVP = true;
  } else {
    if(snippet.startX < vpStart) startIsLeftOfVP = true;
    if(snippet.startX > vpEnd) startIsRightOfVP = true;
  }

  if (snippet.endX >= vpStart && snippet.endX <= vpEnd) {
    endIsInVP = true;
  } else {
    if(snippet.endX < vpStart) endIsLeftOfVP = true;
    if(snippet.endX > vpEnd) endIsRightOfVP = true; // irgendwie mit zomm offset arbeiten..
  }

/*  Serial.println("LIR LIR");
  Serial.print(startIsLeftOfVP);
  Serial.print(startIsInVP);
  Serial.print(startIsRightOfVP);
  Serial.print(" ");
  Serial.print(endIsLeftOfVP);
  Serial.print(endIsInVP);
  Serial.println(endIsRightOfVP);*/
  
  // not in viewport? dismiss and return
  if (startIsLeftOfVP && endIsLeftOfVP) return;
  if (startIsRightOfVP && endIsRightOfVP) return;  

  // start drawing one of the possible cases..
  int upperLeftX = ((snippet.startX-_xPositionOffset)/_zoom->getZoomlevelOffset())*_cellWidth;
  int upperLeftY = snippet.startY*_cellHeight+_screen->AREA_CONTENT.y1+1;
  
  int height = (snippet.endY - snippet.startY+1)*_cellHeight;
  int width = ((snippet.endX - snippet.startX + _zoom->getZoomlevelOffset())/_zoom->getZoomlevelOffset())*_cellWidth;

  if (startIsInVP && endIsInVP) {
    // draw rect top border
    _screen->drawFastHLine(upperLeftX, upperLeftY,  width, _screen->C_SNIPPET);

    // draw rect bottom border
    _screen->drawFastHLine(upperLeftX, upperLeftY+height,  width, _screen->C_SNIPPET);

    // draw rect left border
    _screen->drawFastVLine(upperLeftX, upperLeftY, height, _screen->C_SNIPPET);

    // draw rect right border
    _screen->drawFastVLine(upperLeftX+width, upperLeftY, height, _screen->C_SNIPPET);
  }

  if (startIsInVP && endIsRightOfVP) {    
    // top and bottom border start to end of grid
    _screen->drawFastHLine(upperLeftX, upperLeftY, (_xPositionCapacity*_cellWidth)-upperLeftX+1, _screen->C_SNIPPET);
    _screen->drawFastHLine(upperLeftX, upperLeftY+height, (_xPositionCapacity*_cellWidth)-upperLeftX+1, _screen->C_SNIPPET);

    // draw rect left border
    _screen->drawFastVLine(upperLeftX, upperLeftY, height, _screen->C_SNIPPET);
  }

  if (startIsLeftOfVP && endIsInVP) {
    // top and votom from 0 to end of selection
    width = ((snippet.endX-_xPositionOffset)/_zoom->getZoomlevelOffset()+1)*_cellWidth;
    _screen->drawFastHLine(0, upperLeftY,  width, _screen->C_SNIPPET);
    _screen->drawFastHLine(0, upperLeftY+height,  width, _screen->C_SNIPPET);

    // right border
    _screen->drawFastVLine(width, upperLeftY, height, _screen->C_SNIPPET);    
  }

  if (startIsLeftOfVP && endIsRightOfVP) {
    // top and bottom from 0 to end of grid
    _screen->drawFastHLine(0, upperLeftY, _xPositionCapacity*_cellWidth+1, _screen->C_SNIPPET);
    _screen->drawFastHLine(0, upperLeftY+height, _xPositionCapacity*_cellWidth+1, _screen->C_SNIPPET);   
  }  
};
