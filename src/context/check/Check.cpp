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
   
#include <Arduino.h>
#include "../../hardware/Sucofunkey.h"
#include "../../gui/Screen.h"
#include "Check.h"

Check::Check(Sucofunkey *keyboard, Screen *screen) {
    _keyboard = keyboard;    
    _screen = screen;
}

void Check::handleEvent(Sucofunkey::keyQueueStruct event) {
    if (event.pressed) {

        if (event.type == _keyboard->KEY_NOTE) {
            _keyboard->setLEDState(_keyboard->getLEDPinByEventKey(event.index), false);            
        }

      switch(event.index) {        
        case Sucofunkey::FUNCTION:
            drawInfotext("funk");
            break;
        case Sucofunkey::MENU:
            drawInfotext("menu");
            break;
        case Sucofunkey::SET:
            drawInfotext("set");
            break;
        case Sucofunkey::ZOOM:
            drawInfotext("select");
            break;                 
        case Sucofunkey::PLAY:
            _keyboard->setLEDState(_keyboard->LED_PLAY, false);
            drawInfotext("play");
            playSound();
            break;            
        case Sucofunkey::PAUSE:
            drawInfotext("stop");
            stopSound();
            break;
        case Sucofunkey::RECORD:
            _keyboard->setLEDState(_keyboard->LED_RECORD, false);
            drawInfotext("record");
            break;
        case Sucofunkey::INPUTSELECTOR:
            _keyboard->setLEDState(_keyboard->LED_INPUT_LINE, false);
            _keyboard->setLEDState(_keyboard->LED_INPUT_MIC, false);
            _keyboard->setLEDState(_keyboard->LED_INPUT_RESAMPLE, false);
            drawInfotext("input");
            break;            
        case Sucofunkey::CURSOR_LEFT:
            drawInfotext("left");
            break;            
        case Sucofunkey::CURSOR_RIGHT:
            drawInfotext("right");
            break;            
        case Sucofunkey::CURSOR_UP:
            drawInfotext("up");
            break;            
        case Sucofunkey::CURSOR_DOWN:
            _keyboard->setLEDState(_keyboard->LED_BANK_1, false);
            _keyboard->setLEDState(_keyboard->LED_BANK_2, false);            
            _keyboard->setLEDState(_keyboard->LED_BANK_3, false);
            drawInfotext("down");
            break;
        case Sucofunkey::ENCODER_1_PUSH:
            encoderAction(1, 1);
            break;
        case Sucofunkey::ENCODER_2_PUSH:
            encoderAction(2, 1);
            break;
        case Sucofunkey::ENCODER_3_PUSH:
            encoderAction(3, 1);
            break;
        case Sucofunkey::ENCODER_4_PUSH:
            encoderAction(4, 1);
            break;
      }
    }

    if (event.type == Sucofunkey::ENCODER) {
        switch(event.index) {
        case Sucofunkey::ENCODER_1:
            encoderAction(1, event.pressed ? 2 : 3);
            break;            
        case Sucofunkey::ENCODER_2:
            encoderAction(2, event.pressed ? 2 : 3);
            break;            
        case Sucofunkey::ENCODER_3:
            encoderAction(3, event.pressed ? 2 : 3);
            break;            
        case Sucofunkey::ENCODER_4:
            encoderAction(4, event.pressed ? 2 : 3);
            break;
        } 
    }

}

void Check::setActive(boolean active) {
  if (active) {
    _isActive = true;
        
    turnAllLEDsOn();

    _screen->fillArea(_screen->AREA_SCREEN, _screen->C_BLACK);

    _screen->drawText("encoder", 130, 15);

    _screen->drawCircle(44 + 12, 45, 12, false, _screen->C_ORANGE);
    _screen->drawCircle(88 + 36, 45, 12, false, _screen->C_ORANGE);
    _screen->drawCircle(132 + 60, 45, 12, false, _screen->C_ORANGE);
    _screen->drawCircle(176 + 84, 45, 12, false, _screen->C_ORANGE);

    _screen->drawText("fader", 220, 173);
    _screen->drawText("volume", 220, 213);

    _screen->drawTextInArea(_screen->AREA_CENTER_TEXT, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_SMALL, false, _screen->C_WHITE, "no function key pressed");

  } else {
    _isActive = false;
    _keyboard->setBank(0);
  }
}

long Check::receiveTimerTick() {
    showFaderPosition();
    showVolume();
    return 100000;
}


void Check::drawInfotext(char *msg) {
    _screen->drawTextInArea(_screen->AREA_CENTER_TEXT, _screen->TEXTPOSITION_HCENTER_VCENTER, true, _screen->TEXTSIZE_SMALL, false, _screen->C_WHITE, msg);
}

