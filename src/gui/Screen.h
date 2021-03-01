#ifndef Screen_h
#define Screen_h

#include <Arduino.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

//#include "../context/sequencer/Pattern.h"

class Screen {
    public:
        Screen(Adafruit_ST7789 *tft, int BL);  

        typedef struct area {
                int x1;
                int y1;
                int x2;
                int y2;
                boolean transparent;
                uint16_t bgColor;
        } Area;

        enum TextSize
        {
                TEXTSIZE_SMALL = 1,
                TEXTSIZE_MEDIUM = 2,
                TEXTSIZE_LARGE = 3,
        };

        enum TextPosition
        {
                TEXTPOSITION_HCENTER_VCENTER = 1,
                TEXTPOSITION_LEFT_VCENTER = 2,
                TEXTPOSITION_RIGHT_VCENTER = 3,
                
                TEXTPOSITION_HCENTER_TOP = 4,
                TEXTPOSITION_LEFT_TOP = 5,        
                TEXTPOSITION_RIGHT_TOP = 6,        

                TEXTPOSITION_HCENTER_BOTTOM = 7,
                TEXTPOSITION_LEFT_BOTTOM = 8,        
                TEXTPOSITION_RIGHT_BOTTOM = 9        
        }; 

/*         GFXfont FONT_SMALL = &OxygenMono_Regular8pt7b;
         GFXfont FONT_MEDIUM = &OxygenMono_Regular8pt7b;
         GFXfont FONT_LARGE = &OxygenMono_Regular8pt7b;                
*/
        void testBild(const char* text);

        void fillArea(Area area, uint16_t color);
        void clearAreaLTR(Area area, uint16_t color, int delayTime);
        void clearAreaRTL(Area area, uint16_t color, int delayTime);
        void hr(Area area, float vpos, uint16_t color);
        void vr(Area area, float hpos, uint16_t color);
        void drawTextInArea(Area area, TextPosition textPosition, boolean eraseFirst, TextSize textSize, uint16_t color, const char *text);

        void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
        void drawPixel(int x, int y, uint16_t color);

        void loadingScreen(float percent);

/*        
        void showRecordingScreen(String text);
        void drawSampler();
        void drawSampleBank(boolean sampleBankStatus[4][8], byte activeBank);
        void drawSampleSelector(byte s);
        void hideSampleBank();

        void selectNewSample(const char *filename);
        void drawRAWMonoSample44(const char *filename);
        void redrawRAWMonoSample44();
        void drawSamplerLiveMode();
        void drawSampleEditSubmenu(byte activeItem);

        void updateTrimPosition(int encoderValue);

        long getTrimmerStartPositionMs();
        long getTrimmerEndPositionMs();
        bool useMs(); // true == use ms || false == use byte (sample too small to work with ms)

        void drawHint(const char *hint);
*/        
        void drawText(const char *text,int x, int y, uint16_t color, const GFXfont *font);
        void drawText(const char *text,int x, int y);

        uint16_t RGBtoColor(byte r, byte g, byte b);

/*        void initializeGrid(Pattern *pattern); 
        void drawTrackerAtPosition(uint16_t position, Pattern *p, bool editMode, byte highlightEvery);
        void drawCursorAtColumn(byte column);
        void drawBPM(float bpm);
        void drawExtMemPercentage(byte percent);
*/

        // Color definitions

        const uint16_t C_WHITE = RGBtoColor(255, 255, 255);
        const uint16_t C_BLACK = RGBtoColor(0, 0, 0);
        
        const uint16_t C_ORANGE = ST77XX_ORANGE;

        const uint16_t C_NAV_INACTIVE = RGBtoColor(100, 100, 100);
        const uint16_t C_NAV_ACTIVE = RGBtoColor(255, 255, 255);        

        const uint16_t C_SB_FREE = RGBtoColor(18, 222, 38);
        const uint16_t C_SB_USED = RGBtoColor(222, 38, 18);
        
