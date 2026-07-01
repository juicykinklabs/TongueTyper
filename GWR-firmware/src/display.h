#pragma once

#include "Adafruit_GC9A01A.h"
#include "Adafruit_GFX.h"

#include "FreeMono18pt7b.h"
#include "FreeMonoBold24pt7b.h"


void demoBigTextDisplay(Adafruit_GC9A01A &tft)
{
  tft.fillScreen(GC9A01A_WHITE);
  tft.setTextColor(GC9A01A_BLACK);

  int16_t x1, y1;
  uint16_t w, h;
  int16_t centeredX, centeredY;

  static char cString[2] = "_";

  static char subLine1[10] = "         "; // 9 chars + nt
  static char subLine2[8] = "       ";    // 7 chars + nt
  static char demoText[] = "ABC123 THIS IS A DEMONSTRATION OF THE GWR SYSTEM. :D THIS IS ONLY A TEST!~~";
  int16_t y_offset_big = -38;     // negative for up, positive for down
  int16_t y_offset_subtitle = 62; // negative for up, positive for down

  for (int i = 0; i < strlen(demoText); i++)
  {
    char c = demoText[i];
    uint32_t t_start = millis(); // for timing synchronization

    cString[0] = c;

    tft.setTextSize(5); // matters for this call!
    tft.setFont(&FreeMonoBold24pt7b);

    tft.getTextBounds(cString, 0, 0, &x1, &y1, &w, &h);

    // draw a character centered with border
    tft.fillScreen(GC9A01A_WHITE);

    tft.fillCircle(120, 120 + y_offset_big, 80, GC9A01A_BLUE);
    tft.fillCircle(120, 120 + y_offset_big, 76, GC9A01A_WHITE);

    centeredX = tft.width() / 2 - (x1 + w / 2);
    centeredY = y_offset_big + tft.height() / 2 - (y1 + h / 2);

    tft.setCursor(centeredX, centeredY);

    tft.print(cString);

    // place a string at the bottom

    tft.setTextSize(1); // matters for this call!
    tft.setFont(&FreeMono18pt7b);

    tft.getTextBounds(subLine1, 0, 0, &x1, &y1, &w, &h);
    centeredX = tft.width() / 2 - (x1 + w / 2);
    centeredY = y_offset_subtitle + tft.height() / 2 - (y1 + h / 2);
    tft.setCursor(centeredX, centeredY);
    tft.print(subLine1);

    tft.getTextBounds(subLine2, 0, 0, &x1, &y1, &w, &h);
    centeredX = tft.width() / 2 - (x1 + w / 2);
    centeredY = y_offset_subtitle + (h + 2) + tft.height() / 2 - (y1 + h / 2); // add additional line (h+2)
    tft.setCursor(centeredX, centeredY);
    tft.print(subLine2);

    // rotate subtitle
    for (int i = 0; i < strlen(subLine1) - 1; i++)
    {
      subLine1[i] = subLine1[i + 1];
    }
    subLine1[strlen(subLine1) - 1] = subLine2[0];
    for (int i = 0; i < strlen(subLine2) - 1; i++)
    {
      subLine2[i] = subLine2[i + 1];
    }
    subLine2[strlen(subLine2) - 1] = c;

    while ((millis() - t_start) < 750)
    {
      delay(1);
    }
  }
}