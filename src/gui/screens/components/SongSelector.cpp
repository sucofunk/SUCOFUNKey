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
   
#include "../../Screen.h"
#include "../../../helper/FSIO.h"
#include "SongSelector.h"
#include "Input.h"

// ToDo:
//  - sort songs alphabetically
//  - delete song (recursive)
//  - move to SDFat2 ?
//  - get rid of flickering while scrolling songs
//  - get rid of flickering while typing e.g. A -> _ -> c -> g -> E -> Q

SongSelector::SongSelector() {};

SongSelector::SongSelector(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, char *activeSongName) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _activeSongName = activeSongName;
    _AREA_SONGSELECTOR = { _screen->AREA_SCREEN.x1, static_cast<int>(_screen->AREA_SCREEN.y2*0.2), _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.y2, false, _screen->C_BLACK};
    int padding = 10;
    int height = _AREA_SONGSELECTOR.y2 - _AREA_SONGSELECTOR.y1 - padding*2;
    _AREA_SONGSELECTOR_LINE_1 = { _AREA_SONGSELECTOR.x1+padding, _AREA_SONGSELECTOR.y1+padding, _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.2)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_2 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.2)+padding), _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.4)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_3 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.4)+padding), _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.6)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_4 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.6)+padding), _AREA_SONGSELECTOR.x2-padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.8)+padding), false, _AREA_SONGSELECTOR.bgColor};
    _AREA_SONGSELECTOR_LINE_5 = { _AREA_SONGSELECTOR.x1+padding, static_cast<int>(_AREA_SONGSELECTOR.y1+(height*0.8)+padding), _AREA_SONGSELECTOR.x2-padding, _AREA_SONGSELECTOR.y1+height+padding, false, _AREA_SONGSELECTOR.bgColor};    
    _input = Input(_keyboard, _screen, _fsio, _AREA_SONGSELECTOR_LINE_1);
}

long SongSelector::receiveTimerTick() {
    switch(_activeComponent) {
        case 1: // send events to input
            return _input.receiveTimerTick();
            break;
        default:
            break;
    }
    return 100000;
}

void SongSelector::handleEvent(Sucofunkey::keyQueueStruct event) {
    // check events as song selector list..
    if (_activeComponent == 0) {
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
                    drawSongSelector();
                } else {
                    if (_offset > -1) _offset--;
                    drawSongSelector();
                }
            }    
            break;
        case Sucofunkey::CURSOR_DOWN:
            if (event.pressed) {
                if (_activeItem < 4 && _songCount > _activeItem) { 
                    _activeItem++;
                    drawSongSelector();
                } else {
                    if (_offset+4 < _songCount-1 && _activeItem == 4) _offset++;
                    drawSongSelector();
                }
            }
            break;            
        case Sucofunkey::SET:
        case Sucofunkey::ENCODER_1_PUSH:
            if (event.pressed) {
                if (_activeItem == 0 && _offset == -1) {
                    _activeComponent = 1; // activate input field
                    _screen->clearAreaLTR(_screen->AREA_SCREEN, _screen->C_STARTUP_BG, 1);                
                    _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Song title");
                    _input.activateInput();
                } else {
                    int sel = _offset+_activeItem;                    
                    getLineDescription(sel, _line1);
                                        
                    // set selected song directory to char array available in main scope
                    strcpy(_activeSongName, _line1);

                    // .. and notify main scheduler switch
                    _keyboard->addApplicationEventToQueue(Sucofunkey::SONGSELECTED);
                }
            }                
            break;
        default:
            break;
      }    
    } else {
        
        char buff[9];
        String s;
        byte result;

        // send events to input for song name
        if (_activeComponent == 1) {
            if (event.type == Sucofunkey::EVENT_APPLICATION) {
                switch(event.index) {
                    case Sucofunkey::INPUT_SET:
                        s = String(_input.getInputValue());
                        s.trim();                        
                        s.replace(" ", "_");
                        s.toCharArray(buff, s.length()+1);                        

                        result = _fsio->createSong(buff);

                        if (result == 1) {
                            // set selected song directory to char array available in main scope
                            strcpy(_activeSongName, buff);
                            // .. and notify main scheduler switch
                            delay(100); // wait a bit, if SD card is hanging..
                            _keyboard->addApplicationEventToQueue(Sucofunkey::SONGSELECTED);
                        } else {
                            if (result == 0 || result == 2 || result == 3) {
                                _input.showErrorMessage("Song already exists", 750);
                            } else {
                                _input.showErrorMessage("No name is not a name!", 750);
                            }                            
                        }
                        break;
                    case Sucofunkey::INPUT_CANCEL:
                        // back to song collection
                        _screen->clearAreaRTL(_screen->AREA_SCREEN, _screen->C_STARTUP_BG, 1);                        
                        _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Song collection");

                        _activeComponent = 0;
                        drawSongSelector();
                        break;
                    default:
                        break;
                }
            } else {
                _input.handleEvent(event);
            }            
        }
    }
}

void SongSelector::drawSongSelector() {    
    _songCount = _fsio->getSongCount();

    _screen->drawTextInArea(_screen->AREA_HEADLINE, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, "Song collection");
        
    _screen->fillArea(_AREA_SONGSELECTOR_LINE_1, _activeItem == 0 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);    
    getLineDescription(_offset, _line1);
    _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_1, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line1);

    _screen->fillArea(_AREA_SONGSELECTOR_LINE_2, _activeItem == 1 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);
    if (_offset+1 <= _songCount-1) {
        getLineDescription(_offset+1, _line2);
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_2, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line2);
    }

    _screen->fillArea(_AREA_SONGSELECTOR_LINE_3, _activeItem == 2 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);
    if (_offset+2 <= _songCount-2) {
        getLineDescription(_offset+2, _line3);
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_3, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line3);
    }

    _screen->fillArea(_AREA_SONGSELECTOR_LINE_4, _activeItem == 3 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);
    if (_offset+3 <= _songCount-1) {
        getLineDescription(_offset+3, _line4);
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_4, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line4);
    }

    _screen->fillArea(_AREA_SONGSELECTOR_LINE_5, _activeItem == 4 ? _screen->C_SELECTOR_SELECTED_BG : _AREA_SONGSELECTOR.bgColor);
    if (_offset+4 <= _songCount-1) {
        getLineDescription(_offset+4, _line5);
        _screen->drawTextInArea(_AREA_SONGSELECTOR_LINE_5, _screen->TEXTPOSITION_LEFT_VCENTER, false, _screen->TEXTSIZE_MEDIUM, false, _screen->C_WHITE, _line5);
    }

    _screen->hr(_AREA_SONGSELECTOR, 0.0, _screen->C_STARTUP_HR);
    _screen->hr(_AREA_SONGSELECTOR, 1, _screen->C_STARTUP_HR);
    _screen->vr(_AREA_SONGSELECTOR, 0, _screen->C_STARTUP_HR);
    _screen->vr(_AREA_SONGSELECTOR, 1, _screen->C_STARTUP_HR);
}

boolean SongSelector::getLineDescription(int fileIndex, char *lineBuffer) {
    if (fileIndex == -1) {
        strcpy(lineBuffer, "create a new song");
        return true;
    } else {
        if (fileIndex+1 <= _songCount) {
            _fsio->getSongName(fileIndex, lineBuffer);
            return true;
        } else {
            strcpy(lineBuffer, " ");
            return false;
        }
    }
}
