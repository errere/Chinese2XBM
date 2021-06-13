#include "ChineseFS.h"

void ChineseFS::init(fs::FS *f)
{
    _fs = f;
}
uint8_t *ChineseFS::getCharBM(uint16_t gbk)
{
    _file.seek(getGBKIndex(gbk)); //り
    _file.read(cram, 32);
    return cram;
}

uint8_t ChineseFS::openCharSet(const char *path)
{
    _file = _fs->open(path, "r");
    return _file;
}
uint8_t ChineseFS::closeCharSet()
{
    _file.close();
    return 1;
}

/*==========================gbk only api============================*/

//gbk
void ChineseFS::printGBK(uint16_t gb)
{
    if (Cur_x + 16 > LCD_W)
    {
        Cur_x = 0;
        Cur_y += (AsciiDY > 16) ? AsciiDY : 16;
    }
    if (allowBM)
    {
        _lcdDrawBM(Cur_x, Cur_y, 16, 16, getCharBM(gb), _c);
    }
    else
    {
        drawBitmap(Cur_x, Cur_y, getCharBM(gb), 16, 16, _c.fg, _c.bg);
    }
    Cur_x += (AsciiDY > 16) ? AsciiDY : 16;
}
//x,y,gbk
void ChineseFS::printGBK(int16_t x, int16_t y, uint16_t gb)
{
    if (allowBM)
    {
        _lcdDrawBM(x, y, 16, 16, getCharBM(gb), _c);
    }
    else
    {
        drawBitmap(x, y, getCharBM(gb), 16, 16, _c.fg, _c.bg);
    }
}

void ChineseFS::printString(const char *c)
{
    while (*c)
    {
        uint8_t tmp = *c;
        if (tmp < 0xa1)
        {
            if (tmp == '\n')
            {
                Cur_x = 0;
                Cur_y += (AsciiDY > 16) ? AsciiDY : 16;
            }
            else if (allowDrawASC == true)
            {
                _lcdDrawAsc(Cur_x, Cur_y, tmp, _c);
                Cur_x += AsciiDX;
            }
            c++;
        } //if<a1
        else
        {
            uint16_t gb16 = (tmp << 8);
            c++;
            gb16 |= *c;
            printGBK(gb16);
            c++;
        } //else
    }     //while
}

void ChineseFS::printString(String c)
{
    printString(c.c_str());
}

//////////////////////////////////////////////////////////////////////
uint32_t ChineseFS::getGB2312Index(uint16_t gbk)
{

    //f1a9->c1a9
    //c1a8->f1a9
    //c1a9->c1a8
    //sp
    //c1a8:哩 change to f1a9
    if (gbk == 0xc1a8)
    {
        gbk = 0xf1a9;
    }
    //f1a9:瘵 change to c1a9
    else if (gbk == 0xf1a9)
    {
        gbk = 0xc1a9;
    }
    //c1a9:俩 change to c1a8
    else if (gbk == 0xc1a9)
    {
        gbk = 0xc1a8;
    }
    uint8_t msb = (gbk >> 8) & 0xff;
    uint8_t lsb = gbk & 0xff;
    //assert
    if (msb > 0xf7 || lsb < 0xa1 || msb < 0xa1)
    {
        return 0;
    }
    //proc1
    if (msb > 0xa9)
    {
        msb -= 0xb0;
        msb += 12;
        if (msb == 51)
        {
            if (lsb > 0xf9)
            {
                return 0;
            }
        } //fix [0xd7f9,0xd7fe] not char

        lsb -= 0xa1;
        return (gbkCharSetOffset[msb] + lsb) * COL_NUMBER * BYTE_EACH_ROW;
    } //chinese
    else
    {
        msb -= 0xa1;
        //sp for a5,a7 a8
        if (msb == 0x4) //a5
        {
            if (lsb > 0xf6)
                return 0;
            lsb -= 0xa1;
            return (gbkCharSetOffset[symoffset[msb]] + lsb) * COL_NUMBER * BYTE_EACH_ROW;
        }
        else if (msb == 0x6) //a7
        {
            if (lsb > 0xc1)
                return 0;
            lsb -= 0xa1;
            return (gbkCharSetOffset[symoffset[msb]] + lsb) * COL_NUMBER * BYTE_EACH_ROW;
        }
        else if (msb == 0x7) //a8
        {
            if (lsb == 0xc1)
                return 0;

            if (lsb > 0xc1)
                lsb -= 0xa2;
            else
                lsb -= 0xa1;
            return (gbkCharSetOffset[symoffset[msb]] + lsb) * COL_NUMBER * BYTE_EACH_ROW;
        }
        else
        {
            lsb -= 0xa1;
            return (gbkCharSetOffset[symoffset[msb]] + lsb) * COL_NUMBER * BYTE_EACH_ROW;
        }
        return 0;
    } //symble

    return 0;
}
/*
                              a1......................fe                           v
40..........................a0                                                     v
                              a1......................fe                           3
40....................................................fe                           v
40..........................a0                                                     5
40............4f                                                                   v
*/

