#include <SPI.h>
#include <SD.h>

#include "helper/AudioResources.h"
#include "hardware/Sucofunkey.h"
#include "helper/FSIO.h"
#include "helper/SampleFSIO.h"
#include "gui/Screen.h"
#include "gui/screens/StartupScreen.h"
#include "context/home/Home.h" 
#include "context/sampler/Sampler.h" 
#include "context/sequencer/Sequencer.h"
#include "context/synth/Synth.h"
#include "context/live/Live.h"
#include "context/settings/Settings.h"
#include "context/recorder/Recorder.h"
#include <Audio.h>

#include <Adafruit_ST7789.h>

int PIN_SUCOKEY_INT_1 = 40; // MCP 1
int PIN_SUCOKEY_INT_2 = 41; // ENCODERS (MCP 3)
int PIN_SUCOKEY_INT_3 = 33; // MCP 2
int PIN_SUCOKEY_INT_4 = 32; // MCP 3 KEYS

int PIN_SCREEN_SCK = 27;
int PIN_SCREEN_MOSI = 26; // DIN
int PIN_SCREEN_CS = 38;
int PIN_SCREEN_DC = 39;
int PIN_SCREEN_RST = 35;
int PIN_SCREEN_BL = 34;

// Function definitions
void handleKeyboardEventQueue(void);
void changeContext(byte context);

enum AppContext {
  HOME = 0, SAMPLER = 1, SEQUENCER = 2, SYNTH = 3, LIVE = 4, SETTINGS = 5, STARTUP = 6, RECORDER = 7
};

AppContext currentAppContext; // stores the current active "module" context (HOME|SAMPLER|...)

char songsBasePath[8] = "/SONGS/";
char activeSongName[9];
char activeSongPath[21];

// Sample Memory initialization one psram chip with 8mb added to teensy board
// ToDo: Add line for 16 mb / 2 chips
EXTMEM unsigned int extmemArray[2097152]; 

// Interval Timer for playback pattern, metronome, etc..
IntervalTimer globalTickTimer;
volatile boolean ticked = false;
long globalTickInterval = 1000000; // intervall in microseconds -> starts at 1s
long globalTickIntervalNew;
void globalTick();

// Interval Timer for recording
IntervalTimer globalTickTimerRec;
volatile boolean tickedRec = false;
long globalTickIntervalRec = 1000000; // intervall in microseconds -> starts at 1s
long globalTickIntervalRecNew;
void globalTickRec();


AudioResources audioResources;

// Hardware SPI for LCD Screen
Adafruit_ST7789 tft(&SPI1, PIN_SCREEN_CS, PIN_SCREEN_DC, PIN_SCREEN_RST);

// Initializing the "keyboard"
Sucofunkey keyboard(PIN_SUCOKEY_INT_1, PIN_SUCOKEY_INT_2, PIN_SUCOKEY_INT_3, PIN_SUCOKEY_INT_4);
// Initializing GUI System
Screen screen(&tft, PIN_SCREEN_BL);

FSIO fsio;
SampleFSIO sfsio(extmemArray, sizeof(extmemArray), &screen);

Home  homeContext(&keyboard, &screen, activeSongPath);
Sampler samplerContext(&keyboard, &screen, &fsio, &sfsio, &audioResources);
Recorder recorderContext(&keyboard, &screen, &fsio, &sfsio, &audioResources);
Sequencer sequencerContext(&keyboard, &screen, &sfsio, extmemArray, &audioResources);

Synth synthContext(&keyboard, &screen);
Live liveContext(&keyboard, &screen);
Settings settingsContext(&keyboard, &screen);
StartupScreen startupContext(&keyboard, &screen, &fsio, activeSongName);


// --- START Audio Connections
// unfortunately this is not initializable in the AudioResources class?!?

