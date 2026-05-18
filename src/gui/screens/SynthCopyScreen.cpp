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
   
#include "SynthCopyScreen.h"
#include "../../helper/DebugPrint.h"

SynthCopyScreen::SynthCopyScreen() {};

SynthCopyScreen::SynthCopyScreen(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;
    _screen = screen;
    
    _headlineArea = {0, 60, 319, 80, false, _screen->C_BLACK};
    _lineChannel = {0, 90, 319, 105, false, _screen->C_BLACK};
    _lineRange = {0, 110, 319, 125, false, _screen->C_BLACK};
    _lineTrigger = {0, 130, 319, 145, false, _screen->C_BLACK};
    _lineRecording = {0, 165, 319, 180, false, _screen->C_BLACK};
}

void SynthCopyScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
    _isRunning = false;
    _keyboard->addApplicationEventToQueue(Sucofunkey::MENU_BACK);
}

long SynthCopyScreen::receiveTimerTick() {
    return 15000;
}

void SynthCopyScreen::show(byte channel, byte startNote, byte endNote, long releaseMS) {
//    _screen->fadeBacklightOut(2);
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawTextInArea(_headlineArea, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Synthkopierer");

    updateChannel(channel);
    updateRange(startNote, endNote);
    updateTriggerTime(releaseMS);
    updateRecordingState(false);

    _screen->fadeBacklightIn(0);  
}

void SynthCopyScreen::updateChannel(byte channel) {
    _screen->fillArea(_lineChannel, _screen->C_BLACK);
    char text[20];
    sprintf(text, "Channel: %d", channel);
    _screen->drawTextInArea(_lineChannel, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, text);
}

void SynthCopyScreen::updateRange(byte startNote, byte endNote) {
    _screen->fillArea(_lineRange, _screen->C_BLACK);
    char startName[5];
    char endName[5];
    _keyboard->getMIDINoteName(startNote).toCharArray(startName, 5);
    _keyboard->getMIDINoteName(endNote).toCharArray(endName, 5);
    char text[50];
    sprintf(text, "Recording from %s to %s.", startName, endName);
    _screen->drawTextInArea(_lineRange, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, text);
}

void SynthCopyScreen::updateTriggerTime(long releaseMS) {
    _screen->fillArea(_lineTrigger, _screen->C_BLACK);
    char text[30];
    sprintf(text, "Trigger time: %ld ms", releaseMS);
    _screen->drawTextInArea(_lineTrigger, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, text);
}

void SynthCopyScreen::updateRecordingState(boolean isRecording, byte current, byte total) {
    _screen->fillArea(_lineRecording, _screen->C_BLACK);
    if (isRecording) {
        char text[30];
        sprintf(text, "recording %d of %d", current, total);
        _screen->drawTextInArea(_lineRecording, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_RECORDING, text);
    }
}

