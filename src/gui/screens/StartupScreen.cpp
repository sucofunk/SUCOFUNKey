
#include "../../helper/FSIO.h"
#include "../Screen.h"
#include "StartupScreen.h"
#include "components/SongSelector.h"

StartupScreen::StartupScreen(Sucofunkey *keyboard, Screen *screen, FSIO *fsio, char *activeSongName) {
    _keyboard = keyboard;
    _screen = screen;    
    _fsio = fsio;
    _activeSongName = activeSongName;
    _logoArea = {_screen->AREA_SCREEN.x1, _screen->AREA_SCREEN.y2/3, _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.y2/3+20, false, _screen->C_STARTUP_BG};
    _messageArea = {_screen->AREA_SCREEN.x1, _screen->AREA_SCREEN.y2/1.5, _screen->AREA_SCREEN.x2, _screen->AREA_SCREEN.y2/1.5+20, false, _screen->C_STARTUP_BG};
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

void StartupScreen::showLogo() {
    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_STARTUP_BG);
    _screen->drawTextInArea(_logoArea, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, _screen->C_WHITE, "SUCOFUNKey");
    _screen->drawTextInArea(_messageArea, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, _screen->C_LIGHTGREY, "booting the system..");
}

void StartupScreen::showMessage(const char* message, boolean warning) {
    _screen->drawTextInArea(_messageArea, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_MEDIUM, (warning ? _screen->C_WARNING : _screen->C_LIGHTGREY), message);
}

void StartupScreen::transitionToSelection() {
    for (int y=_messageArea.y1; y <= _screen->AREA_SCREEN.y2; y=y+2) {
        _screen->drawTextInArea(_messageArea, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, _screen->C_STARTUP_BG, "OK");
        _messageArea.y1 += 2;
        _messageArea.y2 += 2;
        _screen->drawTextInArea(_messageArea, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, _screen->C_LIGHTGREY, "OK");
    
        if (_logoArea.y1+2 > _screen->AREA_HEADLINE.y1) {
            _screen->drawTextInArea(_logoArea, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, _screen->C_STARTUP_BG, "SUCOFUNKey");
            _logoArea.y1 -= 2;
            _logoArea.y2 -= 2;
            _screen->drawTextInArea(_logoArea, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, _screen->C_WHITE, "SUCOFUNKey");
        }
        delay(7);
    }

    delay(200);

/*    _screen->clearAreaLTR(_messageArea, _screen->C_BLACK, 2);
    _screen->clearAreaRTL(_logoArea, _screen->C_BLACK, 2);
    _screen->clearAreaLTR(_screen->AREA_SCREEN, _screen->C_ORANGE, 2);
 
    _logoArea.x1 = _screen->AREA_SCREEN.x1;
    _logoArea.x2 = _screen->AREA_SCREEN.x2;
    _logoArea.y1 = _screen->AREA_SCREEN.y1;
    _logoArea.y2 = _screen->AREA_SCREEN.y2;
    _screen->drawTextInArea(_logoArea, _screen->TEXTPOSITION_HCENTER_VCENTER, false, _screen->TEXTSIZE_MEDIUM, _screen->C_WHITE, "SUCOFUNKey");
*/
    _activeComponent = 1;
    _screen->clearAreaLTR(_logoArea, _screen->C_STARTUP_BG, 1);    
    _drawSongSelector();    
}


void StartupScreen::_drawSongSelector() {
    _songSelector.drawSongSelector();
};
