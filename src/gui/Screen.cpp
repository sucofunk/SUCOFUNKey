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
  // clear and fill the wave form buffer
//  _clearWFB(_centerLineY);
  //AREA_SCREEN = { 0, 0, _tft->width(), _tft->height() };
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
  _tft->drawLine(area.x1, area.y1 + floor((area.y2-area.y1)*vpos), area.x2, area.y1 + floor((area.y2-area.y1)*vpos), color);  
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
          x = floor(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = floor(area.y1+((area.y2-area.y1)/2)+(_dh/2))-2;
          break;
        case TEXTPOSITION_LEFT_VCENTER:
          x = area.x1+1;
          y = floor(area.y1+((area.y2-area.y1)/2)+(_dh/2))-2;
          break;
        case TEXTPOSITION_RIGHT_VCENTER:
          x = floor(area.x2-_dw-(_dx-100))-2;
          y = floor(area.y1+((area.y2-area.y1)/2)+(_dh/2))-2;
          break;

        case TEXTPOSITION_HCENTER_TOP:
          x = floor(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = floor(area.y1+_dh+(heightManipulator/(heightManipulator > 2 ? -2 : 1)));          
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
          x = floor(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = floor(area.y2-(_dh/2))+(heightManipulator > 2 ? 1 : 0);
                  
          break;
        case TEXTPOSITION_LEFT_BOTTOM:
          x = area.x1+1;
          y = floor(area.y2-(_dh/2))+(heightManipulator > 2 ? 1 : 0);
          break;
        case TEXTPOSITION_RIGHT_BOTTOM:
          x = floor(area.x2-_dw-(_dx-100))-2;
          y = floor(area.y2-(_dh/2))+(heightManipulator > 2 ? 1 : 0);
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


/*
void Screen::showRecordingScreen(String text) {
  _tft->fillScreen(C_BLACK);
  
  _tft->setFont(&OxygenMono_Regular8pt7b);
    
  _tft->getTextBounds("RECORDING", 100, 100, &_dx, &_dy, &_dw, &_dh);
  
  _tft->setTextColor(C_WHITE);
  _tft->setCursor(floor((_tft->width()/2)-(_dw/2)), floor((_tft->height()/2)));
  
  _tft->print("RECORDING");  

  _tft->fillCircle((_tft->width()/2)-(_dw/2)-20, floor((_tft->height()/2))-4, 6, C_RECORDING);
}


void Screen::drawSampler() {
  _tft->fillScreen(C_BLACK);
  _tft->setFont(&OxygenMono_Regular8pt7b);
    
  _tft->getTextBounds("SAMPLER", 100, 100, &_dx, &_dy, &_dw, &_dh);

  _tft->setCursor(_tft->width()/2-_dw/2, _dh+5);
  _tft->setTextColor(C_WHITE);
  _tft->print("SAMPLER"); 
}


void Screen::drawSampleBank(boolean sampleBankStatus[4][8], byte activeBank) {
    _tft->fillRect(37, 200, 241, 31, C_BLACK);

    uint16_t bankColor;

    int x;
    for (int i=1; i<=8; i++) {
      if (sampleBankStatus[activeBank-1][i-1]) {
        bankColor = C_SB_USED;
      } else {
        bankColor = C_SB_FREE;
      }

      x=i*30+1+27;
      _tft->fillRect(x-20, 201, 29, 29, bankColor);

      _tft->setTextColor(C_WHITE);
      
      _tft->getTextBounds((activeBank-1)*8+i, 100, 100, &_dx, &_dy, &_dw, &_dh);

      _tft->setCursor(x-20+(floor((29-_dw)/2)), 201+13 + round(_dh/2));
      _tft->print((activeBank-1)*8+i); 
    }

    _tft->setCursor(12, 220);

    if (activeBank == 1) {
      _tft->setTextColor(C_NAV_INACTIVE);
    } else {
      _tft->setTextColor(C_NAV_ACTIVE);      
    }
    _tft->print("<");

    if (activeBank == 4) {
      _tft->setTextColor(C_NAV_INACTIVE);
    } else {
      _tft->setTextColor(C_NAV_ACTIVE);
    }

    _tft->setCursor(295, 220);

    _tft->print(">");    
}


void Screen::hideSampleBank() {
  _tft->fillRect(0, 200, 319, 40, C_BLACK);  
}


void Screen::drawSampleSelector(byte s) {  
  int x = s*30+7;
  _tft->drawRect(x, 200, 31, 31, C_NAV_ACTIVE);
}

void Screen::drawSamplerLiveMode() {
    drawSampler();
}


// ---------------------------------------------------------------------
//  Sample Waveform Display Operations
// ---------------------------------------------------------------------

void Screen::selectNewSample(const char *filename) {
      _tft->fillScreen(C_BLACK);

      _trimPositionStartPixel = 0;
      _trimPositionEndPixel = 319;

      // draw sample waveform
      drawRAWMonoSample44(filename);

      _drawTrimMarker(true, true);
      drawSampleEditSubmenu(1);
}


void Screen::drawSampleEditSubmenu(byte activeItem) {

  if (activeItem==1) {
    _tft->setTextColor(C_TRIM_START);
    _activeTrimmer = 1;
  } else {
    _tft->setTextColor(C_NAV_INACTIVE);
  }

  _tft->setCursor(105, 180);
  _tft->print("START");    

  if (activeItem==2) {
    _tft->setTextColor(C_TRIM_END);
    _activeTrimmer = 2;
  } else {
    _tft->setTextColor(C_NAV_INACTIVE);
  }  

  _tft->setCursor(190, 180);
  _tft->print("END");    
  
}

void Screen::drawRAWMonoSample44(const char *filename) {
    _clearWFB(_centerLineY);
    int heightDivisor = 32767/(_magnitudeHeight/2);
    
    File sample;

    if (SD.exists(filename)) {
      sample = SD.open(filename);

      if (!sample) {
        Serial.println("unable to open sample file");
        return;
      }      
    } else {
      // sample not available
      return;
    }

    long fsize = sample.size();

    long pxIterations = floor(fsize/2/320);

    // 44100hz -> 44100 values per second
    // -> 44.1 values per ms
    // pxIterations / 44.1 -> ms per pixel (for trim marker)
    Serial.print("pxIterations::");
    Serial.println(pxIterations);

    if (pxIterations > 45) {
      _pixelToMsFactorIdentifier = true;
      _pixelToMsFactor = round(pxIterations/44.1);  
    } else {
      _pixelToMsFactorIdentifier = false;
      _pixelToMsFactor = pxIterations;
    }
    
    // happens if available sample size < screen width
    if (pxIterations == 0) pxIterations = 1;
    
    int16_t peak;

    long tempMin;
    long tempMax;

    for (int c=0; c<320; c++) {
      tempMin = 0;
      tempMax = 0;      

      for (long i=0; i<pxIterations; i++) {
        if (sample.read(&peak, sizeof(peak)) != -1) {  
          if (peak < tempMin) {
            tempMin = peak;  
          }
          if (peak > tempMax) {
            tempMax = peak;  
          }
        } 
      }

      tempMin = tempMin / heightDivisor;
      tempMin = floor(tempMin);
      tempMin = abs(tempMin);
      
      tempMax = tempMax / heightDivisor;
      tempMax = floor(tempMax);

      // save waveform visualization in _wfb
      _wfb[c][0] = c+1;
      _wfb[c][1] = c+1;
      _wfb[c][2] = _centerLineY + tempMin;
      _wfb[c][3] = _centerLineY - tempMax; 
    }

    sample.close();

    redrawRAWMonoSample44();    
}

// delete wave form buffer array -> set everything to center/zero line
void Screen::_clearWFB(int centerLineY) { 
  for (int i=0; i<320; i++) {
      _wfb[i][0] = i+1;
      _wfb[i][1] = i+1;
      _wfb[i][2] = centerLineY;
      _wfb[i][3] = centerLineY;

      _wfbz[i][0] = i+1;
      _wfbz[i][1] = i+1;      
      _wfbz[i][2] = centerLineY;            
      _wfbz[i][3] = centerLineY;
  }  
}

void Screen::redrawRAWMonoSample44() {          
    _tft->fillRect(0, _centerLineY-_magnitudeHeight/2, 320, _magnitudeHeight, C_BLACK);
    //Rahmen entfernen? Style!?!
    //_tft->drawRect(0,_centerLineY-_magnitudeHeight/2, 320, _magnitudeHeight, C_WHITE);

    _tft->setFont(&OxygenMono_Regular8pt7b);    
    _tft->getTextBounds("Trim Sample", 100, 100, &_dx, &_dy, &_dw, &_dh);
    _tft->setCursor(_tft->width()/2-_dw/2, _dh+5);
    _tft->setTextColor(C_WHITE);
    _tft->print("Trim Sample");


    // draw trim marker "below" waveform
    _drawTrimMarker(true, true);

    // draw zero line
    _tft->drawFastHLine(0, _centerLineY, 320, C_WHITE);

    // draw waveform
    for (int i=0; i<320; i++) {       
      _tft->drawLine(_wfb[i][0], _wfb[i][2], _wfb[i][1], _wfb[i][3], C_WHITE);
    }      
    
}


// awaits pixel positions
// draw == true -> draw marker
// draw == false -> delete marker
void Screen::_drawTrimMarker(bool draw, bool updateBoth) {  
  if (draw) { 
    if (_activeTrimmer==1 || updateBoth) {  
      // draw start marker
      _tft->drawFastVLine(_trimPositionStartPixel, _centerLineY-_magnitudeHeight/2, _magnitudeHeight, C_TRIM_START);
  
      // repaint waveform on top of marker
      if (_trimPositionStartPixel > 0) {
        _tft->drawLine(_wfb[_trimPositionStartPixel-1][0], _wfb[_trimPositionStartPixel-1][2], _wfb[_trimPositionStartPixel-1][1], _wfb[_trimPositionStartPixel-1][3], C_WHITE);      
      }
    }
 
    if (_activeTrimmer==2 || updateBoth) {  
      // draw end marker 
      _tft->drawFastVLine(_trimPositionEndPixel, _centerLineY-_magnitudeHeight/2, _magnitudeHeight, C_TRIM_END);

      // repaint waveform on top of marker
      _tft->drawLine(_wfb[_trimPositionEndPixel-1][0], _wfb[_trimPositionEndPixel-1][2], _wfb[_trimPositionEndPixel-1][1], _wfb[_trimPositionEndPixel-1][3], C_WHITE);
    }
  } 
  else { 
    if (_activeTrimmer==1 || updateBoth) {  
      // remove start marker
      _tft->drawFastVLine(_trimPositionStartPixel, _centerLineY-_magnitudeHeight/2, _magnitudeHeight, C_BLACK);
      // repaint waveform
      if (_trimPositionStartPixel > 0) {
        _tft->drawLine(_wfb[_trimPositionStartPixel-1][0], _wfb[_trimPositionStartPixel-1][2], _wfb[_trimPositionStartPixel-1][1], _wfb[_trimPositionStartPixel-1][3], C_WHITE);
      }
    }

    if (_activeTrimmer==2 || updateBoth) {  
      // remove end marker
      _tft->drawFastVLine(_trimPositionEndPixel, _centerLineY-_magnitudeHeight/2, _magnitudeHeight, C_BLACK);

      // repaint waveform
      _tft->drawLine(_wfb[_trimPositionEndPixel-1][0], _wfb[_trimPositionEndPixel-1][2], _wfb[_trimPositionEndPixel-1][1], _wfb[_trimPositionEndPixel-1][3], C_WHITE);
    }
  }
  
}


void Screen::updateTrimPosition(int encoderValue) { 
  bool updateBoth = false;
  // removes old trim marker
  _drawTrimMarker(false, updateBoth);

  // update marker positions  
  if (_activeTrimmer==1) {
    _trimPositionStartPixel += encoderValue;
    if (_trimPositionStartPixel < 0) { 
      _trimPositionStartPixel = 0;      
    }    
    if (_trimPositionStartPixel > 318) { 
      _trimPositionStartPixel = 318;
    }    
    if (_trimPositionStartPixel >= _trimPositionEndPixel) {
      _trimPositionEndPixel = _trimPositionStartPixel + 1 ;
      updateBoth = true;
    }
    
  } 
  
  if (_activeTrimmer==2) {
    _trimPositionEndPixel += encoderValue;

    if (_trimPositionEndPixel < 2) {
      _trimPositionEndPixel = 2;
    }

    if (_trimPositionEndPixel <= _trimPositionStartPixel) { 
      _trimPositionStartPixel = _trimPositionEndPixel-1;
      updateBoth = true;
    }
        
    if (_trimPositionEndPixel > 319) { 
      _trimPositionEndPixel = 319;
    }    
  }

  // draw updated/moved marker
  _drawTrimMarker(true, updateBoth);
};

long Screen::getTrimmerStartPositionMs() {
  Serial.print("PixelToMsFactor::");
  Serial.println(_pixelToMsFactor);
  return (_trimPositionStartPixel) * _pixelToMsFactor;
}

long Screen::getTrimmerEndPositionMs() {
  return (_trimPositionEndPixel) * _pixelToMsFactor;
}

bool Screen::useMs() {
  return _pixelToMsFactorIdentifier;
}


void Screen::drawHint(const char *hint) {
  _tft->fillRect(0, 165, 320, 29, C_BLACK);

  _tft->setTextColor(C_WHITE);
  _tft->setCursor(1, 180);
  _tft->print(hint);    
}
*/

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




