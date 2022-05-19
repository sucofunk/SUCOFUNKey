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
   
#include "RecorderScreen.h"

RecorderScreen::RecorderScreen(){};

RecorderScreen::RecorderScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _sfsio = sfsio;
    _audioResources = audioResources;
    _peakArea = {static_cast<int>(_screen->AREA_SCREEN.x2*0.5-105), _screen->AREA_SCREEN.y2-50, static_cast<int>(_screen->AREA_SCREEN.x2*0.5+105), _screen->AREA_SCREEN.y2-43, false, _screen->C_BLACK};
    _textArea = {_screen->AREA_SCREEN.x1, static_cast<int>(_screen->AREA_SCREEN.y2*0.5-20), _screen->AREA_SCREEN.x2, static_cast<int>(_screen->AREA_SCREEN.y2*0.5), false, _screen->C_BLACK};   
}

void RecorderScreen::showRecorderScreen() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawTextInArea(_textArea, Screen::TEXTPOSITION_HCENTER_TOP, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,     "ready to               ");
    _screen->drawTextInArea(_textArea, Screen::TEXTPOSITION_HCENTER_TOP, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_RECORDING, "                 record");

    _screen->vr(_peakArea, 0.0, _screen->C_WHITE); // peak meter left border
    _screen->vr(_peakArea, 1.0, _screen->C_WHITE); // peak meter right border
}

void RecorderScreen::showRecorderScreenRecording() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawTextInArea(_textArea, Screen::TEXTPOSITION_HCENTER_TOP, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,     "recording...");

    _screen->vr(_peakArea, 0.0, _screen->C_WHITE); // peak meter left border
    _screen->vr(_peakArea, 1.0, _screen->C_WHITE); // peak meter right border
}


void RecorderScreen::drawInputPeakMeter(float peak) {
    for (int i=0; i<peak*200; i=i+2) {        
        if (i < 160) {
            _screen->drawPixel(_peakArea.x1+5+i, _peakArea.y1+4, _screen->C_PEAK_OK);
        } else {
            _screen->drawPixel(_peakArea.x1+5+i, _peakArea.y1+4, i < 180 ? _screen->C_PEAK_MUCH : _screen->C_PEAK_OVER);
        }        
    }

    for (int i=peak*200; i<=200; i=i+2) {        
        _screen->drawPixel(_peakArea.x1+5+i, _peakArea.y1+4, _screen->C_BLACK);
    }
}

