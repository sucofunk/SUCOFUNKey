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

#include "Snippets.h"

Snippets::Snippets() {};

Snippets::Snippets(Sucofunkey *keyboard, SongStructure *song) {
    _keyboard = keyboard;
    _song = song;
}

void Snippets::showFreeSlots() {
    for (int i=1; i<=14; i++) {
        _keyboard->setLEDState(_keyboard->getLEDPinByWhiteKey(i), _song->isSnippetSlotFree(i));
    }
};

void Snippets::hideFreeSlots() {
    for (int i=1; i<=14; i++) {
        _keyboard->setLEDState(_keyboard->getLEDPinByWhiteKey(i), false);
    }
};

void Snippets::saveSelectionToSlot(Selection *selection, byte slot) {
    _song->saveSelectionAsSnippet(*selection, slot);
};

int Snippets::cursorInSnippet(int position, int channel) {
    for (int i=1; i<=14; i++){
        if (position >= _song->getSnippet(i).startX && position <= _song->getSnippet(i).endX) {
            if (channel >= _song->getSnippet(i).startY && channel <= _song->getSnippet(i).endY) {
                return i;
            }
        }        
    }
    
    return -1;
};

void Snippets::highlightSnippetSlot(byte slot, boolean highlight) {
    _keyboard->setLEDState(_keyboard->getLEDPinByWhiteKey(slot), true);
};

boolean Snippets::deleteSnippetFromSlot(byte slot) {
    return _song->setSnippetSlotFree(slot);
};