AudioConnection          patchCord1(audioResources.wavetable1, 0, audioResources.mixerWav1L, 0);
AudioConnection          patchCord2(audioResources.wavetable1, 0, audioResources.mixerWav1R, 0);
AudioConnection          patchCord3(audioResources.wavetable2, 0, audioResources.mixerWav1L, 1);
AudioConnection          patchCord4(audioResources.wavetable2, 0, audioResources.mixerWav1R, 1);
AudioConnection          patchCord5(audioResources.wavetable3, 0, audioResources.mixerWav1L, 2);
AudioConnection          patchCord6(audioResources.wavetable3, 0, audioResources.mixerWav1R, 2);
AudioConnection          patchCord7(audioResources.wavetable6, 0, audioResources.mixerWav2L, 1);
AudioConnection          patchCord8(audioResources.wavetable6, 0, audioResources.mixerWav2R, 1);
AudioConnection          patchCord9(audioResources.wavetable7, 0, audioResources.mixerWav2L, 2);
AudioConnection          patchCord10(audioResources.wavetable7, 0, audioResources.mixerWav2R, 2);
AudioConnection          patchCord11(audioResources.wavetable8, 0, audioResources.mixerWav2L, 3);
AudioConnection          patchCord12(audioResources.wavetable8, 0, audioResources.mixerWav2R, 3);
AudioConnection          patchCord13(audioResources.playMem7, 0, audioResources.mixerMem2L, 2);
AudioConnection          patchCord14(audioResources.playMem7, 0, audioResources.mixerMem2R, 2);
AudioConnection          patchCord15(audioResources.wavetable4, 0, audioResources.mixerWav1L, 3);
AudioConnection          patchCord16(audioResources.wavetable4, 0, audioResources.mixerWav1R, 3);
AudioConnection          patchCord17(audioResources.playMem1, 0, audioResources.mixerMem1L, 0);
AudioConnection          patchCord18(audioResources.playMem1, 0, audioResources.mixerMem1R, 0);
AudioConnection          patchCord19(audioResources.playMem8, 0, audioResources.mixerMem2L, 3);
AudioConnection          patchCord20(audioResources.playMem8, 0, audioResources.mixerMem2R, 3);
AudioConnection          patchCord21(audioResources.wavetable5, 0, audioResources.mixerWav2L, 0);
AudioConnection          patchCord22(audioResources.wavetable5, 0, audioResources.mixerWav2R, 0);
AudioConnection          patchCord23(audioResources.playMem2, 0, audioResources.mixerMem1L, 1);
AudioConnection          patchCord24(audioResources.playMem2, 0, audioResources.mixerMem1R, 1);
AudioConnection          patchCord25(audioResources.playMem3, 0, audioResources.mixerMem1L, 2);
AudioConnection          patchCord26(audioResources.playMem3, 0, audioResources.mixerMem1R, 2);
AudioConnection          patchCord27(audioResources.playMem4, 0, audioResources.mixerMem1L, 3);
AudioConnection          patchCord28(audioResources.playMem4, 0, audioResources.mixerMem1R, 3);
AudioConnection          patchCord29(audioResources.playMem5, 0, audioResources.mixerMem2L, 0);
AudioConnection          patchCord30(audioResources.playMem5, 0, audioResources.mixerMem2R, 0);
AudioConnection          patchCord31(audioResources.playMem6, 0, audioResources.mixerMem2L, 1);
AudioConnection          patchCord32(audioResources.playMem6, 0, audioResources.mixerMem2R, 1);
AudioConnection          patchCord33(audioResources.audioInput, 0, audioResources.recordMixer, 0);
AudioConnection          patchCord34(audioResources.audioInput, 0, audioResources.mixerOutL, 0);
AudioConnection          patchCord35(audioResources.audioInput, 1, audioResources.recordMixer, 1);
AudioConnection          patchCord36(audioResources.audioInput, 1, audioResources.mixerOutR, 0);
AudioConnection          patchCord37(audioResources.playSdRaw, 0, audioResources.mixerSDL, 0);
AudioConnection          patchCord38(audioResources.playSdRaw, 0, audioResources.mixerSDR, 0);
AudioConnection          patchCord39(audioResources.playMem, 0, audioResources.mixerSDL, 1);
AudioConnection          patchCord40(audioResources.playMem, 0, audioResources.mixerSDR, 1);
AudioConnection          patchCord41(audioResources.recordMixer, audioResources.queue1);
AudioConnection          patchCord42(audioResources.recordMixer, audioResources.peak1);
AudioConnection          patchCord43(audioResources.mixerSDL, 0, audioResources.mixerPreOutL, 2);
AudioConnection          patchCord44(audioResources.mixerSDR, 0, audioResources.mixerPreOutR, 2);
AudioConnection          patchCord45(audioResources.mixerMem2L, 0, audioResources.mixerMemL, 1);
AudioConnection          patchCord46(audioResources.mixerMem2R, 0, audioResources.mixerMemR, 1);
AudioConnection          patchCord47(audioResources.mixerMem1L, 0, audioResources.mixerMemL, 0);
AudioConnection          patchCord48(audioResources.mixerMem1R, 0, audioResources.mixerMemR, 0);
AudioConnection          patchCord49(audioResources.mixerWav2L, 0, audioResources.mixerWavL, 1);
AudioConnection          patchCord50(audioResources.mixerWav2R, 0, audioResources.mixerWavR, 1);
AudioConnection          patchCord51(audioResources.mixerWav1L, 0, audioResources.mixerWavL, 0);
AudioConnection          patchCord52(audioResources.mixerWav1R, 0, audioResources.mixerWavR, 0);
AudioConnection          patchCord53(audioResources.mixerMemL, 0, audioResources.mixerPreOutL, 0);
AudioConnection          patchCord54(audioResources.mixerMemR, 0, audioResources.mixerPreOutR, 0);
AudioConnection          patchCord55(audioResources.mixerWavL, 0, audioResources.mixerPreOutL, 1);
AudioConnection          patchCord56(audioResources.mixerWavR, 0, audioResources.mixerPreOutR, 1);
AudioConnection          patchCord57(audioResources.mixerPreOutL, 0, audioResources.recordMixer, 2);
AudioConnection          patchCord58(audioResources.mixerPreOutL, 0, audioResources.mixerOutL, 1);
AudioConnection          patchCord59(audioResources.mixerPreOutR, 0, audioResources.recordMixer, 3);
AudioConnection          patchCord60(audioResources.mixerPreOutR, 0, audioResources.mixerOutR, 1);
AudioConnection          patchCord61(audioResources.mixerOutL, 0, audioResources.audioOutput, 0);
AudioConnection          patchCord62(audioResources.mixerOutR, 0, audioResources.audioOutput, 1);

