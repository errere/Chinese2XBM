#include <inttypes.h>
#include <Arduino.h>
#include "FS.h"

//16BIT config
#define BYTE_EACH_ROW 2
#define COL_NUMBER 16

class ChineseFS
{
private:
    const uint8_t symoffset[9]  = {
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

    const uint16_t gbkCharSetOffset[84]  = {
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

    uint8_t cram[32];

    fs::FS *_fs;
    fs::File _file;

public:
    void init(fs::FS *f);
    uint8_t *getCharXBM(uint16_t gbk);

    uint8_t openCharSet(const char *path);
    uint8_t closeCharSet();
};
