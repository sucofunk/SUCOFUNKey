#ifndef Screen_h
#define Screen_h

#include <Arduino.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

class Screen {
    public:
        Screen(Adafruit_ST7789 *tft, int BL_PIN, int BL_brightness);  

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

        void testBild(const char* text);

        void setBacklightBrightness(int brightness);
        void fadeBacklightOut(int delayTime);
        void fadeBacklightIn(int delayTime);
        boolean isBacklightOn();

        void fillArea(Area area, uint16_t color);
        void clearAreaLTR(Area area, uint16_t color, int delayTime);
        void clearAreaRTL(Area area, uint16_t color, int delayTime);
        void hr(Area area, float vpos, uint16_t color);
        void vr(Area area, float hpos, uint16_t color);

        void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
        void drawPixel(int x, int y, uint16_t color);

        void loadingScreen(float percent);
        
        void drawTextInArea(Area area, TextPosition textPosition, boolean eraseFirst, TextSize textSize, boolean monoSpaced, uint16_t color, const char *text);      
        void drawText(const char *text, int x, int y, uint16_t color, const GFXfont *font);
        void drawText(const char *text, int x, int y, TextSize textSize, uint16_t color);
        void drawText(const char *text, int x, int y);

        void setTextSize(TextSize textSize, boolean monoSpaced);

        void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void drawCircle(int16_t x, int16_t y, int16_t r, boolean fill, uint16_t color);