void Check::encoderAction(byte encoder, byte action) {
    int xOffset = (encoder * 24) + (encoder * 44) - 12;
    
    switch(action) {
        case 1:
            // Encoder Button push
            _screen->drawCircle(xOffset, 45, 12, true, _screen->C_ORANGE);
            break;
        case 2:
            // Encoder turned right        
            _screen->drawCircle(xOffset, 45, 12, true, _screen->RGBtoColor(255, 0, 0));
            break;
        case 3:
            // Encoder turned left
            _screen->drawCircle(xOffset, 45, 12, true, _screen->RGBtoColor(0, 255, 0));
            break;
    }
}


void Check::turnAllLEDsOn() {
            _keyboard->setLEDState(_keyboard->LED_F_1, true);
            _keyboard->setLEDState(_keyboard->LED_FS_1, true);
            _keyboard->setLEDState(_keyboard->LED_G_1, true);
            _keyboard->setLEDState(_keyboard->LED_GS_1, true);
            _keyboard->setLEDState(_keyboard->LED_A_1, true);
            _keyboard->setLEDState(_keyboard->LED_AS_1, true);
            _keyboard->setLEDState(_keyboard->LED_B_1, true);
            _keyboard->setLEDState(_keyboard->LED_C_1, true);
            _keyboard->setLEDState(_keyboard->LED_CS_1, true);
            _keyboard->setLEDState(_keyboard->LED_D_1, true);
            _keyboard->setLEDState(_keyboard->LED_DS_1, true);
            _keyboard->setLEDState(_keyboard->LED_E_1, true);
            _keyboard->setLEDState(_keyboard->LED_F_2, true);
            _keyboard->setLEDState(_keyboard->LED_FS_2, true);
            _keyboard->setLEDState(_keyboard->LED_G_2, true);
            _keyboard->setLEDState(_keyboard->LED_GS_2, true);
            _keyboard->setLEDState(_keyboard->LED_A_2, true);
            _keyboard->setLEDState(_keyboard->LED_AS_2, true);
            _keyboard->setLEDState(_keyboard->LED_B_2, true);
            _keyboard->setLEDState(_keyboard->LED_C_2, true);
            _keyboard->setLEDState(_keyboard->LED_CS_2, true);
            _keyboard->setLEDState(_keyboard->LED_D_2, true);
            _keyboard->setLEDState(_keyboard->LED_DS_2, true);
            _keyboard->setLEDState(_keyboard->LED_E_2, true);

           _keyboard->setLEDState(_keyboard->LED_PLAY, true);
           _keyboard->setLEDState(_keyboard->LED_RECORD, true);
  
           _keyboard->setLEDState(_keyboard->LED_BANK_1, true);
           _keyboard->setLEDState(_keyboard->LED_BANK_2, true);
           _keyboard->setLEDState(_keyboard->LED_BANK_3, true);

           _keyboard->setLEDState(_keyboard->LED_INPUT_LINE, true);
           _keyboard->setLEDState(_keyboard->LED_INPUT_MIC, true);
           _keyboard->setLEDState(_keyboard->LED_INPUT_RESAMPLE, true);

           _keyboard->switchFaderLED(true);           
}


void Check::showFaderPosition() {
    int v = _keyboard->getFaderValue(0, 101);

    if (_lastFaderPosition != v) { 
        _lastFaderPosition = v; 

        // remove old fader indicator line
        _screen->fillRect(110, 162, 100, 8, _screen->C_BLACK);
        _screen->fillRect(110, 171, 100, 8, _screen->C_BLACK);

        // draw fader range
        _screen->drawLine(110, 170, 210, 170, _screen->C_LIGHTGREY);

        // draw new fader indicator
        _screen->drawLine(110+v, 162, 110+v, 178, _screen->C_WHITE);
    };    
}


void Check::showVolume() {
    // volume potentiometer at pin 22
    int v = 100-(analogRead(22)/1023.0)*100;

    if (_lastVolume != v) { 
        _lastVolume = v; 

        // remove old volume indicator line
        _screen->fillRect(110, 202, 101, 8, _screen->C_BLACK);
        _screen->fillRect(110, 211, 101, 8, _screen->C_BLACK);

        // draw volume range
        _screen->drawLine(110, 210, 210, 210, _screen->C_LIGHTGREY);

        // draw new volume indicator
        _screen->drawLine(110+v, 202, 110+v, 218, _screen->C_WHITE);
    };
}

void Check::playSound() {

}

void Check::stopSound() {

}
