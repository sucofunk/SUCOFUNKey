/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2026 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#include <Arduino.h>
#include "Screen.h"
#include <SD.h>

#include "fonts/OxygenMono_Regular8pt7b.h" // https://fonts.google.com/specimen/Oxygen+Mono
#include "fonts/BaiJamjureeRegularMonoDigits8pt7b.h" // https://fonts.google.com/specimen/Bai+Jamjuree
#include "fonts/BaiJamjuree_Medium5pt7b.h" // https://fonts.google.com/specimen/Bai+Jamjuree

#ifdef ENABLE_SCREEN_STREAMING
#include "../helper/screen-streaming/ScreenStreaming.h"
#endif

#ifdef SCREEN_ILI9341
Screen::Screen(Adafruit_ILI9341 *tft, int BL_PIN, int BL_brightness) {
  _tft = tft;
  _BL_PIN = BL_PIN;
  _BL_brightness = BL_brightness;
}
#endif 

#ifdef SCREEN_ILI9341_DMA
Screen::Screen(ILI9341_t3n *tft, int BL_PIN, int BL_brightness) {
  _tft = tft;
  _BL_PIN = BL_PIN;
  _BL_brightness = BL_brightness;
}
#endif 

#ifdef SCREEN_ST7789
Screen::Screen(Adafruit_ST7789 *tft, int BL_PIN, int BL_brightness) {
  _tft = tft;
  _BL_PIN = BL_PIN;
  _BL_brightness = BL_brightness;
}  
#endif

void Screen::testBild(const char* text) {
    fillArea(AREA_SCREEN, C_BLACK);    
    drawTextInArea(AREA_SCREEN, TEXTPOSITION_HCENTER_VCENTER, false, TEXTSIZE_MEDIUM, false, C_WHITE, text);
}

void Screen::setBacklightBrightness(int brightness) {
  _BL_brightness = brightness;
  analogWrite(_BL_PIN, _BL_brightness); 
  
  if (brightness > 0) {
    _BL_on = true;
  } else {
    _BL_on = false;
  }
}

void Screen::fadeBacklightOut(int delayTime) {
  for (int b=_BL_brightness; b>0; b--) {
    analogWrite(_BL_PIN, b);
    delay(delayTime);
  } 
  _BL_on = false;
}

void Screen::fadeBacklightIn(int delayTime) {
  for (int b=0; b<=_BL_brightness; b++) {
    analogWrite(_BL_PIN, b);
    if (b % 4 == 0) { delay(delayTime); }    
  } 
  _BL_on = true;
}

boolean Screen::isBacklightOn() {
  return _BL_on;
}

#ifdef ENABLE_SCREEN_STREAMING
void Screen::setStreaming(ScreenStreaming* streaming) {
    _streaming = streaming;
}
#endif