        //const uint16_t C_TRIM_START = RGBtoColor(245, 166, 35);
        const uint16_t C_TRIM_START = C_ORANGE;
        const uint16_t C_TRIM_END = RGBtoColor(80, 227, 194);       

        const uint16_t C_GRID_FULL_NOTE_BG = RGBtoColor(20, 20, 20);
        const uint16_t C_GRID_DARK = RGBtoColor(20, 20, 20);
        const uint16_t C_GRID_BRIGHT = RGBtoColor(150, 150, 150);        
        const uint16_t C_GRID_COUNTER_FONT = RGBtoColor(150, 150, 150);

        const uint16_t C_RECORDING = RGBtoColor(255, 0, 0);

        const uint16_t C_MEMORY_FREE = RGBtoColor(0, 222, 0);
        const uint16_t C_MEMORY_USED = RGBtoColor(255, 0, 0);

        const uint16_t C_STARTUP_BG = C_BLACK;
        const uint16_t C_STARTUP_HR = C_WHITE;
        const uint16_t C_GREY = RGBtoColor(20, 20, 20);
        const uint16_t C_LIGHTGREY = RGBtoColor(150, 150, 150);

        const uint16_t C_WARNING = RGBtoColor(250, 50, 10);

        const uint16_t C_SELECTOR_SELECTED_BG = C_ORANGE;

        // Area definitions
        Area AREA_SCREEN = { 0, 0, 319, 239, false, C_BLACK};  // Fullscreen
        Area AREA_HEADLINE = { 0, 10, 319, 25, false, C_BLACK}; // Headline
        Area AREA_CONTENT = { AREA_SCREEN.x1, AREA_SCREEN.y1, AREA_SCREEN.x2, AREA_SCREEN.y2-25, false, C_BLACK}; // like screen, but without bottom navigation
        Area AREA_BOTTOM_MENU = {0, AREA_SCREEN.y2-25, AREA_SCREEN.x2, AREA_SCREEN.y2, false, C_BLACK};   
        Area AREA_BOTTOM_MENU_ITEM1 = {0, AREA_BOTTOM_MENU.y1, floor(AREA_BOTTOM_MENU.x2*0.33), AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_BOTTOM_MENU_ITEM2 = {AREA_BOTTOM_MENU_ITEM1.x2, AREA_BOTTOM_MENU.y1, floor(AREA_BOTTOM_MENU.x2*0.66), AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_BOTTOM_MENU_ITEM3 = {AREA_BOTTOM_MENU_ITEM2.x2, AREA_BOTTOM_MENU.y1, AREA_BOTTOM_MENU.x2, AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_LOADING_BAR = { 110, AREA_SCREEN.y2/2+10, 210, AREA_SCREEN.y2/2+17, C_BLACK };

    private:
        Adafruit_ST7789 *_tft;
        int16_t _dx = 0;
        int16_t _dy = 0;
        uint16_t _dw = 0;
        uint16_t _dh = 0;

        uint16_t _x, _y = 0;
/*        char _cBuff3[3];
        char _cBuff10[10];

        Pattern *_pattern;

        void _drawTrimMarker(bool draw, bool updateBoth);  // true = draw marker | false = remove marker and paint waveform
        long _trimPositionStartPixel = 0;
        long _trimPositionEndPixel = 319;     
        byte _activeTrimmer = 0;
        int _pixelToMsFactor = 0;
        bool _pixelToMsFactorIdentifier = true; // true == use ms || false == use byte (sample too small to work with ms)

        void _clearWFB(int centerLineY);

        int _pixelMsFactor = 0;
        int _wfb[320][4];  // 0=fromX 1=toX 2=fromY 3=toY // typedef is not working in this case?!?!!!
        int _wfbz[320][4]; // same over here, but for the zoomed view 
        int _magnitudeHeight = 120;    
        int _centerLineY = 95;
*/

        void _setColor(const int *colorArr);
/*
        void _drawEmptyRow(uint16_t row);
        void _drawGridCell(int col, int row, const char *text, uint16_t bgColor);
*/
        // color definitions

};

#endif
