/*  
 Test the tft.print() viz the libraries embedded write() function

 This sketch used font 2, 4, 7
 
 Make sure all the required fonts are loaded by editing the
 User_Setup.h file in the TFT_eSPI library folder.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
 */

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

#include "ChineseFS.h"

#include "FS.h"
#include "SPIFFS.h"
#include "gbkstr.h"
#define FORMAT_SPIFFS_IF_FAILED true

void writeGBK(fs::FS &fs, const char *path, uint16_t x, uint16_t y, uint16_t W, const char *g, uint8_t len);
uint32_t getGBKIndex(uint16_t gbk);

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#define TFT_GREY 0x5AEB // New colour
ChineseFS hz;

void setup(void)
{

  Serial.begin(115200);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  hz.init(&SPIFFS);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(0x0000);
  pinMode(14, OUTPUT);
  digitalWrite(14, 1);
}

void loop()
{
  uint16_t iii;

  hz.openCharSet("/chineseSong.bin");

  for (uint16_t j = 0; j < 10; j++)
  {
    for (uint16_t i = 0; i < 10; i++)
    {
      iii = gb1[(2 * i)];
      iii <<= 8;
      iii |= gb1[(2 * i) + 1];
      tft.drawBitmap((i * 16), (j*16), hz.getCharXBM(iii), 16, 16, 0xffff);
    }
  }

  hz.closeCharSet();

  // for (size_t j = 0; j < 10; j++)
  // {
  //   writeGBK(SPIFFS, "/chineseSong.bin", 0, (j * 16), 16, gb1, 10);
  // }
delay(100);
  tft.fillScreen(0x0000);
}