void Screen::fillArea(Area area, uint16_t color) {
  int16_t w = area.x2-area.x1+(area.x1 == 0 ? 1 : 0);
  int16_t h = area.y2-area.y1+(area.x1 == 0 ? 1 : 0);
  _tft->fillRect(area.x1, area.y1, w, h, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logFillRect(area.x1, area.y1, w, h, color);
#endif
  area.bgColor = color;
  area.transparent = false;
}

void Screen::clearAreaLTR(Area area, uint16_t color, int delayTime) {  
  for (int x=area.x1; x<=area.x2; x++) {
    _tft->drawLine(x, area.y1, x, area.y2, color);
#ifdef ENABLE_SCREEN_STREAMING
    if (_streaming) _streaming->logVLine(x, area.y1, area.y2 - area.y1 + 1, color);
#endif
    delay(delayTime);
  }
  
  area.bgColor = color;
  area.transparent = false;
}

void Screen::clearAreaRTL(Area area, uint16_t color, int delayTime) {    
  for (int x=area.x2; x>=area.x1; x--) {
    _tft->drawLine(x, area.y1, x, area.y2, color);
#ifdef ENABLE_SCREEN_STREAMING
    if (_streaming) _streaming->logVLine(x, area.y1, area.y2 - area.y1 + 1, color);
#endif
    delay(delayTime);
  }
  
  area.bgColor = color;
  area.transparent = false;
}


// draws a vertical line at the in vpos defined percentage (0..1) of the height of the area
void Screen::hr(Area area, float vpos, uint16_t color) {
  int16_t y = area.y1 + static_cast<int>((area.y2-area.y1)*vpos);
  _tft->drawLine(area.x1, y, area.x2, y, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logHLine(area.x1, y, area.x2 - area.x1 + 1, color);
#endif
}

// draws a vertical line at the in hpos defined percentage (0..1) of the width of the area
void Screen::vr(Area area, float hpos, uint16_t color) {
  int16_t x = area.x1 + abs((area.x2-area.x1)*hpos);
  _tft->drawLine(x, area.y1, x, area.y2, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logVLine(x, area.y1, area.y2 - area.y1 + 1, color);
#endif
}

void Screen::drawTextInArea(Area area, TextPosition textPosition, boolean eraseFirst, TextSize textSize, boolean monoSpaced, uint16_t color, const char *text) {
  
  if (eraseFirst && !area.transparent) {
    fillArea(area, area.bgColor);
  }

  setTextSize(textSize, monoSpaced);

  _tft->getTextBounds(text, 0, 0, &_dx, &_dy, &_dw, &_dh);

  int x = 0;
  int y = 0;

  // offsets for different font-constellations: 
  //   (afg) -> full with "underhanging" part
  //   (ABC) or abc -> full, without the "underhanging" part
  //   not in switch below, as handled via _dh (aag -> half with "underhanging" part)
  //   (aaa -> half, no "underhanging" part)

  int ydBottom = 0;
  int ydTop = 1;
  int ydCentered = 0;

  if (textSize == TEXTSIZE_MEDIUM) {
  // medium font
    ydCentered = 7;
    switch (_dh) {
      case 14:
        break;
      case 11:
        ydBottom = -3;
        break;
      case 8:
        ydBottom = -3;
        if (_dy > 0) { 
          ydBottom = ydBottom - _dy; 
        } 
        break;
      default:
        break;
    }
  }

  if (textSize == TEXTSIZE_SMALL) {
  // small font
    ydCentered = 4;
    switch (_dh) {
      case 9:
        break;
      case 7:
        ydBottom = -2;
        break;
      case 5:
        ydBottom = -2;
        if (_dy > 0) { 
          ydBottom = ydBottom - _dy; 
        } 
        break;
      default:
        break;
    }
  }


  switch (textPosition) {
        case TEXTPOSITION_HCENTER_VCENTER:
          x = static_cast<int>(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = static_cast<int>(area.y1+((area.y2-area.y1)/2)-ydCentered+ydTop);        
          break;
        case TEXTPOSITION_LEFT_VCENTER:
          x = area.x1+1;
          y = static_cast<int>(area.y1+((area.y2-area.y1)/2)-ydCentered+ydTop);
          break;
        case TEXTPOSITION_RIGHT_VCENTER:
          x = static_cast<int>(area.x2-_dw-(_dx-1))-2;
          y = static_cast<int>(area.y1+((area.y2-area.y1)/2)-ydCentered+ydTop);
          break;
        case TEXTPOSITION_HCENTER_TOP:
          x = static_cast<int>(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = static_cast<int>(area.y1 + ydTop);
          break;
        case TEXTPOSITION_LEFT_TOP:
          x = area.x1+1;
          y = static_cast<int>(area.y1 + ydTop);
          break;
        case TEXTPOSITION_RIGHT_TOP:
          x = area.x2-_dw-(_dx-1)-2;
          y = static_cast<int>(area.y1 + ydTop);
          break;
        case TEXTPOSITION_HCENTER_BOTTOM:
          x = static_cast<int>(area.x1+((area.x2-area.x1)/2)-(_dw/2));
          y = static_cast<int>(area.y2 -_dh + ydBottom);
          break;
        case TEXTPOSITION_LEFT_BOTTOM:
          x = area.x1+1;
          y = static_cast<int>(area.y2 -_dh + ydBottom);
          break;
        case TEXTPOSITION_RIGHT_BOTTOM:
          x = static_cast<int>(area.x2-_dw-(_dx-1))-2;
          y = static_cast<int>(area.y2 -_dh + ydBottom);
          break;
        default:
          break;
  }
  _tft->setCursor(x, (y>area.y2 ? area.y2 : y));

  _tft->setTextColor(color);
  _tft->print(text);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logText(x, (y>area.y2 ? area.y2 : y), color, _currentFontId, text);
#endif
}


void Screen::drawLine(int x1, int y1, int x2, int y2, uint16_t color) {
  _tft->drawLine(x1, y1, x2, y2, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) {
    // Optimize: use VLINE for vertical lines (saves 2 bytes per command)
    if (x1 == x2) {
      int yStart = (y1 < y2) ? y1 : y2;
      int h = abs(y2 - y1) + 1;
      _streaming->logVLine(x1, yStart, h, color);
    } else if (y1 == y2) {
      // Horizontal line
      int xStart = (x1 < x2) ? x1 : x2;
      int w = abs(x2 - x1) + 1;
      _streaming->logHLine(xStart, y1, w, color);
    } else {
      _streaming->logLine(x1, y1, x2, y2, color);
    }
  }
#endif
}

void Screen::drawPixel(int x, int y, uint16_t color) {
  _tft->drawPixel(x, y, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logPixel(x, y, color);
#endif
}

void Screen::loadingScreen(float percent) {
  if (percent == 0.0) {
    fillArea(AREA_SCREEN, C_BLACK);
    drawTextInArea(AREA_CONTENT, TEXTPOSITION_HCENTER_VCENTER, false, TEXTSIZE_MEDIUM, false, C_WHITE, "Loading samples");
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
  // Note: custom font pointer - streaming with unknown font ID
}

void Screen::drawText(const char *text,int x, int y, TextSize textSize, uint16_t color) {
  setTextSize(textSize, false);
  _tft->setTextColor(color);
  _tft->setCursor(x, y);
  _tft->print(text);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logText(x, y, color, _currentFontId, text);
#endif
};

void Screen::drawText(const char *text,int x, int y) {
  _tft->setTextColor(C_WHITE);
  _tft->setCursor(x, y);
  _tft->print(text);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logText(x, y, C_WHITE, _currentFontId, text);
#endif
}


void Screen::setTextSize(TextSize textSize, boolean monoSpaced) {
  switch (textSize) {
    case TEXTSIZE_SMALL:
      if (monoSpaced) {
        // ToDo: create small font with monospacing, if needed one day.. currently it is the medium font
        _tft->setFont(&OxygenMono_Regular8pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 0; // FONT_OXYGEN_MONO
#endif
      } else {      
        _tft->setFont(&BaiJamjuree_Medium5pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 1; // FONT_BAI_JAMJUREE_MEDIUM_5PT
#endif
      }
      break;
    case TEXTSIZE_MEDIUM:
      if (monoSpaced) {
        _tft->setFont(&OxygenMono_Regular8pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 0; // FONT_OXYGEN_MONO
#endif
      } else {      
        _tft->setFont(&BaiJamjureeRegularMonoDigits8pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 2; // FONT_BAI_JAMJUREE_REGULAR_MONO_DIGITS
#endif
      }
      break;
    case TEXTSIZE_LARGE:
      // ToDo: create large font, if needed one day.. currently it is the medium font
      if (monoSpaced) {        
        _tft->setFont(&OxygenMono_Regular8pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 0; // FONT_OXYGEN_MONO
#endif
      } else {      
        _tft->setFont(&BaiJamjureeRegularMonoDigits8pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 2; // FONT_BAI_JAMJUREE_REGULAR_MONO_DIGITS
#endif
      }
      break;
    default:
      if (monoSpaced) {
        _tft->setFont(&OxygenMono_Regular8pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 0; // FONT_OXYGEN_MONO
#endif
      } else {      
        _tft->setFont(&BaiJamjureeRegularMonoDigits8pt7b);
#ifdef ENABLE_SCREEN_STREAMING
        _currentFontId = 2; // FONT_BAI_JAMJUREE_REGULAR_MONO_DIGITS
#endif
      }
      break;    
  }
};

void Screen::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  _tft->drawFastHLine(x, y, w, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logHLine(x, y, w, color);
#endif
}

void Screen::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  _tft->drawFastVLine(x, y, h, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logVLine(x, y, h, color);
#endif
}

void Screen::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  _tft->fillRect(x, y, w, h, color);
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logFillRect(x, y, w, h, color);
#endif
}

void Screen::drawCircle(int16_t x, int16_t y, int16_t r, boolean fill, uint16_t color) {
  if (fill) {
    _tft->fillCircle(x, y, r, color);
  } else {
    _tft->drawCircle(x, y, r, color);
  }
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logCircle(x, y, r, fill, color);
#endif
}

void Screen::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, boolean fill, uint16_t color) {
  if (fill) {
    _tft->fillTriangle(x0, y0, x1, y1, x2, y2, color);  
  } else {
    _tft->drawTriangle(x0, y0, x1, y1, x2, y2, color);
  }
#ifdef ENABLE_SCREEN_STREAMING
  if (_streaming) _streaming->logTriangle(x0, y0, x1, y1, x2, y2, fill, color);
#endif
};

void Screen::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  _tft->drawBitmap(x, y, bitmap, w, h, color);
  // Note: bitmap streaming not implemented - would need bitmap ID lookup
};

// --- Icons and helper functions ---
/*
uint64_t Screen::binary_string_to_uint64(const char *binary)
{
    uint64_t result = 0;

    for (int i = 0; i < 64; i++) {
        result <<= 1;

        if (binary[i] == '1') {
            result |= 1ULL;
        }
    }

    return result;
}
*/

int Screen::get_bit_at_position(uint64_t icon, uint8_t position)
{
    if (position > 63) {
        return 0; // false
    }

    return (icon >> position) & 1ULL;
}

void Screen::drawIconBy64Int(int x, int y, uint64_t icon, uint16_t color) {
  if (icon == 0) return;

  int bitPosition = 0;  
  for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            bitPosition = (7 - row) * 8 + (7 - col);
            if (get_bit_at_position(icon, bitPosition)) {
                drawPixel(x + col, y + row, color);
            }
        }
    }  
}
    