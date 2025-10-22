/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2025 by Marc Berendes (marc @ sucofunk.com)
    
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
   
   #ifndef MultiSampleStructure_h
   #define MultiSampleStructure_h
   
   #include <Arduino.h>
   
   class MultiSampleStructure {
       public:
           MultiSampleStructure() {};
   
/*           enum shiftAction {
               UP = 1,
               RIGHT = 2,
               DOWN = 3,
               LEFT = 4
           };
      
           typedef struct  {
               byte sampleNumber = 255; // 1..72 sample, 255 == nothing, 254 == stop sample playback, 253 = change velocity/panning
               byte stereoPosition = 64; // 0 = 100% left | 64 = center | 127 = 100% right 
               byte velocity = 64;      // 0..127 -> standard: 64, as defined in midi standard for keyboards without velocity
               byte baseMidiNote = 60; // ToDo: move definition of base note to sampler and add it as meta data to save  RAM
               byte pitchedNote = 60;
               byte probability = 100; // probability of sample to play 0..100
               byte swing = 0; // level -> 0..11 for 1/12 of a playback tick (64th note) AND Swing group combined in one byte (4 Bit Level | 4 Bit Group) -> Level = 2, Group = 8 => 0010|1000
               byte reverse = false;
           } sampleStruct;
*/   
  
/*           boolean *getSamplesUsed();
           boolean samplesUsed[72] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                       false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
                                       false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
   
           void setPlayBackSpeed(float bpm);
           float getPlayBackSpeed();
   
           Swing* getSwing() {
               return &_swing;
           }
*/   
   
       private:
//           uint8_t BLOCKSIZE = 128;


    typedef struct {
        byte sampleID;
        byte chopIndex;
        long offsetFromZero;
    } _positionStruct;

    // zeigerverkettete Liste, die auf Nachfolger zeigt???
   
   };
   
   #endif