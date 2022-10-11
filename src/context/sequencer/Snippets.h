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
   
#ifndef Snippets_h
#define Snippets_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "SongStructure.h"
#include "Selection.h"

class Snippets {
    public:
        Snippets();
        Snippets(Sucofunkey *keyboard, SongStructure *song);
        
        void showFreeSlots();
        void hideFreeSlots();
        void saveSelectionToSlot(Selection *selection, byte slot);
        int cursorInSnippet(int position, int channel);
        void highlightSnippetSlot(byte slot, boolean highlight);
        boolean deleteSnippetFromSlot(byte slot);

    private:
        Sucofunkey *_keyboard;
        SongStructure *_song;
};

#endif