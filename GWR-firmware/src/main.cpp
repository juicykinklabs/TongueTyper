#include <Arduino.h>
#include <SPI.h>

#include "app_conf.h"
#include "hardware_conf.h"

#include "ADXL343.h"

#include "Audio.h"
#include "SD.h"
#include "FS.h"

#include "SdFat_Adafruit_Fork.h"
#include "Adafruit_ImageReader.h"
#include "Adafruit_GC9A01A.h"
#include "Adafruit_GFX.h"

#include "FreeMono18pt7b.h"
#include "FreeMonoBold24pt7b.h"

SdFat32 SD_as_f32;
SdSpiConfig sdConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(25), &SPI); // just for the imageReader
Adafruit_GC9A01A tft(&SPI, TFT_DC, TFT_CS);
Adafruit_ImageReader reader(SD_as_f32);

ADXL343 accel(ADXL_CS, XIAO_SCK, XIAO_MISO, XIAO_MOSI);
Audio audio;

SemaphoreHandle_t metaSPIbustransactionMutex = NULL;

void my_audio_info(Audio::msg_t m)
{
    //Serial.printf("%s: %s\n", m.s, m.msg);
}

void do_heartbeat(void *pv)
{
    pinMode(LED_BUILTIN, OUTPUT);
    while (1)
    {
        digitalWrite(LED_BUILTIN, millis() % 1000 > 500);
        vTaskDelay((1000 / 10) / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

bool metaTransaction_showImage(const char *filename, bool clear = false)
{
    //  digitalWrite(TFT_RST, LOW);
    //  delay(500);
    //  digitalWrite(TFT_RST, HIGH);

    tft.begin(40000000); // try 25 also

    if (clear)
    {
        tft.fillScreen(GC9A01A_BLACK);
        return true;
    }

    tft.setRotation(3);

    bool success = SD_as_f32.begin(sdConfig);
    if (!success)
    {
        debugln("SD card is no bueno");
    }
    reader.drawBMP(filename, tft, 0, 0, true);
    SD_as_f32.end();

    return success;
}

bool metaTransaction_playSound(const char *filename)
{
    // assumptions
    // sd_cs configured as output
    // this is blocking, if we want to read user input while sound is still playing,
    // we'll need a task
    SPI.end(); // broken without?
    SPI.begin(XIAO_SCK, XIAO_MISO, XIAO_MOSI); // we probably need this
    SPI.setFrequency(1000000); // might not need this
    SD.begin(SD_CS);

    audio.setVolume(18); // 0...21
    uint32_t t_playStart = millis();
    audio.connecttoFS(SD, filename);
    uint32_t t_duration = (audio.getAudioFileDuration() * 1000 + 1000); // must round up
    //audio.isRunning();
    while (millis() - t_playStart < t_duration)
    {
        audio.loop();
        vTaskDelay(1);
    }
    SD.end();

    return true;
}

void do_adxl(void *pv)
{
    static bool printOnNextMutexFail = true;
    
    if (xSemaphoreTake(metaSPIbustransactionMutex, (TickType_t)10) == pdTRUE) {

        bool success = accel.init();
        while (!success) {
            debugln("uhh you're not gonna like this! adxl fucked up :3");
            Serial.flush();
            vTaskDelay(500 / portTICK_PERIOD_MS);
            success = accel.init();
        }
    
        accel.setRange(ADXL343_RANGE_4_G);
        accel.setRate(ADXL343_DATARATE_100_HZ);

        xSemaphoreGive(metaSPIbustransactionMutex);
    }

    while (1)
    {
        if (xSemaphoreTake(metaSPIbustransactionMutex, (TickType_t)10) == pdTRUE)
        {
            double x, y, z, x0, y0, z0;
            accel.getAcceleration3V3(&x, &y, &z);
            accel.getAcceleration(&x0, &y0, &z0);
            x *= 9.81; // convert to m/s^2
            y *= 9.81; // convert to m/s^2
            z *= 9.81; // convert to m/s^2
            x0 *= 9.81; // convert to m/s^2
            y0 *= 9.81; // convert to m/s^2
            z0 *= 9.81; // convert to m/s^2
            debugf("X: %f, Y: %f, Z: %f, X0: %f, Y0: %f, Z0: %f\n", x, y, z, x0, y0, z0);
            
            xSemaphoreGive(metaSPIbustransactionMutex);
            printOnNextMutexFail = true;
        }
        else
        {
            if (printOnNextMutexFail)
            {
                debugln("adxl was blocked");
                printOnNextMutexFail = false;
            }
        }
        vTaskDelay((1000 / 100) / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void setup()
{
    debugStart();

    xTaskCreate(do_heartbeat, "LED heartbeat", 1024, NULL, 1, NULL);

    Audio::audio_info_callback = my_audio_info;

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);

    bool success = audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    if (!success)
    {
        debugln("I2s IO matrix config failed, probably");
        delay(15000);
    }

    //setTFTBrightness(0.35);

    metaSPIbustransactionMutex = xSemaphoreCreateMutex();
    // have to do this AFTER making a spi mutex. duh,
    xTaskCreate(do_adxl, "ADXL34X Function", 8192, NULL, 2, NULL);
}

void loop()
{

    //if (xSemaphoreTake(metaSPIbustransactionMutex, (TickType_t)25) == pdTRUE)
    //{
//
    //    metaTransaction_showImage("1fae4.bmp");
    //    metaTransaction_playSound("fard.mp3");
    //    xSemaphoreGive(metaSPIbustransactionMutex);
    //}
    //else
    //{
    //    debugln("main loop was semaphore blocked for 25 ticks!!!");
    //}
    //vTaskDelay(500 / portTICK_PERIOD_MS);
//
    //if (xSemaphoreTake(metaSPIbustransactionMutex, (TickType_t)25) == pdTRUE)
    //{
//
    //    metaTransaction_showImage("1f975.bmp");
    //    metaTransaction_playSound("quickfart.mp3");
    //    xSemaphoreGive(metaSPIbustransactionMutex);
    //}
    //else
    //{
    //    debugln("main loop was semaphore blocked for 25 ticks!!!");
    //}
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}