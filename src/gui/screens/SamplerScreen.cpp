#include "SamplerScreen.h"

SamplerScreen::SamplerScreen(){};

SamplerScreen::SamplerScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _sfsio = sfsio;
    _audioResources = audioResources;
    zeroAxisY = (_screen->AREA_CONTENT.y2 - _screen->AREA_CONTENT.y1)/2+_screen->AREA_CONTENT.y1;
}

void SamplerScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
}

void SamplerScreen::showEmptyScreen() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    Screen::Area text1 = {_screen->AREA_SCREEN.x1, static_cast<int>(_screen->AREA_SCREEN.y2*0.5-20), _screen->AREA_SCREEN.x2, static_cast<int>(_screen->AREA_SCREEN.y2*0.5), false, _screen->C_BLACK};
    Screen::Area text2 = {text1.x1, text1.y2+1, text1.x2, text1.y2+21, false, _screen->C_BLACK};    

    _screen->drawTextInArea(text1, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,     "Select a sample");
    _screen->drawTextInArea(text2, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE,     "or hit                ");
    _screen->drawTextInArea(text2, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_RECORDING, "           Record");
};


void SamplerScreen::showSampleInfo(byte sampleId72, float volumeScaleFactor) {
    //int zeroAxisY = (_screen->AREA_CONTENT.y2 - _screen->AREA_CONTENT.y1)/2+_screen->AREA_CONTENT.y1;

    _screen->fillArea(_screen->AREA_HEADLINE, _screen->C_BLACK);
    _screen->fillArea(_screen->AREA_CONTENT, _screen->C_BLACK);

    if (sampleId72 == 72) {
        _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_TOP, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Latest Recording");
    } else {
// ToDo: change to sample name
        sprintf(_cBuff20, "Sample %d", sampleId72);
        _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_TOP, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff20);
    }

    // draw zero line
    _screen->drawLine(0, zeroAxisY, 320, zeroAxisY, _screen->C_WHITE);

    // draw waveform
    for (int i=0; i<320; i++) {        
      _screen->drawLine(i, floor(zeroAxisY+_sfsio->waveFormBuffer[sampleId72][i][0]*volumeScaleFactor), i, floor(zeroAxisY-_sfsio->waveFormBuffer[sampleId72][i][1]*volumeScaleFactor), (i >= _sfsio->waveFormBufferLength[sampleId72] ? _screen->C_LIGHTGREY : _screen->C_WHITE ));
    }
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