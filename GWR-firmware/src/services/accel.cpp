#include "accel.h"

#include <Arduino.h>
#include "ADXL343.h"

#include "config/hardware_conf.h"
#include "config/app_conf.h"
#include "structs/AccelEvent.h"
#include "taskglobals.h"

ADXL343 accel(Pins::XL::CS, Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);

void task_accel(void *pv) {
    static bool printOnNextMutexFail = true;
    
    // setup
    if (xSemaphoreTake(Mutexes::SPI, (TickType_t) 10) == pdTRUE) {

        bool success = accel.init();
        while (!success) {
            debugln("adxl init fail");
            Serial.flush();
            vTaskDelay(500 / portTICK_PERIOD_MS);
            success = accel.init();
        }

        accel.setRange(ADXL343_RANGE_4_G);
        accel.setRate(ADXL343_DATARATE_400_HZ);
        accel.setOffsets(settings.adxl.ofx, settings.adxl.ofy, settings.adxl.ofz);
        
        debugln("adxl init good");
        xSemaphoreGive(Mutexes::SPI);
    }

    // loop
    while (1) {
        if (xSemaphoreTake(Mutexes::SPI, (TickType_t) 10) == pdTRUE) {
            if (xSemaphoreTake(Mutexes::AccelData, (TickType_t) 0) == pdTRUE) {
                accel.getAcceleration3V3(&(g_accelEvent.x), &(g_accelEvent.y), &(g_accelEvent.z));
                xSemaphoreGive(Mutexes::AccelData);
            } else {
                debuglnF("someone else has the accel data semaphore");
            }
            xSemaphoreGive(Mutexes::SPI);

            printOnNextMutexFail = true;
        } else {
            if (printOnNextMutexFail) {
                debugln("adxl was blocked");
                printOnNextMutexFail = false;
            }
        }
        vTaskDelay((1000 / 200) / portTICK_PERIOD_MS); // we could get more data with fewer SPI transfers using FIFO + interrupt
    }

    vTaskDelete(NULL);
}