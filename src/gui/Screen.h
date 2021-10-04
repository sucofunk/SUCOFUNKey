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


        void fillArea(Area area, uint16_t color);
        void clearAreaLTR(Area area, uint16_t color, int delayTime);
        void clearAreaRTL(Area area, uint16_t color, int delayTime);
        void hr(Area area, float vpos, uint16_t color);
        void vr(Area area, float hpos, uint16_t color);
        void drawTextInArea(Area area, TextPosition textPosition, boolean eraseFirst, TextSize textSize, uint16_t color, const char *text);

        void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
        void drawPixel(int x, int y, uint16_t color);

        void loadingScreen(float percent);
      
        void drawText(const char *text,int x, int y, uint16_t color, const GFXfont *font);
        void drawText(const char *text,int x, int y);

        void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void drawCircle(int16_t x, int16_t y, int16_t r, boolean fill, uint16_t color);

        uint16_t RGBtoColor(byte r, byte g, byte b);

        // Color definitions

        const uint16_t C_WHITE = RGBtoColor(255, 255, 255);
        const uint16_t C_BLACK = RGBtoColor(0, 0, 0);
        
        const uint16_t C_ORANGE = ST77XX_ORANGE;

        const uint16_t C_PEAK_OVER = RGBtoColor(255, 0, 0);
        const uint16_t C_PEAK_MUCH = RGBtoColor(245, 150, 5);
        const uint16_t C_PEAK_OK = RGBtoColor(0, 255, 0);

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
        const uint16_t C_PLAYER_POSITION = C_ORANGE;

        // Area definitions
        Area AREA_SCREEN = { 0, 0, 319, 239, false, C_BLACK};  // Fullscreen
        Area AREA_HEADLINE = { 0, 0, 319, 25, false, C_BLACK}; // Headline
        Area AREA_CONTENT = { AREA_SCREEN.x1, AREA_SCREEN.y1+25, AREA_SCREEN.x2, AREA_SCREEN.y2-25, false, C_BLACK}; // like screen, but without bottom navigation
        Area AREA_BOTTOM_MENU = {0, AREA_SCREEN.y2-25, AREA_SCREEN.x2, AREA_SCREEN.y2, false, C_BLACK};   
        Area AREA_BOTTOM_MENU_ITEM1 = {0, AREA_BOTTOM_MENU.y1, static_cast<int>(AREA_BOTTOM_MENU.x2*0.33), AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_BOTTOM_MENU_ITEM2 = {AREA_BOTTOM_MENU_ITEM1.x2, AREA_BOTTOM_MENU.y1, static_cast<int>(AREA_BOTTOM_MENU.x2*0.66), AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_BOTTOM_MENU_ITEM3 = {AREA_BOTTOM_MENU_ITEM2.x2, AREA_BOTTOM_MENU.y1, AREA_BOTTOM_MENU.x2, AREA_BOTTOM_MENU.y2, false, C_BLACK};
        Area AREA_LOADING_BAR = { 110, AREA_SCREEN.y2/2+20, 210, AREA_SCREEN.y2/2+27, C_BLACK };

    private:
        Adafruit_ST7789 *_tft;
        int16_t _dx = 0;
        int16_t _dy = 0;
        uint16_t _dw = 0;
        uint16_t _dh = 0;

        uint16_t _x, _y = 0;

        int _BL_PIN;
        int _BL_brightness = 110;

        void _setColor(const int *colorArr);
};

#endif
