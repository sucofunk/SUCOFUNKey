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

#include "SamplerScreen.h"

SamplerScreen::SamplerScreen(){};

SamplerScreen::SamplerScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources) {
    _keyboard = keyboard;
    _screen = screen;
    _fsio = fsio;
    _sfsio = sfsio;
    _audioResources = audioResources;
    zeroAxisY = (_screen->AREA_CONTENT.y2 - _screen->AREA_CONTENT.y1)/2+_screen->AREA_CONTENT.y1;
    _sampleSelector = SampleSelector(_keyboard, _screen, _fsio);
}

void SamplerScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
    switch(_activeComponent) {
        case 1: 
            // start prelistening selected sample from sample library
            if (event.pressed && event.index == _keyboard->PLAY) {
                _audioResources->playSdRaw.play(_fsio->getSelectedSamplePathFromSD());
            } 
            
            // stop prelistening selected sample from sample library
            if (!event.pressed && event.index == _keyboard->PLAY) {
                _audioResources->playSdRaw.stop();
            } 

            _sampleSelector.handleEvent(event);
            break;
        default:
            break;
    }
}

void SamplerScreen::showEmptyScreen() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    Screen::Area text1 = {_screen->AREA_SCREEN.x1, static_cast<int>(_screen->AREA_SCREEN.y2*0.5-20), _screen->AREA_SCREEN.x2, static_cast<int>(_screen->AREA_SCREEN.y2*0.5), false, _screen->C_BLACK};
    Screen::Area text2 = {text1.x1, text1.y2+1, text1.x2, text1.y2+21, false, _screen->C_BLACK};    

    _screen->drawTextInArea(text1, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,     "Select a sample");
    _screen->drawTextInArea(text2, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,     "or hit                ");
    _screen->drawTextInArea(text2, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_RECORDING, "           Record");
    
    if (!_screen->isBacklightOn()) _screen->fadeBacklightIn(10);
};


void SamplerScreen::clearScreen() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);
};

void SamplerScreen::showSampleInfo(byte sampleId72, float volumeScaleFactor) {
    if (_activeComponent == 1) _hideSampleSelector();

    //int zeroAxisY = (_screen->AREA_CONTENT.y2 - _screen->AREA_CONTENT.y1)/2+_screen->AREA_CONTENT.y1;
    _screen->fillArea(_screen->AREA_HEADLINE, _screen->C_BLACK);
    _screen->fillArea(_screen->AREA_CONTENT, _screen->C_BLACK);

    if (sampleId72 == 72) {
        _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_TOP, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Latest Recording");
    } else {

        // ToDo: change to sample name
        
        sprintf(_cBuff20, "Sample %d", sampleId72+1);
        _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_TOP, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff20);
    }

    // draw zero line
    _screen->drawLine(0, zeroAxisY, 320, zeroAxisY, _screen->C_WHITE);

    // draw waveform
    for (int i=0; i<320; i++) {        
      _screen->drawLine(i, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][i][0]*volumeScaleFactor), i, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][i][1]*volumeScaleFactor), (i >= _sfsio->waveFormBufferLength[sampleId72] ? _screen->C_LIGHTGREY : _screen->C_WHITE ));
    }

    if (!_screen->isBacklightOn()) _screen->fadeBacklightIn(10);
}

void SamplerScreen::showSampleInfo(byte bank0, byte sampleId0, float volumeScaleFactor) {
    showSampleInfo((bank0*24)+sampleId0, volumeScaleFactor);    
}; 


void SamplerScreen::setBottomMenu(BottomMenu bottomMenu) {
    _bottomMenu = bottomMenu;    
}


void SamplerScreen::drawTrimMarker(int trimMarkerStartPosition, int trimMarkerEndPosition, byte sampleId72, float volumeScaleFactor) {
    // Start Marker
    if (trimMarkerStartPosition > 0){
        _screen->drawLine(trimMarkerStartPosition-1, _screen->AREA_CONTENT.y1, trimMarkerStartPosition-1, _screen->AREA_CONTENT.y2, _screen->C_BLACK);
        _screen->drawLine(trimMarkerStartPosition-1, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][trimMarkerStartPosition-1][0]*volumeScaleFactor), trimMarkerStartPosition-1, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][trimMarkerStartPosition-1][1]*volumeScaleFactor), _screen->C_WHITE);
    }
    if (trimMarkerStartPosition < 319){
        _screen->drawLine(trimMarkerStartPosition+1, _screen->AREA_CONTENT.y1, trimMarkerStartPosition+1, _screen->AREA_CONTENT.y2, _screen->C_BLACK);    
        _screen->drawLine(trimMarkerStartPosition+1, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][trimMarkerStartPosition+1][0]*volumeScaleFactor), trimMarkerStartPosition+1, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][trimMarkerStartPosition+1][1]*volumeScaleFactor), _screen->C_WHITE);
    }

    _screen->drawLine(trimMarkerStartPosition, _screen->AREA_CONTENT.y1, trimMarkerStartPosition, _screen->AREA_CONTENT.y2, _screen->C_TRIM_START);
    _screen->drawLine(trimMarkerStartPosition, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][trimMarkerStartPosition][0]*volumeScaleFactor), trimMarkerStartPosition, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][trimMarkerStartPosition][1]*volumeScaleFactor), _screen->C_LIGHTGREY);

    // End Marker
    if (trimMarkerEndPosition > 0){
        _screen->drawLine(trimMarkerEndPosition-1, _screen->AREA_CONTENT.y1, trimMarkerEndPosition-1, _screen->AREA_CONTENT.y2, _screen->C_BLACK);
        _screen->drawLine(trimMarkerEndPosition-1, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][trimMarkerEndPosition-1][0]*volumeScaleFactor), trimMarkerEndPosition-1, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][trimMarkerEndPosition-1][1]*volumeScaleFactor), _screen->C_WHITE);
    }

    if (trimMarkerEndPosition < 319){
        _screen->drawLine(trimMarkerEndPosition+1, _screen->AREA_CONTENT.y1, trimMarkerEndPosition+1, _screen->AREA_CONTENT.y2, _screen->C_BLACK);    
        _screen->drawLine(trimMarkerEndPosition+1, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][trimMarkerEndPosition+1][0]*volumeScaleFactor), trimMarkerEndPosition+1, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][trimMarkerEndPosition+1][1]*volumeScaleFactor), _screen->C_WHITE);
    }

    _screen->drawLine(trimMarkerEndPosition, _screen->AREA_CONTENT.y1, trimMarkerEndPosition, _screen->AREA_CONTENT.y2, _screen->C_TRIM_END);
    _screen->drawLine(trimMarkerEndPosition, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][trimMarkerEndPosition][0]*volumeScaleFactor), trimMarkerEndPosition, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][trimMarkerEndPosition][1]*volumeScaleFactor), _screen->C_LIGHTGREY);
}