// --- END of AudioConnections


void setup() {
  Serial.begin(9600);

  boolean ok = true;
  //strcpy(activeSongPath, songsBasePath); // init main path.. song directory will be concatenated, when available  

  tft.init(240, 320, SPI_MODE0);
  tft.setRotation(1);
  tft.fillScreen(screen.C_STARTUP_BG);

  // Screeen Backlight regulator
  // needs to be connected to a PWM PIN!!! 34 is NOT!!!
  //pinMode(PIN_SCREEN_BL, OUTPUT);
  //analogWrite(PIN_SCREEN_BL, 100); 
  currentAppContext = AppContext::STARTUP;
  startupContext.showLogo();

  keyboard.switchLEDsOff();
  keyboard.setBank(1);

  AudioMemory(50);

  // Enable the audio shield, select input and enable output
  audioResources.audioShield.enable();

  keyboard.setInput(Sucofunkey::INPUT_NONE);
  audioResources.muteInput();
  audioResources.muteResampling();
  audioResources.audioShield.volume(0.6);  //0.0-1.0

  // erase psram (sample memory)
  for (unsigned long i=0; i<sizeof(extmemArray)/4; i++) {
    extmemArray[i] = 0;
  }

  delay(200);

  // enable SD card
  if (!(SD.begin(BUILTIN_SDCARD))) {    
      startupContext.showMessage("SD card not available", true);
      ok = false;
  } else {
      startupContext.showMessage("OK", false);
  }

  // start globalTickInterval Timer
  globalTickTimer.begin(globalTick, globalTickInterval);
  globalTickTimerRec.begin(globalTickRec, globalTickIntervalRec);

  delay(500);
  
  if (ok) {
    startupContext.transitionToSelection();
  }

}


