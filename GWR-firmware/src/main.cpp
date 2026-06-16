#include "SPI.h"

#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "SdFat_Adafruit_Fork.h"
#include "Adafruit_ImageReader.h"

#include "FreeMono18pt7b.h"
#include "FreeMonoBold24pt7b.h"

#define TFT_DC D1
#define TFT_CS D2
#define TFT_RST D3

#define XIAO_SCK D8
#define XIAO_MISO D9
#define XIAO_MOSI D10

#define SD_CS D4

#define BUTTON1 D5
#define BUTTON2 D6
#define BUTTON3 D7

SdFat32 SD;
Adafruit_GC9A01A tft(TFT_CS, TFT_DC); // not using hardware reset because it wasn't working WITH it
Adafruit_ImageReader reader(SD);

// temporary emoji filenames
const char *emojiNames[] =
    {"1f351.bmp",
     "1f36a.bmp",
     "1f419.bmp",
     "1f438.bmp",
     "1f440.bmp",
     "1f47d.bmp",
     "1f604.bmp",
     "1f605.bmp",
     "1f60c.bmp",
     "1f60e.bmp",
     "1f60f.bmp",
     "1f614.bmp",
     "1f616.bmp",
     "1f61b.bmp",
     "1f61c.bmp",
     "1f621.bmp",
     "1f622.bmp",
     "1f624.bmp",
     "1f62b.bmp",
     "1f633.bmp",
     "1f641.bmp",
     "1f644.bmp",
     "1f910.bmp",
     "1f913.bmp",
     "1f914.bmp",
     "1f916.bmp",
     "1f920.bmp",
     "1f922.bmp",
     "1f923.bmp",
     "1f924.bmp",
     "1f928.bmp",
     "1f92a.bmp",
     "1f970.bmp",
     "1f971.bmp",
     "1f972.bmp",
     "1f975.bmp",
     "1f976.bmp",
     "1f97a.bmp",
     "1f98b.bmp",
     "1fae3.bmp",
     "1fae4.bmp",
     "1faea.bmp"}; //sz: 42 temp


void clearTFT()
{
  // clear the TFT to prepare for use, also sets desired rotation
  tft.setRotation(3);
  tft.fillScreen(GC9A01A_BLACK);
}

void setTFTBrightness(float b) {
  const uint8_t TFT_PIN = D0;
  pinMode(TFT_PIN, OUTPUT); // OUTPUT or ANALOG?
  // analogWriteResolution(TFT_PIN, 8);
  // analogWriteFrequency(TFT_PIN, 100); // these were giving errors so lets leave em commented
  analogWrite(TFT_PIN, (int) (255 * (b)));
}

void printDirectory(File32 dir, int depth = 0)
{
  while (true)
  {
    File32 entry = dir.openNextFile();
    if (!entry)
      break;
    for (int i = 0; i < depth; i++)
      Serial.print("  ");
    char namebuf[32] = "\0";
    entry.getName(namebuf, 32);
    Serial.print(namebuf);
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, depth + 1);
    }
    else
    {
      Serial.print("  ");
      Serial.println(entry.size());
    }
    entry.close();
  }
}


void demoBigTextDisplay() {
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
void setup()
{
  Serial.begin(115200);

  delay(1000);
  Serial.println("GC9A01A Test!");
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  tft.begin(40000000); // confirmed to work at up to 25MHz, sometimes up to 40MHz
  // digitalWrite(TFT_RST, LOW);
  // delay(500);
  // digitalWrite(TFT_RST, HIGH);

  clearTFT();

  delay(100);
  SdSpiConfig sdConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(25), &SPI);
  bool success = SD.begin(sdConfig);
  delay(100);

  if (!success)
  {
    Serial.println("SD card is fucked");
  }

  File32 root = SD.open("/");
  if (!root)
  {
    Serial.println("Failed to open root directory!");
  }
  printDirectory(root);
  root.close();

  delay(500);
}

void loop(void)
{

  delay(1000);
  setTFTBrightness(0.15);
  // todo: make customizable borders with circles. eg trans flag?
  char currentFile[10];
  for (int x = 0; x < 42; x++) {
    strcpy(currentFile, emojiNames[x]);
    reader.drawBMP(currentFile, tft, 0, 0, true);
  }
}
