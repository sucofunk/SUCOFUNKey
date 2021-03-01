#include "BottomMenu.h"

BottomMenu::BottomMenu() {};

BottomMenu::BottomMenu(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;
    _screen = screen;    
}

void BottomMenu::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {
        switch (event.index) {
            case Sucofunkey::CURSOR_LEFT:
                if (_itemCount == 2 && _selectedItem == 3) _selectedItem = 1;
                if (_itemCount == 3 && _selectedItem == 2) _selectedItem = 1;
                if (_itemCount == 3 && _selectedItem == 3) _selectedItem = 2;
                _redrawMenu();
                break;    
            case Sucofunkey::CURSOR_RIGHT:
                if (_itemCount == 2 && _selectedItem == 1) _selectedItem = 3;
                if (_itemCount == 3 && _selectedItem == 2) _selectedItem = 3;
                if (_itemCount == 3 && _selectedItem == 1) _selectedItem = 2;
                _redrawMenu();
                break;    
            case Sucofunkey::SET:                
                if (_selectedItem == 1) _keyboard->addApplicationEventToQueue(_keyboard->BOTTOM_NAV_ITEM1);
                if (_selectedItem == 2) _keyboard->addApplicationEventToQueue(_keyboard->BOTTOM_NAV_ITEM2);
                if (_selectedItem == 3) _keyboard->addApplicationEventToQueue(_keyboard->BOTTOM_NAV_ITEM3);
            default:
                break;
        }
    }
};

BottomMenu::BottomMenu(Sucofunkey *keyboard, Screen *screen, char *label1, int retVal1, char *label3, int retVal3) {
    _keyboard = keyboard;
    _screen = screen;    
    _label1 = label1;
    _label3 = label3;
    _retVal1 = retVal1;
    _retVal2 = retVal3;
    _itemCount = 2;
    _selectedItem = 1;
};

BottomMenu::BottomMenu(Sucofunkey *keyboard, Screen *screen, char *label1, int retVal1, char *label2, int retVal2, char *label3, int retVal3) {
    _keyboard = keyboard;
    _screen = screen;    
    _label1 = label1;
    _label2 = label2;
    _label3 = label3;
    _retVal1 = retVal1;
    _retVal2 = retVal2;
    _retVal3 = retVal3;
    _itemCount = 3;
    _selectedItem = 1;    
};

void BottomMenu::setupMenu3(char *label1, int retVal1, char *label2, int retVal2, char *label3, int retVal3) {
    _label1 = label1;
    _label2 = label2;
    _label3 = label3;
    _retVal1 = retVal1;
    _retVal2 = retVal2;
    _retVal3 = retVal3;
    _itemCount = 3;
    _selectedItem = 1;
};

void BottomMenu::setupMenu2(char *label1, int retVal1, char *label3, int retVal3) {
    _label1 = label1;
    _label3 = label3;
    _retVal1 = retVal1;
    _retVal3 = retVal3;
    _itemCount = 2;
    _selectedItem = 1;
};


void BottomMenu::setMenuItems(byte itemCount) {
    _itemCount = itemCount;
};

void BottomMenu::setItem1Label(char *label) {
    _label1 = label;
};

void BottomMenu::setItem2Label(char *label) {
    _label2 = label;
};

void BottomMenu::setItem3Label(char *label) {
    _label3 = label;
};

void BottomMenu::setItem1CallbackValue(int retVal) {
    _retVal1 = retVal;
};

void BottomMenu::setItem2CallbackValue(int retVal) {
    _retVal2 = retVal;
};

void BottomMenu::setItem3CallbackValue(int retVal) {
    _retVal3 = retVal;
};

void BottomMenu::showMenu(boolean visible) {    
    _screen->fillArea(_screen->AREA_BOTTOM_MENU, _screen->C_BLACK);

    if (visible) {
        _visible = true;
        _redrawMenu();        
    } else {
        _visible = false;
    }    
};

boolean BottomMenu::isVisible() {
    return _visible;
};

void BottomMenu::selectItem(byte item) {
    _selectedItem = item;
};

void BottomMenu::_redrawMenu() {
        if (_itemCount == 2) {
            _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU_ITEM1, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, (_selectedItem == 1 ? _screen->C_NAV_ACTIVE : _screen->C_NAV_INACTIVE), _label1);
            _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU_ITEM3, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, (_selectedItem == 3 ? _screen->C_NAV_ACTIVE : _screen->C_NAV_INACTIVE), _label3);
        }

        if (_itemCount == 3) {            
            _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU_ITEM1, _screen->TEXTPOSITION_LEFT_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, (_selectedItem == 1 ? _screen->C_NAV_ACTIVE : _screen->C_NAV_INACTIVE), _label1);
            _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU_ITEM2, _screen->TEXTPOSITION_HCENTER_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, (_selectedItem == 2 ? _screen->C_NAV_ACTIVE : _screen->C_NAV_INACTIVE), _label2);
            _screen->drawTextInArea(_screen->AREA_BOTTOM_MENU_ITEM3, _screen->TEXTPOSITION_RIGHT_BOTTOM, true, _screen->TEXTSIZE_MEDIUM, (_selectedItem == 3 ? _screen->C_NAV_ACTIVE : _screen->C_NAV_INACTIVE), _label3);
        }    
}
