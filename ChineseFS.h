#include <inttypes.h>
#include <Arduino.h>
#include "FS.h"

//16BIT config
#define BYTE_EACH_ROW 2
#define COL_NUMBER 16

typedef struct
{
    uint16_t fg;
    uint16_t bg;
} ChineseColor_t;

class ChineseFS
{
private:
    const uint8_t symoffset[9] = {
        0, //a1
        1, //2
        2, //3
        3, //4
        5, //5
        6, //6
        8, //7
        9, //a8
        11 //a9
    };

    const uint16_t gbkCharSetOffset[84] = {
        //a1...a9
        //0
        0,
        93,
        169,
        237,
        320,
        331,
        417,
        478,
        487,
        520, //9
        552,
        613,
        //b0...bf
        //12
        702,
        796,
        890,
        984,
        1078,
        1172,
        1266,
        1360,
        1454,
        1548,
        1642,
        1736,
        1830,
        1924,
        2018,
        2112,
        //c0...cf
        //56
        2206,
        2300,
        2394,
        2488,
        2582,
        2676,
        2770,
        2864,
        2958,
        3052,
        3146,
        3240,
        3334,
        3428,
        3522,
        3616,

        //d0...df([d7fa,d7fe] not define)
        //88
        3710,
        3804,
        3898,
        3992,
        4086,
        4180,
        4274,
        4368,
        4457,
        4551,
        4645,
        4739,
        4833,
        4927,
        5021,
        5115,

        //e0...ef
        //120
        5209,
        5303,
        5397,
        5491,
        5585,
        5679,
        5773,
        5867,
        5961,
        6055,
        6149,
        6243,
        6337,
        6431,
        6525,
        6619,

        //f0...f7
        //152
        6713,
        6807,
        6901,
        6995,
        7089,
        7183,
        7277,
        7371

    };

    uint32_t getGBKIndex(uint16_t gbk);

    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bgcolor);

    uint8_t cram[32];

    fs::FS *_fs;
    fs::File _file;

    bool allowBM = false;
    bool allowDrawASC = false;

    //x,y,w,h,buf,color[2]
    using lcdDrawBMCallBack_t = void(int16_t, int16_t, int16_t, int16_t, uint8_t *, ChineseColor_t);
    //x,y,color
    using lcdDrawPixel_t = void(int16_t, int16_t, uint16_t);
    //x,y,ascii,color[2]
    using lcdDrawAsc = void(int16_t, int16_t, char, ChineseColor_t);

    //high level :use device draw BM
    lcdDrawBMCallBack_t *_lcdDrawBM;
    //Low level : draw pixel
    lcdDrawPixel_t *_lcdDrawPixel;
    //x,y,ascii,color[2]
    lcdDrawAsc *_lcdDrawAsc;

    ChineseColor_t _c;
    uint8_t AsciiDX = 0;
    uint8_t AsciiDY = 0;
    int16_t Cur_x = 0;
    int16_t Cur_y = 0;
    int16_t LCD_W = 0x7fff;

public:
    void init(fs::FS *f);
    uint8_t *getCharBM(uint16_t gbk);

    uint8_t openCharSet(const char *path);
    uint8_t closeCharSet();

    /*==========================gbk only api============================*/
    //gbk
    void printGBK(uint16_t);
    //x,y,gbk
    void printGBK(int16_t, int16_t, uint16_t);

    //disp
    void setLCD_W(int16_t w)
    {
        LCD_W = w;
    }
    void setCursor(int16_t x, int16_t y)
    {
        Cur_x = x;
        Cur_y = y;
    }
    int16_t getCursorX() { return Cur_x; }
    int16_t getCursorY() { return Cur_y; }
    void setAsciiPrg(uint8_t dx, uint8_t dy)
    {
        AsciiDX = dx;
        AsciiDY = dy;
    }
    void setTextColor(uint16_t fgcolor, uint16_t bgcolor)
    {
        _c.bg = bgcolor;
        _c.fg = fgcolor;
    }
    //set callback
    void setBMCallback(lcdDrawBMCallBack_t *cb)
    {
        _lcdDrawBM = cb;
        allowBM = true;
    }
    void setDrawPixelCallback(lcdDrawPixel_t *cb)
    {
        _lcdDrawPixel = cb;
    }
    //asc
    void setDrawASCCallback(lcdDrawAsc *cb)
    {
        _lcdDrawAsc = cb;
        allowDrawASC = true;
    }

    //finally main api
    void printString(const char * c);
    void printString(String c);
};
