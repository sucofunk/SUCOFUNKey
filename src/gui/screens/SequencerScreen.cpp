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

  _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_LEFT_TOP, true, _screen->TEXTSIZE_MEDIUM, _screen->C_WHITE, "Tracker");  

  _screen->drawFastHLine(0, 20, _screen->AREA_SCREEN.x2, _screen->C_GRID_DARK);
  _screen->drawFastHLine(0, 45, _screen->AREA_SCREEN.x2, _screen->C_GRID_DARK);
  _screen->drawFastHLine(0, 70, _screen->AREA_SCREEN.x2, _screen->C_GRID_DARK);

  _screen->drawFastHLine(0, 145, _screen->AREA_SCREEN.x2, _screen->C_GRID_DARK);
  _screen->drawFastHLine(0, 170, _screen->AREA_SCREEN.x2, _screen->C_GRID_DARK);
  _screen->drawFastHLine(0, 195, _screen->AREA_SCREEN.x2, _screen->C_GRID_DARK);
  _screen->drawFastHLine(0, 220, _screen->AREA_SCREEN.x2, _screen->C_GRID_DARK);

  _screen->drawFastVLine( 0, 20, _screen->AREA_SCREEN.y2-40, _screen->C_GRID_DARK);
  _screen->drawFastVLine(39, 20, _screen->AREA_SCREEN.y2-39, _screen->C_GRID_BRIGHT);
  _screen->drawFastVLine(109, 20, _screen->AREA_SCREEN.y2-40, _screen->C_GRID_DARK);
  _screen->drawFastVLine(179, 20, _screen->AREA_SCREEN.y2-40, _screen->C_GRID_DARK);
  _screen->drawFastVLine(249, 20, _screen->AREA_SCREEN.y2-40, _screen->C_GRID_DARK);  
  _screen->drawFastVLine(319, 20, _screen->AREA_SCREEN.y2-40, _screen->C_GRID_DARK);   

  _screen->drawFastHLine(0, 95, _screen->AREA_SCREEN.x2, _screen->C_GRID_BRIGHT);
  _screen->drawFastHLine(0, 120, _screen->AREA_SCREEN.x2, _screen->C_GRID_BRIGHT);
}

// position = current position on highlighted row
void SequencerScreen::drawTrackerAtPosition(uint16_t position, Pattern *p, bool editMode, byte highlightEvery) {
  boolean highlight = false;
  for (int r = 0; r<8; r++) {
    if (position-3+r >= 0 && position-3+r < p->getPatternLength()){
      sprintf(_cBuff3, "%03d", position-3+r);
      
      highlight = (((position-3+r)%highlightEvery)==0);

      _drawGridCell(0, r, true, _cBuff3, 0, 0, highlight ? _screen->C_GRID_DARK : _screen->C_BLACK);
      
      for (int c=1; c<5; c++) {
        _drawGridCell(c, r, p->getSampleAt(c, position-3+r).sampleNumber == 255 ? false : true, p->getSampleAt(c, position-3+r).displayText, p->getSampleAt(c, position-3+r).stereoPosition, p->getSampleAt(c, position-3+r).velocity,  highlight ? _screen->C_GRID_DARK : _screen->C_BLACK);
      }
      
    } else {
      if (editMode && position-3+r < 0) {
        _drawEmptyRow(r);
      }
      if (editMode && position-3+r >= p->getPatternLength()) {
        _drawEmptyRow(r);
      }      
    }
  }
}


void SequencerScreen::drawCursorAtColumn(byte column) {
  _screen->drawFastHLine(0, 95, _screen->AREA_SCREEN.x2, _screen->C_GRID_BRIGHT);
  _screen->drawFastHLine(0, 120, _screen->AREA_SCREEN.x2, _screen->C_GRID_BRIGHT);
  
  _screen->drawFastHLine(39+((column-1)*70), 95, 70, _screen->C_WHITE);
  _screen->drawFastHLine(39+((column-1)*70), 120, 70, _screen->C_WHITE);
};


