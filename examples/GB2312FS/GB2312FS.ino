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

#include "gbkstr.h"
#define FORMAT_SPIFFS_IF_FAILED true

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#define TFT_GREY 0x5AEB // New colour
ChineseFS hz;

void drawBMCallback(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *buf, ChineseColor_t c)
{
  tft.drawBitmap(x, y, buf, w, h, c.fg, c.bg);
}

// void drawPixelCallBack(int16_t x, int16_t y, uint16_t c)
// {
//   tft.drawPixel(x, y, c);
// }

void drawASCCallback(int16_t x, int16_t y, char asc, ChineseColor_t c)
{
  //tft.drawChar(x, y, asc, c.fg, c.bg, tft.textfont);
  //tft.drawChar(asc,x,y);
  tft.setTextColor(c.fg, c.bg);
  tft.setCursor(hz.getCursorX(), hz.getCursorY(), 2);
  tft.print(asc);
  hz.setCursor(tft.getCursorX(), tft.getCursorY());
}

void setup(void)
{

  Serial.begin(115200);
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  hz.init(&SPIFFS);

  hz.setLCD_W(320);
  hz.setAsciiPrg(8, 16);
  hz.setDrawASCCallback(drawASCCallback);
  hz.setBMCallback(drawBMCallback);
  //hz.setDrawPixelCallback(drawPixelCallBack);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(0x0000);
  pinMode(21, OUTPUT);
  digitalWrite(21, 1);
}

void loop()
{
  hz.openCharSet("/chineseSong.bin");
  hz.setCursor(1, 10);
  hz.setTextColor(0xffff, 0x0000);
  hz.printString(gb1);
  hz.printString(gbstr);
  hz.closeCharSet();
  delay(1);
}