void sendEventToActiveContext(Sucofunkey::keyQueueStruct event) {
  switch(currentAppContext) {
    case AppContext::HOME: 
            homeContext.handleEvent(event);
            break;
    case AppContext::SAMPLER:
            samplerContext.handleEvent(event);
            break;
    case AppContext::SEQUENCER:
            sequencerContext.handleEvent(event);
            break;
    case AppContext::SYNTH:
            synthContext.handleEvent(event);
            break;
    case AppContext::LIVE:
            liveContext.handleEvent(event);
            break;
    case AppContext::SETTINGS:
              settingsContext.handleEvent(event);
            break;
    case AppContext::STARTUP:
            startupContext.handleEvent(event);
            break;
    case AppContext::RECORDER:
            recorderContext.handleEvent(event);
            break;
    default:
            break;
  }
}

// --- START Timing ---

void globalTick() {
  ticked = true;
}

void globalTickRec() {
  tickedRec = true;
}


void sendTickToActiveContext() {
  switch(currentAppContext) {
    case  AppContext::SAMPLER:
           globalTickIntervalNew = samplerContext.receiveTimerTick();
          break;
    case  AppContext::SEQUENCER:
           //globalTickIntervalNew = sequencerContext.receiveTimerTick();
          break;
    case  AppContext::STARTUP:
           globalTickIntervalNew = startupContext.receiveTimerTick();
          break;
    default:
          break;
  }    

  globalTickIntervalNew = sequencerContext.receiveTimerTick();

  if (globalTickIntervalNew != globalTickInterval) {
    globalTickInterval = globalTickIntervalNew;
    globalTickTimer.update(globalTickInterval);
  }
}

// --- END Timing ---

int z = 0;

void loop() {
  // recording? -> has highest priority -> store record buffer to sd..
  if (recorderContext.currentState == recorderContext.RECORDER_RECORDING) recorderContext.continueRecording();
  z++;

  // handle encoder and keyboard events.. but divide into three loops to not disrupt recording
  if (z == 1) keyboard.hasEncoderChange();
  if (z == 2) keyboard.hasKeyPressed();
  if (z == 3) {
    handleKeyboardEventQueue();
    z = 0;
  }

  if (tickedRec) {
    tickedRec = false;
    globalTickIntervalRecNew = recorderContext.receiveTimerTick();
    
    if (globalTickIntervalRecNew != globalTickIntervalRec) {
      globalTickIntervalRec = globalTickIntervalRecNew;
      globalTickTimerRec.update(globalTickIntervalRec);
    }  
  }

  // handle timer
  if (ticked) {
    ticked=false;
    sendTickToActiveContext();
  }
}


void changeContext(AppContext context) {    
    homeContext.setActive(false);
    samplerContext.setActive(false);

    if (currentAppContext == SEQUENCER && sequencerContext.isPlaying() && context == SAMPLER) {
      // warning?
      // or fix it with two seperated timers.. but maybe we need them later for something else..
    } 
    else {
      sequencerContext.setActive(false);
    }
        
    synthContext.setActive(false);
    liveContext.setActive(false);
    settingsContext.setActive(false);
    recorderContext.setActive(false);

    switch (context) {
      case  AppContext::HOME: 
                  homeContext.setActive(true);
                  currentAppContext = HOME;
                  break;
      case  AppContext::SAMPLER: 
                  if (!recorderContext.isRecording() && !sequencerContext.isPlaying()) {
                    samplerContext.setActive(true);
                    currentAppContext = SAMPLER;
                  }                  
                  break;
      case  AppContext::SEQUENCER: 
                  sequencerContext.setActive(true);
                  currentAppContext = SEQUENCER;
                  break;
      case  AppContext::SYNTH: 
                  synthContext.setActive(true);
                  currentAppContext = SYNTH;
                  break;
      case  AppContext::LIVE: 
                  liveContext.setActive(true);
                  currentAppContext = LIVE;
                  break;
      case  AppContext::SETTINGS: 
                  settingsContext.setActive(true);
                  currentAppContext = SETTINGS;
                  break;                  
      case  AppContext::RECORDER: 
                  recorderContext.setActive(true);
                  currentAppContext = RECORDER;
                  break;                  
      default: 
                  break;                  
    }    
}

