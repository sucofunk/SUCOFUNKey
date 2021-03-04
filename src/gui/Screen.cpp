#include <Arduino.h>
#include "Screen.h"
#include <SD.h>
//#include "../context/sampler/Sampler.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "fonts/OxygenMono_Regular8pt7b.h" // https://fonts.google.com/specimen/Oxygen+Mono

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

void Screen::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  _tft->drawFastHLine(x, y, w, color);
}

void Screen::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  _tft->drawFastVLine(x, y, h, color);
}

void Screen::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  _tft->fillRect(x, y, w, h, color);
}