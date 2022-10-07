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
   
#ifndef Play_h
#define Play_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/FSIO.h"
#include "../../helper/AudioResources.h"
#include "SongStructure.h"
#include "Selection.h"
#include "Snippets.h"
#include "Swing.h"

class Play {
    public:
        Play(Sucofunkey *keyboard, FSIO *fsio, SampleFSIO *sfsio, unsigned int *extmemArray, AudioResources *audioResources) {
            _keyboard = keyboard;
            _fsio = fsio;
            _sfsio = sfsio;
            _extmemArray = extmemArray;
            _audioResources = audioResources;

            _swing = _song.getSwing();
            _snippets = Snippets(_keyboard, &_song);
            _initialized = true;
        };

        boolean isInitialized() {
            return _initialized;
        }

        unsigned int *_extmemArray;
        AudioResources *_audioResources;

        SongStructure* getSong();
        Snippets* getSnippets();


    private:
        Sucofunkey* _keyboard;
        SampleFSIO* _sfsio;
        FSIO* _fsio;
        
        SongStructure _song;
        Snippets _snippets;
        Swing* _swing;

        boolean _initialized = false;

        boolean playMemsInUse[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
        int playingSnippets[4] = {-1, -1, -1, -1};
        byte snippetChannels[4][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};        
};

#endif