void handleKeyboardEventQueue() {
  bool preCheck = false; // did we already process a key press in a "precondition", e.g. go to main menu?

  while(keyboard.hasEvents()) {
    Sucofunkey::keyQueueStruct event = keyboard.getNextEvent();

/*    Serial.print(event.index);
    Serial.print("::");
    Serial.print(event.pressed);
    Serial.print("::");
    Serial.println(event.type);
*/

    if (event.type == Sucofunkey::EVENT_APPLICATION) {
      switch(event.index) {
        case Sucofunkey::SONGSELECTED:
          // generate new _activeSongPath after selecting or creating a song
          strcpy(activeSongPath, songsBasePath);
          strcat(activeSongPath, activeSongName);
          sfsio.setSongPath(activeSongPath);
          screen.loadingScreen(0.0);
          sfsio.writeAllSamplesToWaveformBuffer();
          changeContext(AppContext::HOME);          
          break;

        case Sucofunkey::RECORDED:
          changeContext(AppContext::SAMPLER);
          break;

        default:
          break;
      }      
    }

    // Encoder pushed -> with FN in every context: change context .. in home screen the same without FN OR Main menu OR Settings
    if (currentAppContext!=STARTUP && event.pressed && (event.type == Sucofunkey::KEY_FN_OPERATION || (currentAppContext == AppContext::HOME && event.type == Sucofunkey::KEY_OPERATION) || event.index == Sucofunkey::FN_FUNCTION || event.index == Sucofunkey::MENU_MENU)) {
      switch (event.index) {
        case Sucofunkey::ENCODER_1_PUSH:  
        case Sucofunkey::FN_ENCODER_1_PUSH:  
              changeContext(SAMPLER);
              preCheck = true;
              break;
        case Sucofunkey::ENCODER_2_PUSH:
        case Sucofunkey::FN_ENCODER_2_PUSH:          
              changeContext(SEQUENCER);
              preCheck = true;
              break;             
        case Sucofunkey::ENCODER_3_PUSH:  
        case Sucofunkey::FN_ENCODER_3_PUSH:        
              changeContext(SYNTH);
              preCheck = true;
              break; 
        case Sucofunkey::ENCODER_4_PUSH:
        case Sucofunkey::FN_ENCODER_4_PUSH:          
              changeContext(LIVE);
              preCheck = true;
              break;
        case Sucofunkey::FN_FUNCTION:  
              changeContext(SETTINGS);
              preCheck = true;
              break;
        case Sucofunkey::MENU_MENU:  
              changeContext(HOME);
              preCheck = true;
              break;
      }
    }

    if (!preCheck && currentAppContext != STARTUP && event.type == Sucofunkey::KEY_OPERATION) {    
      
      if (event.index == Sucofunkey::RECORD && event.pressed) { 
        if (currentAppContext != RECORDER && !recorderContext.isRecording()) {
          changeContext(RECORDER);
          preCheck = true;
        }
        if (currentAppContext != RECORDER && recorderContext.isRecording()) {          
          recorderContext.stopRecording();
          preCheck = true;
        }
      } 
    }

    if (!preCheck) {
      sendEventToActiveContext(event);
    }   
  }
}