        void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, boolean fill, uint16_t color);

        uint16_t RGBtoColor(byte r, byte g, byte b);

        // Color definitions

        const uint16_t C_WHITE = RGBtoColor(255, 255, 255);
        const uint16_t C_BLACK = RGBtoColor(0, 0, 0);
        
        const uint16_t C_ORANGE = ST77XX_ORANGE;

        const uint16_t C_PEAK_OVER = RGBtoColor(255, 0, 0);
        const uint16_t C_PEAK_MUCH = RGBtoColor(245, 150, 5);
        const uint16_t C_PEAK_OK = RGBtoColor(0, 255, 0);

        const uint16_t C_NAV_INACTIVE = RGBtoColor(255, 255, 255);
        const uint16_t C_NAV_ACTIVE = C_ORANGE;        
        const uint16_t C_NAV_DISABLED = RGBtoColor(100, 100, 100);        


        const uint16_t C_SB_FREE = RGBtoColor(18, 222, 38);
        const uint16_t C_SB_USED = RGBtoColor(222, 38, 18);
        
        //const uint16_t C_TRIM_START = RGBtoColor(245, 166, 35);
        const uint16_t C_TRIM_START = C_ORANGE;
        const uint16_t C_TRIM_END = RGBtoColor(80, 227, 194);       

        const uint16_t C_GRID_FULL_NOTE_BG = RGBtoColor(60, 60, 60);
        const uint16_t C_GRID_DARK = RGBtoColor(60, 60, 60);
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
        const uint16_t C_PLAYER_POSITION = C_ORANGE;

        // Area definitions
        Area AREA_SCREEN = { 0, 0, 319, 239, false, C_BLACK};  // Fullscreen
        Area AREA_HEADLINE = { 0, 0, 319, 25, false, C_BLACK}; // Headline
        Area AREA_CONTENT = { AREA_SCREEN.x1, AREA_SCREEN.y1+25, AREA_SCREEN.x2, AREA_SCREEN.y2-25, false, C_BLACK}; // like screen, but without headline and bottom navigation

        Area AREA_CENTER_TEXT = { AREA_SCREEN.x1, AREA_SCREEN.y1+100, AREA_SCREEN.x2, AREA_SCREEN.y2-100, false, C_ORANGE}; // content line in the middle of the screen for system check

        Area AREA_LOADING_BAR = { 110, AREA_SCREEN.y2/2+20, 210, AREA_SCREEN.y2/2+27, C_BLACK };

        Area AREA_BOTTOM_MENU = {0, AREA_SCREEN.y2-25, AREA_SCREEN.x2, AREA_SCREEN.y2, false, C_BLACK};   
        Area AREA_BOTTOM_MENU_ITEM1 = {0, AREA_BOTTOM_MENU.y1, static_cast<int>(AREA_BOTTOM_MENU.x2*0.33), AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_BOTTOM_MENU_ITEM2 = {AREA_BOTTOM_MENU_ITEM1.x2, AREA_BOTTOM_MENU.y1, static_cast<int>(AREA_BOTTOM_MENU.x2*0.66), AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_BOTTOM_MENU_ITEM3 = {AREA_BOTTOM_MENU_ITEM2.x2, AREA_BOTTOM_MENU.y1, AREA_BOTTOM_MENU.x2, AREA_BOTTOM_MENU.y2, false, C_BLACK};


        Area AREA_BLACK_KEY_MENU = {0, AREA_SCREEN.y2-12, AREA_SCREEN.x2, AREA_SCREEN.y2, false, C_BLACK};
        
        Area AREA_BLACK_KEY_MENU_ITEM1 = {1, AREA_BLACK_KEY_MENU.y1+1, 26, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};
        Area AREA_BLACK_KEY_MENU_ITEM2 = {27, AREA_BLACK_KEY_MENU.y1+1, 52, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};
        Area AREA_BLACK_KEY_MENU_ITEM3 = {53, AREA_BLACK_KEY_MENU.y1+1, 78, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};
        
        Area AREA_BLACK_KEY_MENU_ITEM4 = {99, AREA_BLACK_KEY_MENU.y1+1, 124, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};
        Area AREA_BLACK_KEY_MENU_ITEM5 = {125, AREA_BLACK_KEY_MENU.y1+1, 150, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};

        Area AREA_BLACK_KEY_MENU_ITEM6 = {171, AREA_BLACK_KEY_MENU.y1+1, 196, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};
        Area AREA_BLACK_KEY_MENU_ITEM7 = {197, AREA_BLACK_KEY_MENU.y1+1, 222, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};
        Area AREA_BLACK_KEY_MENU_ITEM8 = {223, AREA_BLACK_KEY_MENU.y1+1, 248, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};

        Area AREA_BLACK_KEY_MENU_ITEM9 = {268, AREA_BLACK_KEY_MENU.y1+1, 293, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};
        Area AREA_BLACK_KEY_MENU_ITEM10 = {294, AREA_BLACK_KEY_MENU.y1+1, 319, AREA_BLACK_KEY_MENU.y2, false, C_BLACK};


        Area AREA_SEQUENCER_OPTIONS = {AREA_SCREEN.x1, AREA_CONTENT.y1+125, AREA_SCREEN.x2, AREA_BOTTOM_MENU.y1, false, C_BLACK}; 
        Area AREA_SEQUENCER_OPTIONS_SAMPLENAME = {AREA_SEQUENCER_OPTIONS.x1, AREA_SEQUENCER_OPTIONS.y1, AREA_SEQUENCER_OPTIONS.x2, AREA_SEQUENCER_OPTIONS.y1+25, false, C_BLACK};
        
        Area AREA_SEQUENCER_OPTION1 = {AREA_SEQUENCER_OPTIONS.x1, AREA_SEQUENCER_OPTIONS_SAMPLENAME.y2+2, 80, AREA_SEQUENCER_OPTIONS.y2, true, C_BLACK};
        
        uint16_t optionheight = static_cast<int>(AREA_SEQUENCER_OPTION1.y1+2+((AREA_SEQUENCER_OPTION1.y2 - AREA_SEQUENCER_OPTION1.y1)/2));
        Area AREA_SEQUENCER_OPTION1_BAR = {AREA_SEQUENCER_OPTIONS.x1+1, AREA_SEQUENCER_OPTION1.y1, 80, optionheight, true, C_BLACK};        

        uint16_t barVCENTEROffset = static_cast<int>((optionheight - AREA_SEQUENCER_OPTION1.y1)/2);
        Area AREA_SEQUENCER_OPTION1_VOLUME = {AREA_SEQUENCER_OPTION1_BAR.x1+40-33, AREA_SEQUENCER_OPTION1_BAR.y1+barVCENTEROffset-4, AREA_SEQUENCER_OPTIONS.x1+40+32, AREA_SEQUENCER_OPTION1_BAR.y1+barVCENTEROffset+4, true, C_BLACK};

        Area AREA_SEQUENCER_OPTION2 = {AREA_SEQUENCER_OPTION1.x2+1, AREA_SEQUENCER_OPTIONS_SAMPLENAME.y2+2, 160, AREA_SEQUENCER_OPTIONS.y2, false, C_BLACK};
        Area AREA_SEQUENCER_OPTION2_BAR = {AREA_SEQUENCER_OPTION1_BAR.x2+1, AREA_SEQUENCER_OPTION1.y1, 160, optionheight, false, C_BLACK};
        Area AREA_SEQUENCER_OPTION2_PANNING = {AREA_SEQUENCER_OPTION2_BAR.x1+40-32, AREA_SEQUENCER_OPTION2_BAR.y1+barVCENTEROffset-4, AREA_SEQUENCER_OPTION2_BAR.x1+40+32, AREA_SEQUENCER_OPTION2_BAR.y1+barVCENTEROffset+4, true, C_BLACK};
        
        Area AREA_SEQUENCER_OPTION3 = {AREA_SEQUENCER_OPTION2.x2, AREA_SEQUENCER_OPTIONS_SAMPLENAME.y2+2, 240, AREA_SEQUENCER_OPTIONS.y2, true, C_BLACK};
        Area AREA_SEQUENCER_OPTION3_BAR = {AREA_SEQUENCER_OPTION2_BAR.x2+1, AREA_SEQUENCER_OPTION1.y1, 240, optionheight, false, C_BLACK};
        
        Area AREA_SEQUENCER_OPTION4 = {AREA_SEQUENCER_OPTION3.x2, AREA_SEQUENCER_OPTIONS_SAMPLENAME.y2+2, AREA_SEQUENCER_OPTIONS.x2, AREA_SEQUENCER_OPTIONS.y2, true, C_BLACK};
        Area AREA_SEQUENCER_OPTION4_BAR = {AREA_SEQUENCER_OPTION3_BAR.x2+1, AREA_SEQUENCER_OPTION1.y1, AREA_SEQUENCER_OPTIONS.x2, optionheight , false, C_BLACK};


    private:
        Adafruit_ST7789 *_tft;
        int16_t _dx = 0;
        int16_t _dy = 0;
        uint16_t _dw = 0;
        uint16_t _dh = 0;

        uint16_t _x, _y = 0;

        int _BL_PIN;
        int _BL_brightness = 110;
        boolean _BL_on = true;

        void _setColor(const int *colorArr);
};

#endif
