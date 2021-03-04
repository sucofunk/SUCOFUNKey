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
// patternLength = complete length of pattern (max 64)
void SequencerScreen::drawTrackerAtPosition(uint16_t position, Pattern *p, bool editMode, byte highlightEvery) {
  boolean highlight = false;
  for (int r = 0; r<8; r++) {
    if (position-3+r >= 0 && position-3+r < p->getPatternLength()){
      sprintf(_cBuff3, "%03d", position-3+r);
      
      highlight = (((position-3+r)%highlightEvery)==0);

      _drawGridCell(0, r, _cBuff3, highlight ? _screen->C_GRID_DARK : _screen->C_BLACK);
      _drawGridCell(1, r, p->getSampleAt(1, position-3+r).displayText, highlight ? _screen->C_GRID_DARK : _screen->C_BLACK);
      _drawGridCell(2, r, p->getSampleAt(2, position-3+r).displayText, highlight ? _screen->C_GRID_DARK : _screen->C_BLACK);      
      _drawGridCell(3, r, p->getSampleAt(3, position-3+r).displayText, highlight ? _screen->C_GRID_DARK : _screen->C_BLACK);      
      _drawGridCell(4, r, p->getSampleAt(4, position-3+r).displayText, highlight ? _screen->C_GRID_DARK : _screen->C_BLACK);      

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
  _drawGridCell(0, row, "", _screen->C_BLACK);
  _drawGridCell(1, row, "", _screen->C_BLACK);
  _drawGridCell(2, row, "", _screen->C_BLACK);
  _drawGridCell(3, row, "", _screen->C_BLACK); 
  _drawGridCell(4, row, "", _screen->C_BLACK);   
}


void SequencerScreen::_drawGridCell(int col, int row, const char *text, uint16_t bgColor) {
  _y = 21 + (row*25);

  if (col == 0) {
    _x = 0;  
    _screen->fillRect(_x, _y, 39, 24, bgColor);
    _screen->drawText(text, 4, _y+18);
  } else {
    _x = 40+(col-1)*70;
    _screen->fillRect(_x, _y, 69, 24, bgColor);
    _screen->drawText(text, _x+15, _y+18);
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