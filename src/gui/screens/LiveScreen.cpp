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
   
#include "LiveScreen.h"

LiveScreen::LiveScreen(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;
    _screen = screen;        
}

void LiveScreen::showEmptyOverview(boolean createAreas) {
    int offsetLR = 12;
    int cellSize = 20;    
    int centerLineY = 105;

    int sharpieTopLineY = centerLineY - cellSize - 7;
    int sharpieBottomLineY = centerLineY - 5;
    int normalTopLineY = centerLineY + 5;
    int normalBottomLineY = centerLineY + cellSize + 7;

    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);
    

    // first F# G# A#
    _screen->drawFastHLine(offsetLR + static_cast<int>(0.5*cellSize), sharpieTopLineY, 3*cellSize+4, _screen->C_WHITE);
    _screen->drawFastHLine(offsetLR + static_cast<int>(0.5*cellSize), sharpieBottomLineY, 3*cellSize+4, _screen->C_WHITE);

    _screen->drawFastVLine(offsetLR + static_cast<int>(0.5*cellSize), sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(1.5*cellSize) + 1, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(2.5*cellSize) + 2, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(3.5*cellSize) + 3, sharpieTopLineY, 22, _screen->C_WHITE);    


    // first C# D#
    _screen->drawFastHLine(offsetLR + static_cast<int>(4.5*cellSize) + 4, sharpieTopLineY, 2*cellSize+3, _screen->C_WHITE);
    _screen->drawFastHLine(offsetLR + static_cast<int>(4.5*cellSize) + 4, sharpieBottomLineY, 2*cellSize+3, _screen->C_WHITE);

    _screen->drawFastVLine(offsetLR + static_cast<int>(4.5*cellSize) + 4, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(5.5*cellSize) + 5, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(6.5*cellSize) + 6, sharpieTopLineY, 22, _screen->C_WHITE);


    // second F# G# A#
    _screen->drawFastHLine(offsetLR + static_cast<int>(7.5*cellSize) +  8, sharpieTopLineY, 3*cellSize+4, _screen->C_WHITE);
    _screen->drawFastHLine(offsetLR + static_cast<int>(7.5*cellSize) +  8, sharpieBottomLineY, 3*cellSize+4, _screen->C_WHITE);

    _screen->drawFastVLine(offsetLR + static_cast<int>(7.5*cellSize) + 8, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(8.5*cellSize) + 9, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(9.5*cellSize) + 10, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(10.5*cellSize) + 11, sharpieTopLineY, 22, _screen->C_WHITE);    


    // second C# D#
    _screen->drawFastHLine(offsetLR + static_cast<int>(11.5*cellSize) + 12, sharpieTopLineY, 2*cellSize+3, _screen->C_WHITE);
    _screen->drawFastHLine(offsetLR + static_cast<int>(11.5*cellSize) + 12, sharpieBottomLineY, 2*cellSize+3, _screen->C_WHITE);

    _screen->drawFastVLine(offsetLR + static_cast<int>(11.5*cellSize) + 12, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(12.5*cellSize) + 13, sharpieTopLineY, 22, _screen->C_WHITE);    
    _screen->drawFastVLine(offsetLR + static_cast<int>(13.5*cellSize) + 14, sharpieTopLineY, 22, _screen->C_WHITE);    


    // white keys
    _screen->drawFastHLine(offsetLR, centerLineY + 5, 14*cellSize+15, _screen->C_WHITE);
    _screen->drawFastHLine(offsetLR, centerLineY + cellSize + 7, 14*cellSize+15, _screen->C_WHITE);

    for (int i=0; i<=14; i++) {
        _screen->drawFastVLine(i*cellSize+i+offsetLR, centerLineY + 5, cellSize + 2, _screen->C_WHITE);
    }


    // define areas for slots
    if (createAreas) {
        for (int i=0; i<24; i++) {
            _slotAreas[i].x1 = 1;
            _slotAreas[i].x2 = 1;
            _slotAreas[i].y1 = 1;
            _slotAreas[i].y2 = 1;

            _slotAreas[i].bgColor = _screen->C_BLACK;
            _slotAreas[i].transparent = false;


            if (i == 1 || i == 3 || i == 5 || i == 8 || i == 10 || i == 13 || i == 15 || i == 17 || i == 20 || i == 22) {
                // black keys
                _slotAreas[i].y1 = sharpieTopLineY + 1;
                _slotAreas[i].y2 = sharpieBottomLineY;
            } else {
                // white keys
                _slotAreas[i].y1 = normalTopLineY + 1;
                _slotAreas[i].y2 = normalBottomLineY;
            }           

        }

        // black keys..
        _slotAreas[1].x1 = offsetLR + static_cast<int>(0.5*cellSize) + 1;
        _slotAreas[1].x2 = _slotAreas[1].x1 + cellSize;

        _slotAreas[3].x1 = offsetLR + static_cast<int>(1.5*cellSize) + 2;
        _slotAreas[3].x2 = _slotAreas[3].x1 + cellSize;

        _slotAreas[5].x1 = offsetLR + static_cast<int>(2.5*cellSize) + 3;
        _slotAreas[5].x2 = _slotAreas[5].x1 + cellSize;


        _slotAreas[8].x1 = offsetLR + static_cast<int>(4.5*cellSize) + 5;
        _slotAreas[8].x2 = _slotAreas[8].x1 + cellSize;

        _slotAreas[10].x1 = offsetLR + static_cast<int>(5.5*cellSize) + 6;
        _slotAreas[10].x2 = _slotAreas[10].x1 + cellSize;


        _slotAreas[13].x1 = offsetLR + static_cast<int>(7.5*cellSize) + 9;
        _slotAreas[13].x2 = _slotAreas[13].x1 + cellSize;

        _slotAreas[15].x1 = offsetLR + static_cast<int>(8.5*cellSize) + 10;
        _slotAreas[15].x2 = _slotAreas[15].x1 + cellSize;

        _slotAreas[17].x1 = offsetLR + static_cast<int>(9.5*cellSize) + 11;
        _slotAreas[17].x2 = _slotAreas[17].x1 + cellSize;


        _slotAreas[20].x1 = offsetLR + static_cast<int>(11.5*cellSize) + 13;
        _slotAreas[20].x2 = _slotAreas[20].x1 + cellSize;

        _slotAreas[22].x1 = offsetLR + static_cast<int>(12.5*cellSize) + 14;
        _slotAreas[22].x2 = _slotAreas[22].x1 + cellSize;


        // white keys..
        _slotAreas[0].x1 = offsetLR + 1;
        _slotAreas[0].x2 = _slotAreas[0].x1 + cellSize;

        _slotAreas[2].x1 = _slotAreas[0].x2 + 1;
        _slotAreas[2].x2 = _slotAreas[2].x1 + cellSize;

        _slotAreas[4].x1 = _slotAreas[2].x2 + 1;
        _slotAreas[4].x2 = _slotAreas[4].x1 + cellSize;

        _slotAreas[6].x1 = _slotAreas[4].x2 + 1;
        _slotAreas[6].x2 = _slotAreas[6].x1 + cellSize;

        _slotAreas[7].x1 = _slotAreas[6].x2 + 1;
        _slotAreas[7].x2 = _slotAreas[7].x1 + cellSize;

        _slotAreas[9].x1 = _slotAreas[7].x2 + 1;
        _slotAreas[9].x2 = _slotAreas[9].x1 + cellSize;

        _slotAreas[11].x1 = _slotAreas[9].x2 + 1;
        _slotAreas[11].x2 = _slotAreas[11].x1 + cellSize;

        _slotAreas[12].x1 = _slotAreas[11].x2 + 1;
        _slotAreas[12].x2 = _slotAreas[12].x1 + cellSize;

        _slotAreas[14].x1 = _slotAreas[12].x2 + 1;
        _slotAreas[14].x2 = _slotAreas[14].x1 + cellSize;

        _slotAreas[16].x1 = _slotAreas[14].x2 + 1;
        _slotAreas[16].x2 = _slotAreas[16].x1 + cellSize;

        _slotAreas[18].x1 = _slotAreas[16].x2 + 1;
        _slotAreas[18].x2 = _slotAreas[18].x1 + cellSize;

        _slotAreas[19].x1 = _slotAreas[18].x2 + 1;
        _slotAreas[19].x2 = _slotAreas[19].x1 + cellSize;

        _slotAreas[21].x1 = _slotAreas[19].x2 + 1;
        _slotAreas[21].x2 = _slotAreas[21].x1 + cellSize;

        _slotAreas[23].x1 = _slotAreas[21].x2 + 1;
        _slotAreas[23].x2 = _slotAreas[23].x1 + cellSize;

        _slotTypeSelectionAreaLeft.bgColor = _screen->C_BLACK;
        _slotTypeSelectionAreaLeft.transparent = false;
        _slotTypeSelectionAreaLeft.x1 = 20;
        _slotTypeSelectionAreaLeft.x2 = 140;
        _slotTypeSelectionAreaLeft.y1 = centerLineY - 10;
        _slotTypeSelectionAreaLeft.y2 = centerLineY + 10;

        _slotTypeSelectionAreaRight.bgColor = _screen->C_BLACK;
        _slotTypeSelectionAreaRight.transparent = false;
        _slotTypeSelectionAreaRight.x1 = 180;
        _slotTypeSelectionAreaRight.x2 = 299;
        _slotTypeSelectionAreaRight.y1 = centerLineY - 10;
        _slotTypeSelectionAreaRight.y2 = centerLineY + 10;

        _bpmValueArea.bgColor = _screen->C_BLACK;
        _bpmValueArea.transparent = false;
        _bpmValueArea.x1 = 10;
        _bpmValueArea.x2 = 155;
        _bpmValueArea.y1 = static_cast<int>(sharpieTopLineY / 2)-10;        
        _bpmValueArea.y2 = static_cast<int>(sharpieTopLineY / 2)+10;

        _bpmLabelArea.bgColor = _screen->C_BLACK;
        _bpmLabelArea.transparent = false;
        _bpmLabelArea.x1 = 165;
        _bpmLabelArea.x2 = 309;
        _bpmLabelArea.y1 = static_cast<int>(sharpieTopLineY / 2)-10;        
        _bpmLabelArea.y2 = static_cast<int>(sharpieTopLineY / 2)+10;

        _snippetNameArea.bgColor = _screen->C_BLACK;
        _snippetNameArea.transparent = false;
        _snippetNameArea.x1 = 0;
        _snippetNameArea.x2 = 319;
        _snippetNameArea.y1 = centerLineY - 100;
        _snippetNameArea.y2 = centerLineY - 80;

        _snippetsCompleteArea.bgColor = _screen->C_BLACK;
        _snippetsCompleteArea.transparent = false;
        _snippetsCompleteArea.x1 = 0;
        _snippetsCompleteArea.x2 = 106;
        _snippetsCompleteArea.y1 = centerLineY - 10;
        _snippetsCompleteArea.y2 = centerLineY + 10;

        _snippetsLoopArea.bgColor = _screen->C_BLACK;
        _snippetsLoopArea.transparent = false;
        _snippetsLoopArea.x1 = 106;
        _snippetsLoopArea.x2 = 213;
        _snippetsLoopArea.y1 = centerLineY - 10;
        _snippetsLoopArea.y2 = centerLineY + 10;

        _snippetsMIDIArea.bgColor = _screen->C_BLACK;       
        _snippetsMIDIArea.transparent = false;        
        _snippetsMIDIArea.x1 = 213;
        _snippetsMIDIArea.x2 = 319;
        _snippetsMIDIArea.y1 = centerLineY - 10;       
        _snippetsMIDIArea.y2 = centerLineY + 10;       

    }



    // BPM Label
    _screen->drawTextInArea(_bpmLabelArea, Screen::TEXTPOSITION_LEFT_BOTTOM, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "BPM");

};