void SamplerScreen::drawTrimMarker(int trimMarkerStartPosition, int trimMarkerEndPosition, byte bank0, byte sampleId0, float volumeScaleFactor) {
    drawTrimMarker(trimMarkerStartPosition, trimMarkerEndPosition, (bank0*24)+sampleId0, volumeScaleFactor);
}

void SamplerScreen::drawTrimMarkerOffsets(int startOffset, int endOffset) {

        sprintf(_cBuff20, "%d", startOffset);
        _screen->drawTextInArea(_screen->AREA_HEADLINE_STARTOFFSET, _screen->TEXTPOSITION_LEFT_TOP, true, _screen->TEXTSIZE_MEDIUM, true, _screen->C_TRIM_START, _cBuff20);

        sprintf(_cBuff20, "%d", endOffset);
        _screen->drawTextInArea(_screen->AREA_HEADLINE_ENDOFFSET, _screen->TEXTPOSITION_RIGHT_TOP, true, _screen->TEXTSIZE_MEDIUM, true, _screen->C_TRIM_END, _cBuff20);


};


void SamplerScreen::removeTrimMarker(int position, byte sampleId72, float volumeScaleFactor) {
    _screen->drawLine(position, _screen->AREA_CONTENT.y1, position, _screen->AREA_CONTENT.y2, _screen->C_BLACK);
    _screen->drawLine(position, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][position][0]*volumeScaleFactor), position, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][position][1]*volumeScaleFactor), _screen->C_WHITE);
}

void SamplerScreen::showSlotSelectionHint() {
    _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WARNING, "Select a free slot");
};

void SamplerScreen::showSavingMessage() {
    _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WARNING, "Saving sample..");
};

void SamplerScreen::drawPlayerPosition(int x, int start, int end) {
    if (!isPlayerPositionVisible) isPlayerPositionVisible = true;

    if (x < start) x = start;
    if (x > end) x = end;
    _screen->drawLine(start, zeroAxisY, x, zeroAxisY, _screen->C_PLAYER_POSITION);

};

void SamplerScreen::resetPlayerPosition() {
    if (isPlayerPositionVisible) {
        isPlayerPositionVisible = false;
        _screen->drawLine(0, zeroAxisY, 320, zeroAxisY, _screen->C_WHITE);        
    }    
};


void SamplerScreen::showNoSampleInfo() {
    _activeComponent = 0;
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    Screen::Area text1 = {_screen->AREA_SCREEN.x1, static_cast<int>(_screen->AREA_SCREEN.y2*0.5-20), _screen->AREA_SCREEN.x2, static_cast<int>(_screen->AREA_SCREEN.y2*0.5), false, _screen->C_BLACK};
    Screen::Area text2 = {text1.x1, text1.y2+1, text1.x2, text1.y2+21, false, _screen->C_BLACK};    

    _screen->drawTextInArea(text1, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,  "No Sample here");
    _screen->drawTextInArea(text2, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,  "hit               to load one");
    _screen->drawTextInArea(text2, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_ORANGE, "   switch                   ");
    
    if (!_screen->isBacklightOn()) _screen->fadeBacklightIn(10);
};

void SamplerScreen::transitionToSelection() {
    _activeComponent = 1;
//    _screen->clearAreaLTR(_screen->AREA_SCREEN, _screen->C_STARTUP_BG, 3);
    _screen->fadeBacklightOut(1);
    _drawSampleSelector();        
    _screen->fadeBacklightIn(10);    
}

void SamplerScreen::_drawSampleSelector() {
    _sampleSelector.drawSampleSelector();    
}

void SamplerScreen::_hideSampleSelector() {
    _activeComponent = 0;
    _screen->fadeBacklightOut(1);
    clearScreen();
}