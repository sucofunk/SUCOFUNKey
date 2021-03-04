#include <Arduino.h>
#include "Screen.h"
#include <SD.h>
//#include "../context/sampler/Sampler.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "fonts/OxygenMono_Regular8pt7b.h" // https://fonts.google.com/specimen/Oxygen+Mono
//#include "../context/sequencer/Pattern.h"

Screen::Screen(Adafruit_ST7789 *tft, int BL) {
  _tft = tft;
}

void Screen::testBild(const char* text) {
    fillArea(AREA_SCREEN, C_BLACK);    
    drawTextInArea(AREA_SCREEN, TEXTPOSITION_HCENTER_VCENTER, false, TEXTSIZE_MEDIUM, C_WHITE, text);

/*    drawTextInArea(AREA_SCREEN, TEXTPOSITION_LEFT_VCENTER, false, TEXTSIZE_MEDIUM, C_WHITE, "left");    
    drawTextInArea(AREA_SCREEN, TEXTPOSITION_RIGHT_VCENTER, false, TEXTSIZE_MEDIUM, C_WHITE, "right");    

    drawTextInArea(AREA_SCREEN, TEXTPOSITION_HCENTER_TOP, false, TEXTSIZE_MEDIUM, C_WHITE, "center");    
    drawTextInArea(AREA_SCREEN, TEXTPOSITION_LEFT_TOP, false, TEXTSIZE_MEDIUM, C_WHITE, "left");    
    drawTextInArea(AREA_SCREEN, TEXTPOSITION_RIGHT_TOP, false, TEXTSIZE_MEDIUM, C_WHITE, "right");    

    drawTextInArea(AREA_SCREEN, TEXTPOSITION_HCENTER_BOTTOM, false, TEXTSIZE_MEDIUM, C_WHITE, "center");    
    drawTextInArea(AREA_SCREEN, TEXTPOSITION_LEFT_BOTTOM, false, TEXTSIZE_MEDIUM, C_WHITE, "left");    
    drawTextInArea(AREA_SCREEN, TEXTPOSITION_RIGHT_BOTTOM, false, TEXTSIZE_MEDIUM, C_WHITE, "right");
*/
}

void Screen::fillArea(Area area, uint16_t color) {
  _tft->fillRect(area.x1, area.y1, area.x2-area.x1+(area.x1 == 0 ? 1 : 0), area.y2-area.y1+(area.x1 == 0 ? 1 : 0), color);
  area.bgColor = color;
  area.transparent = false;
}

void Screen::clearAreaLTR(Area area, uint16_t color, int delayTime) {  
  for (int x=area.x1; x<=area.x2; x++) {
    _tft->drawLine(x, area.y1, x, area.y2, color);
    delay(delayTime);
  }
  
  area.bgColor = color;
  area.transparent = false;
}

void Screen::clearAreaRTL(Area area, uint16_t color, int delayTime) {    
  for (int x=area.x2; x>=area.x1; x--) {
    _tft->drawLine(x, area.y1, x, area.y2, color);
    delay(delayTime);
  }
  
  area.bgColor = color;
  area.transparent = false;
}


// draws a vertical line at the in vpos defined percentage (0..1) of the height of the area
void Screen::hr(Area area, float vpos, uint16_t color) {
  _tft->drawLine(area.x1, area.y1 + static_cast<int>((area.y2-area.y1)*vpos), area.x2, area.y1 + static_cast<int>((area.y2-area.y1)*vpos), color);  
}

// draws a vertical line at the in hpos defined percentage (0..1) of the width of the area
void Screen::vr(Area area, float hpos, uint16_t color) {
  _tft->drawLine(area.x1 + abs((area.x2-area.x1)*hpos), area.y1, area.x1 + abs((area.x2-area.x1)*hpos), area.y2, color);  
}