// slotNumber1 = 1..24
void LiveScreen::drawOverviewSlot(Play::LiveSlotDefinitionStruct* slot, int slotNumber1) {

    uint16_t color;

    switch (slot->type) {
        case Play::EMPTY:
            color = _screen->C_BLACK;
            break;
        case Play::SNIPPET:
            color = _screen->C_LIVE_SNIPPET;
            break;
        case Play::SAMPLE:
            color = _screen->C_LIVE_SAMPLE;
            break;
        default:
            color = _screen->C_BLACK;
            break;            
    }
    
    _screen->fillArea(_slotAreas[slotNumber1 - 1], color);

};


void LiveScreen::drawBPM(float bpm) {
    sprintf(_cBuff5, "%g", bpm);
    _screen->drawTextInArea(_bpmValueArea, Screen::TEXTPOSITION_RIGHT_BOTTOM, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff5);
};



void LiveScreen::showSlotTypeSelection(Play::LiveSlotDefinitionStruct slot, boolean initial) {
    if (initial) _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);    
    _screen->drawTextInArea(_slotTypeSelectionAreaLeft, Screen::TEXTPOSITION_RIGHT_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, slot.type == Play::SNIPPET ? _screen->C_LIVE_SNIPPET : _screen->C_LIGHTGREY, "Snippet");
    _screen->drawTextInArea(_slotTypeSelectionAreaRight, Screen::TEXTPOSITION_LEFT_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, slot.type == Play::SAMPLE ? _screen->C_LIVE_SAMPLE : _screen->C_LIGHTGREY, "Sample"); 
};


