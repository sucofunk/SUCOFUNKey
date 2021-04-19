#ifndef SequencerScreen_h
#define SequencerScreen_h

#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../helper/FSIO.h"
#include "../../helper/SampleFSIO.h"
#include "../../helper/AudioResources.h"
#include "../Screen.h"
#include "../../context/sequencer/Pattern.h"

class SequencerScreen {
    public:
        SequencerScreen();
        SequencerScreen(Sucofunkey *keyboard, Screen *screen, SampleFSIO *sfsio, AudioResources *audioResources);

        void initializeGrid(Pattern *pattern); 
        void drawTrackerAtPosition(uint16_t position, Pattern *p, bool editMode, byte highlightEvery);
        void drawCursorAtColumn(byte column);
        void drawBPM(float bpm);
        void drawExtMemPercentage(byte percent);
        
    private:
        Sucofunkey *_keyboard;
        Screen *_screen;
        FSIO *_fsio;
        SampleFSIO *_sfsio;
        AudioResources *_audioResources;

        char _cBuff3[3];
        char _cBuff10[10];
        int _x;
        int _y;

        Pattern *_pattern;
        void _drawEmptyRow(uint16_t row);
        //void _drawGridCell(int col, int row, const char *text, uint16_t bgColor);
        void _drawGridCell(int col, int row, boolean hasContent, const char *text, byte stereoPosition, byte velocity, uint16_t bgColor);
};

#endif
