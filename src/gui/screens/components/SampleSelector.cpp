/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2023 by Marc Berendes (marc @ sucofunk.com)
    
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
   
#include "../../Screen.h"
#include "../../../helper/FSIO.h"
#include "SampleSelector.h"

SampleSelector::SampleSelector() {};

SampleSelector::SampleSelector(Sucofunkey *keyboard, Screen *screen, FSIO *fsio) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _AREA_SONGSELECTOR = { _screen->AREA_SCREEN.x1, static_cast<int>(_screen->AREA_SCREEN.y2*0.2), _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.y2, false, _screen->C_BLACK};
    int padding = 10;
    int height = _AREA_SONGSELECTOR.y2 - _AREA_SONGSELECTOR.y1 - padding*2;
    _AREA_SONGSELECTOR_LINE_1 = { _AREA_SONGSELECTOR.x1+padding, _AREA_SONGSELECTOR.y1+padding, _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.2)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_2 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.2)+padding), _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.4)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_3 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.4)+padding), _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.6)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_4 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.6)+padding), _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.8)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_5 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.8)+padding), _AREA_SONGSELECTOR.x2-padding, _AREA_SONGSELECTOR.y1+height+padding, false, _AREA_SONGSELECTOR.bgColor};    
    
    _fsio->setSelectedSamplePathFromSD(_offset + _activeItem);
}

long SampleSelector::receiveTimerTick() {
    return 100000;
}

void SampleSelector::handleEvent(Sucofunkey::keyQueueStruct event) {
      // convert encoder 1 ticks to cursor up/down event
      if (event.index == Sucofunkey::ENCODER_1) {
        if (event.pressed) {
            event.index = Sucofunkey::CURSOR_DOWN;
        } else {
            event.index = Sucofunkey::CURSOR_UP;
            event.pressed = true;
        }            
      }

      switch(event.index) {          
        case Sucofunkey::CURSOR_UP:         
            if (event.pressed) {
                if (_activeItem > 0) {
                    _activeItem--;
                    drawSampleSelector();
                } else {
                    if (_offset > 0) _offset--;
                    drawSampleSelector();
                }
            }    
            break;
        case Sucofunkey::CURSOR_DOWN:
            if (event.pressed) {
                if (_activeItem < 4 && _sampleCount-1 > _activeItem) { 
                    _activeItem++;
                    drawSampleSelector();
                } else {
                    if (_offset+4 < _sampleCount-1 && _activeItem == 4) _offset++;
                    drawSampleSelector();
                }
            }
            break;            
        case Sucofunkey::SET:
        case Sucofunkey::ENCODER_1_PUSH:
            // something in the list selected
            if (event.pressed) {
                    String s = _fsio->getSampleFileName(_activeItem+_offset);

                    if (s[0] == '/') {
                        // directory
                        _fsio->readLibrarySamplesFromSD(_fsio->getLibrarySamples(), s);
                        // set cursor to first entry in list/viewport after changing a directory
                        _activeItem = 0; 
                        _offset = 0;
                        drawSampleSelector();
                    } else {
                        // file
                        _fsio->setSelectedSamplePathFromSD(_offset + _activeItem);
                        _keyboard->addApplicationEventToQueue(Sucofunkey::SAMPLE_LIBRARY_SELECTED);                                                
                    }
            }                
            break;

        case Sucofunkey::FN_SET:
        case Sucofunkey::ZOOM:
            if (event.pressed) {
                _keyboard->addApplicationEventToQueue(Sucofunkey::SAMPLE_LIBRARY_CANCEL);
                _screen->fadeBacklightOut(1);
            }
            break;

        default:
            break;      
    } 
}

void SampleSelector::drawSampleSelector() {    
    _sampleCount = _fsio->getSamplesCount();

    _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Sample library");

    _screen->fillArea(_AREA_SONGSELECTOR_LINE_1, _activeItem == 0 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);

    if (_offset <= _sampleCount-1) {
        getLineDescription(_offset, _line1);
        // used to store filename for pre-listening the sample (done in SamperScreen, as it handles the component) 
        if (_activeItem == 0) _fsio->setSelectedSamplePathFromSD(_fsio->getSampleFileName(_offset));
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_1, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line1);
    }


    _screen->fillArea(_AREA_SONGSELECTOR_LINE_2, _activeItem == 1 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);

    if (_offset+1 <= _sampleCount-1) {
        getLineDescription(_offset+1, _line2);
        if (_activeItem == 1) _fsio->setSelectedSamplePathFromSD(_fsio->getSampleFileName(_offset+1));            
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_2, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line2);
    }


    _screen->fillArea(_AREA_SONGSELECTOR_LINE_3, _activeItem == 2 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);

    if (_offset+2 <= _sampleCount-1) {
        getLineDescription(_offset+2, _line3);
        if (_activeItem == 2) _fsio->setSelectedSamplePathFromSD(_fsio->getSampleFileName(_offset+2));    
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_3, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line3);
    }


    _screen->fillArea(_AREA_SONGSELECTOR_LINE_4, _activeItem == 3 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);

    if (_offset+3 <= _sampleCount-1) {
        getLineDescription(_offset+3, _line4);
        if (_activeItem == 3) _fsio->setSelectedSamplePathFromSD(_fsio->getSampleFileName(_offset+3)); 
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_4, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line4);
    }

    _screen->fillArea(_AREA_SONGSELECTOR_LINE_5, _activeItem == 4 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);

    if (_offset+4 <= _sampleCount-1) {
        getLineDescription(_offset+4, _line5);
        if (_activeItem == 4) _fsio->setSelectedSamplePathFromSD(_fsio->getSampleFileName(_offset+4)); 
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_5, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line5);
    }

    _screen->hr(_AREA_SONGSELECTOR, 0.0, _screen->C_STARTUP_HR);
    _screen->hr(_AREA_SONGSELECTOR, 1, _screen->C_STARTUP_HR);
    _screen->vr(_AREA_SONGSELECTOR, 0, _screen->C_STARTUP_HR);
    _screen->vr(_AREA_SONGSELECTOR, 1, _screen->C_STARTUP_HR);

    _keyboard->addApplicationEventToQueue(Sucofunkey::SAMPLE_LIBRARY_PRELISTEN);
}

boolean SampleSelector::getLineDescription(int fileIndex, char *lineBuffer) {
    if (fileIndex == -1) {
        strcpy(lineBuffer, "create a new record");
        return true;
    } else {
        if (fileIndex+1 <= _sampleCount) {
            _fsio->getSampleName(fileIndex, lineBuffer);
            return true;
        } else {
            strcpy(lineBuffer, " ");
            return false;
        }
    }
}