/*
        a1................a9                                                           v
                      a8..a9                                                           v
                                    b0......................f7                         3
81....a0                                                                               v
                             aa........................................fd              5
                                                                         fe            v

*/
uint32_t ChineseFS::getGBKIndex(uint16_t gbk)
{

    uint8_t msb = (gbk >> 8) & 0xff;
    uint8_t lsb = gbk & 0xff;

    if (msb >= 0x81 && msb <= 0xa0)
    {
        // GBK扩充3区:
        // 0x8140 - 0xA0FE			7808 - 13919				*191, *1.
        if (lsb > 0xfe)
            return 0;
        msb -= 0x81;
        lsb -= 0x40;
        return (((msb * 191) + lsb) + 7808) * 32;
    } //81a0
    else if (msb >= 0xa1 && msb <= 0xa9)
    {
        if (lsb < 0xa1)
        {
            // GBK扩充5区:
            // 0xA840 - 0xA9A0 846 - 1039 * 97, *1.
            if (msb != 0xa9 || msb != 0xa8)
                return 0;
            msb -= 0xa8;
            lsb -= 0x40;
            return (((msb * 97) + lsb) + 846) * 32;
        }
        else
        {
            // 0xA1A1 - 0xA9FE			0 - 845				*94, *1.
            msb -= 0xa1;
            lsb -= 0xa1;
            return (((msb * 94) + lsb) + 0) * 32;
        }
    } //a1a9
    else if (msb >= 0xaa && msb <= 0xfd)
    {
        if (lsb < 0xa1)
        {
            //0xAA40 - 0xFDA0			13920 - 22067				*97, *1.
            msb -= 0xaa;
            lsb -= 0x40;
            return (((msb * 97) + lsb) + 13920) * 32;
        }
        else
        {
            // GBK扩充5区:
            // 0xB0A1 - 0xF7FE			1040 - 7807				*94, *1.
            if (msb < 0xb0 || msb > 0xf7)
                return 0;
            msb -= 0xb0;
            lsb -= 0xa1;
            return (((msb * 94) + lsb) + 1040) * 32;
        }
    } //aafd
    else if (msb == 0xfe)
    {
        // 0xFE40 - 0xFE4F			22068 - 22083				-, *1.
        msb -= 0xfe;
        lsb -= 0x40;
        return (((msb * 1) + lsb) + 22068) * 32;
    } //fe
    return 0;
}

uint32_t ChineseFS::getGBKIndexDEBUG(uint16_t gbk)
{
    return gbk * 32;
}
void ChineseFS::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bgcolor)
{

    int32_t i, j, byteWidth = (w + 7) / 8;

    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7)))
                _lcdDrawPixel(x + i, y + j, color);
            else
                _lcdDrawPixel(x + i, y + j, bgcolor);
        }
    }
}

///////////eof//////////