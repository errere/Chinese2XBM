#include "ChineseFS.h"

void ChineseFS::init(fs::FS *f)
{
    _fs = f;
}
uint8_t *ChineseFS::getCharXBM(uint16_t gbk)
{
    _file.seek(getGBKIndex(gbk));
    _file.read(cram, 32);
    return cram;
}

uint8_t ChineseFS::openCharSet(const char *path)
{
    _file = _fs->open(path);
    return 0;
}
uint8_t ChineseFS::closeCharSet()
{
    _file.close();
    return 0;
}

//////////////////////////////////////////////////////////////////////
uint32_t ChineseFS::getGBKIndex(uint16_t gbk)
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