void SequencerScreen::_drawEmptyRow(uint16_t row) {
  _drawGridCell(0, row, false, "", 0, 0, _screen->C_BLACK);
  _drawGridCell(1, row, false, "", 0, 0, _screen->C_BLACK);
  _drawGridCell(2, row, false, "", 0, 0, _screen->C_BLACK);
  _drawGridCell(3, row, false, "", 0, 0, _screen->C_BLACK);
  _drawGridCell(4, row, false, "", 0, 0, _screen->C_BLACK);
}


void SequencerScreen::_drawGridCell(int col, int row, boolean hasContent, const char *text, byte stereoPosition, byte velocity, uint16_t bgColor) {
  _y = 21 + (row*25);

  int panOffset = 0;
  //int hideOffset = 2;
  //int radius = static_cast<int>(floor(velocity/13)+1);

  if (col == 0) {
    _x = 0;  
    _screen->fillRect(_x, _y, 39, 24, bgColor);
    _screen->drawText(text, 4, _y+18);
  } else {
    _x = 40+(col-1)*70;
    _screen->fillRect(_x, _y, 69, 24, bgColor);
    if (hasContent) {
      _screen->drawText(text, _x+2, _y+18);
//    _screen->drawText(text, _x+15, _y+18);

      // draw panning
      // pan   0 = -25 px  // 100% left
      // pan  64 =   0 px  // center
      // pan 127 = +25 px  // 100% right

      if (stereoPosition < 64) {
        panOffset = static_cast<int>(-10 + floor(stereoPosition / 6));
        //hideOffset = 2;
      }
      
      if (stereoPosition > 64) {
        panOffset = static_cast<int>(floor((stereoPosition-64) / 6));
        //hideOffset = -2;
      }

      // draw center crosshair
      _screen->drawLine(_x+48, _y+12, _x+52, _y+12, _screen->C_WHITE);
      _screen->drawLine(_x+50, _y+10, _x+50, _y+14, _screen->C_WHITE);

      // draw velocity circle at panning position
      _screen->drawCircle(_x+50+panOffset, _y+12, static_cast<int>(floor(velocity/13)), false, _screen->C_ORANGE);


/*      if (abs(panOffset) > radius) {
        // draw center marker circle when velocity bubble is right or left of marker
        //_screen->drawCircle(_x+50, _y+12, 1, true, _screen->C_WHITE);
//        _screen->drawPixel(_x+50, _y+12, _screen->C_WHITE);
          _screen->drawLine(_x+48, _y+12, _x+52, _y+12, _screen->C_WHITE);
          _screen->drawLine(_x+50, _y+10, _x+50, _y+14, _screen->C_WHITE);

      } else {
        if (radius > 1) {
          _screen->drawLine(_x+48, _y+12, _x+52, _y+12, _screen->C_WHITE);
          _screen->drawLine(_x+50, _y+10, _x+50, _y+14, _screen->C_WHITE);

          //_screen->drawPixel(_x+50, _y+12, _screen->C_WHITE);
          //_screen->drawCircle(_x+50, _y+12, 1, true, bgColor);
        }        
      }
*/
    }
  }
} 


void SequencerScreen::drawBPM(float bpm) {
  /*_screen->setTextColor(_screen->C_WHITE);
  _screen->setFont(&OxygenMono_Regular8pt7b);
  _screen->fillRect(0, 221, 160, 19, _screen->C_BLACK);

  sprintf(_cBuff10, "BPM: %g", bpm);
  _screen->setCursor(4, 239);
  _screen->print(_cBuff10);
  */
}


void SequencerScreen::drawExtMemPercentage(byte percent) {
  _screen->fillRect(219, 1, 100, 1, _screen->C_MEMORY_FREE);
  _screen->fillRect(219, 1, percent, 1, _screen->C_MEMORY_USED);
};