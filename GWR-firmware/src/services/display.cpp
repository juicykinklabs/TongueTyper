#include "display.h"

#include <Arduino.h>

#include "SdFat_Adafruit_Fork.h"
#include "Adafruit_GC9A01A.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ImageReader.h"

#include "FreeMono18pt7b.h"
#include "FreeMonoBold24pt7b.h"

#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "structs/DisplayMessage.h"
#include "util/settings.h"
#include "taskglobals.h"

void setTFTBrightness(double b) {
#ifdef PCB_REVISION
#if PCB_REVISION >= 1
    pinMode(Pins::TFT::BL, OUTPUT); // OUTPUT or ANALOG?
    // analogWriteResolution(TFT_PIN, 8);
    // analogWriteFrequency(TFT_PIN, 100); // these were giving errors so lets leave em commented
    analogWrite(Pins::TFT::BL, (int) (255 * (b)));
#endif
#endif
}

void task_displayImageOrText(void *pv) {
    // recv path from a queue,
    // put that image on the tft display

    Adafruit_GC9A01A tft(Pins::TFT::CS, Pins::TFT::DC); // resets will be done manually
    SdSpiConfig sdConfig(Pins::SD::CS, SHARED_SPI, SPISpeed::SD, &SPI);
    SdFat32 SD_as_FAT32;
    Adafruit_ImageReader reader(SD_as_FAT32);

    tft.begin(SPISpeed::TFT);

#ifdef PCB_REVISION
#if PCB_REVISION >= 1
    pinMode(Pins::TFT::RST, OUTPUT);
    pinMode(Pins::TFT::BL, OUTPUT);
    digitalWrite(Pins::TFT::RST, LOW);
    vTaskDelay(500);
    digitalWrite(Pins::TFT::RST, HIGH);
#endif
#endif

    setTFTBrightness(0);
    tft.fillScreen(GC9A01A_BLACK);

    DisplayMessage cmd;

    while (1) {

        if (xQueueReceive(q_display, (void *) &cmd, portMAX_DELAY)) {
            debugln("display recv");

            tft.begin(SPISpeed::TFT); // untraceable crashes without this reinitialization.
                                      // probably SPI timers being clobbered?
            tft.setRotation(1);       // todo change to tft.setRotation(3);

            if (cmd.inst == DisplayInstruction::JUST_CLEAR) {
                tft.fillScreen(GC9A01A_BLACK);
                setTFTBrightness(0);
                debuglnF("cleared screen");
            } else if (cmd.inst == DisplayInstruction::DRAW_IMAGE) {
                debugln("DRAW IMAGE");
                setTFTBrightness(0); // black out the display before drawing next image

                bool success = SD_as_FAT32.begin(sdConfig);
                if (!success) {
                    debuglnF("SD as FAT32 error");
                    continue; // <-- todo this continue may cause perma lock
                }
                debugln("about to attempt draw");
                debugln(cmd.data);
                SD_as_FAT32.ls(LS_R);
                ImageReturnCode rc = reader.drawBMP(cmd.data, tft, 0, 0, true);
                setTFTBrightness(settings.disp.brightness);
                if (rc != IMAGE_SUCCESS) {
                    success = false;
                    debuglnF("ImageReader error");
                }
                SD_as_FAT32.end();
            } else if (cmd.inst == DisplayInstruction::SHOW_STRING) {
                // we receive a string from the main task. the last character
                // is shown big in the middle, and subtitles are shown with
                // as many 'previous' characters as will fit on the display.
                debugln("SHOW STRING");
                static const size_t LINE_1_MAXCHARS = 9;
                static const size_t LINE_2_MAXCHARS = 7;
                int16_t x1, y1;                                        // temp variables for determining character bounds of the font
                uint16_t w, h;                                         // temp variables for determining character bounds of the font
                int16_t centeredX, centeredY;                          // where to draw the big letter
                int16_t y_offset_big                    = -38;         // offset the big letter. negative for up, positive for down
                int16_t y_offset_subtitle               = 62;          // where to draw subtitles. negative for up, positive for down
                char centeredString[2]                  = "_";         // goes in the middle
                char subtitleLine1[LINE_1_MAXCHARS + 1] = "         "; // 9 chars + nt
                char subtitleLine2[LINE_2_MAXCHARS + 1] = "       ";   // 7 chars + nt

                // *** Draw blank canvas ***
                tft.fillScreen(GC9A01A_WHITE);
                tft.setTextColor(GC9A01A_BLACK);
                tft.fillCircle(120, 120 + y_offset_big, 80, GC9A01A_BLUE);
                tft.fillCircle(120, 120 + y_offset_big, 76, GC9A01A_WHITE);

                size_t textLength = strnlen(cmd.data, DISPLAYMESSAGE_DATA_SZ);
                if (textLength <= 0) {
                    continue; // <-- todo this continue may cause perma lock
                }

                // *** Draw big char in the middle ***
                tft.setTextSize(5); // matters for calls to setFont, getTextBounds, ...
                tft.setFont(&FreeMonoBold24pt7b);

                centeredString[0] = cmd.data[textLength - 1];
                tft.getTextBounds(centeredString, 0, 0, &x1, &y1, &w, &h);
                centeredX = tft.width() / 2 - (x1 + w / 2);
                centeredY = y_offset_big + tft.height() / 2 - (y1 + h / 2);
                tft.setCursor(centeredX, centeredY);
                tft.print(centeredString);

                // *** Draw subtitles ***
                // first, extract from data String
                // if the second line starts with a space, we can omit the space, to show an extra letter!
                debuglnF("about to run strncpy :3 hope i dont crash ^_^");

                char *dp_copy = cmd.data;
                size_t dp_sz  = strlen(dp_copy);

                // trim leading spaces, check size
                if (dp_copy[0] == ' ') {
                    dp_copy++;
                }
                dp_sz = strlen(dp_copy);
                if (dp_sz <= 0) {
                    continue; // <-- todo this continue may cause perma lock
                }

                if (dp_sz < LINE_1_MAXCHARS) {
                    // fits on one line
                    strcpy(subtitleLine1, dp_copy);

                } else {
                    // larger than LINE_1_MAXCHARS, trim the string to 16 chars or less,
                    while ((dp_sz = strlen(dp_copy)) > (LINE_1_MAXCHARS + LINE_2_MAXCHARS)) {
                        dp_copy++;
                    }
                    // trim leading space. we know size is >>1 so no need to check it again.
                    if (dp_copy[0] == ' ') {
                        dp_copy++;
                    }
                    strncpy(subtitleLine1, dp_copy, LINE_1_MAXCHARS);
                    dp_copy += LINE_1_MAXCHARS;

                    // trim leading spaces of second line, check size
                    if (dp_copy[0] == ' ') {
                        dp_copy++;
                    }
                    dp_sz = strlen(dp_copy);
                    if (dp_sz <= 0) {
                        continue; // <-- todo this continue may cause perma lock
                    }

                    strncpy(subtitleLine2, dp_copy, LINE_2_MAXCHARS);
                }

                debuglnF("we made it chat");
                debugln(subtitleLine1);
                debugln(subtitleLine2);

                tft.setTextSize(1); // matters for calls to setFont, getTextBounds, ...
                tft.setFont(&FreeMono18pt7b);

                tft.getTextBounds(subtitleLine1, 0, 0, &x1, &y1, &w, &h);
                centeredX = tft.width() / 2 - (x1 + w / 2);
                centeredY = y_offset_subtitle + tft.height() / 2 - (y1 + h / 2);
                tft.setCursor(centeredX, centeredY);
                tft.print(subtitleLine1);

                tft.getTextBounds(subtitleLine2, 0, 0, &x1, &y1, &w, &h);
                centeredX = tft.width() / 2 - (x1 + w / 2);
                centeredY = y_offset_subtitle + (h + 2) + tft.height() / 2 - (y1 + h / 2); // add additional pixels (h+2)
                tft.setCursor(centeredX, centeredY);
                tft.print(subtitleLine2);
            }
        }
        vTaskDelay(1);
    }
    vTaskDelete(NULL); // <-- bitch
}