void Screen::drawTextInArea(Area area, TextPosition textPosition, boolean eraseFirst, TextSize textSize, uint16_t color, const char *text) {
  
  if (eraseFirst && !area.transparent) {
    fillArea(area, area.bgColor);
  }

  // ToDo: 
  // - generate fonts for SMALL and LARGE and include them
  switch (textSize) {
    case TEXTSIZE_SMALL:
      _tft->setFont(&OxygenMono_Regular8pt7b);
      break;
    case TEXTSIZE_MEDIUM:
      _tft->setFont(&OxygenMono_Regular8pt7b);
      break;
    case TEXTSIZE_LARGE:
      _tft->setFont(&OxygenMono_Regular8pt7b);
      break;
    default:
      _tft->setFont(&OxygenMono_Regular8pt7b);
      break;    
  }

  _tft->getTextBounds(text, 100, 100, &_dx, &_dy, &_dw, &_dh);

  float heightManipulator = 0;
  heightManipulator = _dh-100+_dy;

  int x = 0;
  int y = 0;


  switch (textPosition) {
        case TEXTPOSITION_HCENTER_VCENTER:
          x = static_cast<int>(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = static_cast<int>(area.y1+((area.y2-area.y1)/2)+(_dh/2))-2;
          break;
        case TEXTPOSITION_LEFT_VCENTER:
          x = area.x1+1;
          y = static_cast<int>(area.y1+((area.y2-area.y1)/2)+(_dh/2))-2;
          break;
        case TEXTPOSITION_RIGHT_VCENTER:
          x = static_cast<int>(area.x2-_dw-(_dx-100))-2;
          y = static_cast<int>(area.y1+((area.y2-area.y1)/2)+(_dh/2))-2;
          break;

        case TEXTPOSITION_HCENTER_TOP:
          x = static_cast<int>(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = static_cast<int>(area.y1+_dh+(heightManipulator/(heightManipulator > 2 ? -2 : 1)));          
          break;
        case TEXTPOSITION_LEFT_TOP:
          x = area.x1+1;
          y = abs(area.y1+_dh+(heightManipulator/(heightManipulator > 2 ? -2 : 1)));
          break;
        case TEXTPOSITION_RIGHT_TOP:
          x = area.x2-_dw-(_dx-100)-2;
          y = area.y1+_dh+(heightManipulator/(heightManipulator > 2 ? -2 : 1));
          break;

        case TEXTPOSITION_HCENTER_BOTTOM:
          x = static_cast<int>(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = static_cast<int>(area.y2-(_dh/2))+(heightManipulator > 2 ? 1 : 0);
                  
          break;
        case TEXTPOSITION_LEFT_BOTTOM:
          x = area.x1+1;
          y = static_cast<int>(area.y2-(_dh/2))+(heightManipulator > 2 ? 1 : 0);
          break;
        case TEXTPOSITION_RIGHT_BOTTOM:
          x = static_cast<int>(area.x2-_dw-(_dx-100))-2;
          y = static_cast<int>(area.y2-(_dh/2))+(heightManipulator > 2 ? 1 : 0);
          break;
        default:
          break;
  }
  _tft->setCursor(x, (y>area.y2 ? area.y2 : y));

  _tft->setTextColor(color);
  _tft->print(text);
}


void Screen::drawLine(int x1, int y1, int x2, int y2, uint16_t color) {
  _tft->drawLine(x1, y1, x2, y2, color);
}

void Screen::drawPixel(int x, int y, uint16_t color) {
  _tft->drawPixel(x, y, color);
}

void Screen::loadingScreen(float percent) {
  if (percent == 0.0) {
    fillArea(AREA_SCREEN, C_BLACK);
    drawTextInArea(AREA_CONTENT, TEXTPOSITION_HCENTER_VCENTER, false, TEXTSIZE_MEDIUM, C_WHITE, "Loading samples");
    vr(AREA_LOADING_BAR, 0.0, C_WHITE);
    vr(AREA_LOADING_BAR, 1.0, C_WHITE);
    hr(AREA_LOADING_BAR, 0.0, C_WHITE);
    hr(AREA_LOADING_BAR, 1.0, C_WHITE);
  } else {
    vr(AREA_LOADING_BAR, percent, C_WHITE);
  }
}

uint16_t Screen::RGBtoColor(byte r, byte g, byte b) {
  uint16_t color = 0;
  byte tr = round(r / 8);
  byte tg = round(g / 4);
  byte tb = round(b / 8);
  color = color + (tr << 11);
  color = color + (tg << 5);
  color = color + tb;  
  return color;
};


/*

// -------------------------------------------------------------------------------------------
// Sequencer Stuff
// -------------------------------------------------------------------------------------------


void Screen::initializeGrid(Pattern *pattern) {
  _pattern = pattern;

  _tft->fillScreen(C_BLACK);
  _tft->setTextColor(C_WHITE);
  _tft->setFont(&OxygenMono_Regular8pt7b);  

  _tft->drawFastHLine(0, 20, _tft->width(), C_GRID_DARK);
  _tft->drawFastHLine(0, 45, _tft->width(), C_GRID_DARK);
  _tft->drawFastHLine(0, 70, _tft->width(), C_GRID_DARK);

  _tft->drawFastHLine(0, 145, _tft->width(), C_GRID_DARK);
  _tft->drawFastHLine(0, 170, _tft->width(), C_GRID_DARK);
  _tft->drawFastHLine(0, 195, _tft->width(), C_GRID_DARK);
  _tft->drawFastHLine(0, 220, _tft->width(), C_GRID_DARK);

  _tft->drawFastVLine( 0, 20, _tft->height()-40, C_GRID_DARK);
  _tft->drawFastVLine(39, 20, _tft->height()-39, C_GRID_BRIGHT);
  _tft->drawFastVLine(109, 20, _tft->height()-40, C_GRID_DARK);
  _tft->drawFastVLine(179, 20, _tft->height()-40, C_GRID_DARK);
  _tft->drawFastVLine(249, 20, _tft->height()-40, C_GRID_DARK);  
  _tft->drawFastVLine(319, 20, _tft->height()-40, C_GRID_DARK);   

  _tft->drawFastHLine(0, 95, _tft->width(), C_GRID_BRIGHT);
  _tft->drawFastHLine(0, 120, _tft->width(), C_GRID_BRIGHT);
}

// position = current position on highlighted row
// patternLength = complete length of pattern (max 64)
void Screen::drawTrackerAtPosition(uint16_t position, Pattern *p, bool editMode, byte highlightEvery) {
  boolean highlight = false;
  for (int r = 0; r<8; r++) {
    if (position-3+r >= 0 && position-3+r < p->getPatternLength()){
      sprintf(_cBuff3, "%03d", position-3+r);
      
      highlight = (((position-3+r)%highlightEvery)==0);

      _drawGridCell(0, r, _cBuff3, highlight ? C_GRID_DARK : C_BLACK);
      _drawGridCell(1, r, p->getSampleAt(1, position-3+r).displayText, highlight ? C_GRID_DARK : C_BLACK);
      _drawGridCell(2, r, p->getSampleAt(2, position-3+r).displayText, highlight ? C_GRID_DARK : C_BLACK);      
      _drawGridCell(3, r, p->getSampleAt(3, position-3+r).displayText, highlight ? C_GRID_DARK : C_BLACK);      
      _drawGridCell(4, r, p->getSampleAt(4, position-3+r).displayText, highlight ? C_GRID_DARK : C_BLACK);      

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


void Screen::drawCursorAtColumn(byte column) {
  _tft->drawFastHLine(0, 95, _tft->width(), C_GRID_BRIGHT);
  _tft->drawFastHLine(0, 120, _tft->width(), C_GRID_BRIGHT);
  
  _tft->drawFastHLine(39+((column-1)*70), 95, 70, C_WHITE);
  _tft->drawFastHLine(39+((column-1)*70), 120, 70, C_WHITE);
};


void Screen::_drawEmptyRow(uint16_t row) {
  _drawGridCell(0, row, "", C_BLACK);
  _drawGridCell(1, row, "", C_BLACK);
  _drawGridCell(2, row, "", C_BLACK);
  _drawGridCell(3, row, "", C_BLACK); 
  _drawGridCell(4, row, "", C_BLACK);   
}


void Screen::_drawGridCell(int col, int row, const char *text, uint16_t bgColor) {
  _y = 21 + (row*25);

  if (col == 0) {
    _x = 0;  
    _tft->fillRect(_x, _y, 39, 24, bgColor);
    drawText(text, 4, _y+18);
  } else {
    _x = 40+(col-1)*70;
    _tft->fillRect(_x, _y, 69, 24, bgColor);
    drawText(text, _x+15, _y+18);
  }
} 


void Screen::drawBPM(float bpm) {
  _tft->setTextColor(C_WHITE);
  _tft->setFont(&OxygenMono_Regular8pt7b);
  _tft->fillRect(0, 221, 160, 19, C_BLACK);

  sprintf(_cBuff10, "BPM: %g", bpm);
  _tft->setCursor(4, 239);
  _tft->print(_cBuff10);
}


void Screen::drawExtMemPercentage(byte percent) {
  _tft->fillRect(219, 1, 100, 1, C_MEMORY_FREE);
  _tft->fillRect(219, 1, percent, 1, C_MEMORY_USED);
};

*/

void Screen::drawText(const char *text,int x, int y, uint16_t color, const GFXfont *font) {
  _tft->setFont(font);  
  _tft->setCursor(x, y);
  _tft->setTextColor(color);
  _tft->print(text);
}

void Screen::drawText(const char *text,int x, int y) {
  _tft->setTextColor(C_WHITE);
  _tft->setCursor(x, y);
  _tft->print(text);
}