void LiveScreen::showSelectSnippet(Play::LiveSlotDefinitionStruct slot, boolean initial) {
    if (initial) _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);    
    _screen->drawTextInArea(_slotTypeSelectionAreaLeft, Screen::TEXTPOSITION_RIGHT_BOTTOM, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Snippet");

    sprintf(_cBuff3, "%d", slot.snippet);
    _screen->drawTextInArea(_slotTypeSelectionAreaRight, Screen::TEXTPOSITION_LEFT_BOTTOM, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, slot.snippet == -1 ? "NONE" : _cBuff3);
};


void LiveScreen::showSnippetConfig(Play::LiveSlotDefinitionStruct slot) {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    sprintf(_cBuff10, "Snippet %d", slot.snippet);
    _screen->drawTextInArea(_snippetNameArea, Screen::TEXTPOSITION_HCENTER_VCENTER, false, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _cBuff10);

    updateSnippetConfig(slot, 1, NONE);
    updateSnippetConfig(slot, 2, NONE);
    updateSnippetConfig(slot, 4, NONE);
};

// encoder 1..4 and 5..8 (with FN hold)
void LiveScreen::updateSnippetConfig(Play::LiveSlotDefinitionStruct slot, int encoder, Options option) {
    switch (encoder) {
        case 1:
        _screen->drawTextInArea(_snippetsCompleteArea, Screen::TEXTPOSITION_LEFT_BOTTOM, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, slot.immediateStopOnRelease ? "while hold" : "play complete");
        break;

        case 2:
        _screen->drawTextInArea(_snippetsLoopArea, Screen::TEXTPOSITION_HCENTER_BOTTOM, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WHITE, slot.loopSnippet ? "LOOP" : "ONCE");
        break;
        
        case 4:
        sprintf(_cBuff10, "%d Note", slot.midiNote);
        _screen->drawTextInArea(_slotTypeSelectionAreaRight, Screen::TEXTPOSITION_RIGHT_BOTTOM, true, Screen::TEXTSIZE_MEDIUM, false, option == NONE ? _screen->C_WHITE : option == MIDI_NOTE_FREE ? _screen->C_GREEN : _screen->C_WARNING , _cBuff10);
        break;
    }
}; 


void LiveScreen::drawMIDIinWaitForTraining(boolean show) {
    _screen->fillArea(_screen->AREA_BOTTOM_MENU, _screen->C_BLACK);

    if (show) {
        _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU, Screen::TEXTPOSITION_HCENTER_VCENTER, true, Screen::TEXTSIZE_MEDIUM, false, _screen->C_WARNING, "hit a note on channel 1");
    }        
};
