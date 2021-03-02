#include "RecorderScreen.h"

RecorderScreen::RecorderScreen(){};

RecorderScreen::RecorderScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, SampleFSIO *sfsio, AudioResources *audioResources) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _sfsio = sfsio;
    _audioResources = audioResources;
    _peakArea = {_screen->AREA_SCREEN.x2*0.5-105, _screen->AREA_SCREEN.y2-50, _screen->AREA_SCREEN.x2*0.5+105, _screen->AREA_SCREEN.y2-43, false, _screen->C_BLACK};
    _textArea = {_screen->AREA_SCREEN.x1, _screen->AREA_SCREEN.y2*0.5-20, _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.y2*0.5, false, _screen->C_BLACK};   
}

void RecorderScreen::showRecorderScreen() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawTextInArea(_textArea, Screen::TEXTPOSITION_HCENTER_TOP, false, Screen::TEXTSIZE_MEDIUM, _screen->C_WHITE,     "ready to        ");
    _screen->drawTextInArea(_textArea, Screen::TEXTPOSITION_HCENTER_TOP, false, Screen::TEXTSIZE_MEDIUM, _screen->C_RECORDING, "         record");

    _screen->vr(_peakArea, 0.0, _screen->C_WHITE); // peak meter left border
    _screen->vr(_peakArea, 1.0, _screen->C_WHITE); // peak meter right border
}

void RecorderScreen::showRecorderScreenRecording() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawTextInArea(_textArea, Screen::TEXTPOSITION_HCENTER_TOP, false, Screen::TEXTSIZE_MEDIUM, _screen->C_WHITE,     "recording..");

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

