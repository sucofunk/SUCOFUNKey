
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "StartupScreen.h"
#include "components/SongSelector.h"

StartupScreen::StartupScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, char *activeSongName) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _activeSongName = activeSongName;
    _messageArea = {_screen->AREA_SCREEN.x1, _screen->AREA_SCREEN.y2-12, _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.y2, false, _screen->C_STARTUP_BG};
    _songSelector = SongSelector(_keyboard, _screen, _fsio, _activeSongName);
}

void StartupScreen::handleEvent(Sucofunkey::keyQueueStruct event) {
    switch(_activeComponent) {
        case 1: // send events to songSelector
            _songSelector.handleEvent(event);
            break;
        default:
            break;
    }
}

long StartupScreen::receiveTimerTick() {
    switch(_activeComponent) {
        case 1: // send events to songSelector
            return _songSelector.receiveTimerTick();
            break;
        default:
            break;
    }
    return 100000;
}

void StartupScreen::showMessage(const char* message, boolean warning) {
    _screen->drawTextInArea(_messageArea, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, false, (warning ? _screen->C_WARNING : _screen->C_LIGHTGREY), message);
}

void StartupScreen::transitionToSelection() {
    _activeComponent = 1;
    _screen->clearAreaLTR(_screen->AREA_SCREEN, _screen->C_STARTUP_BG, 3);
    _screen->fadeBacklightOut(1);
    _drawSongSelector();        
    _screen->fadeBacklightIn(10);    
}


void StartupScreen::_drawSongSelector() {
    _songSelector.drawSongSelector();
};
