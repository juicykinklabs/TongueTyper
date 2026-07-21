#pragma once

#include <Arduino.h>

#include "ADXL343.h"

#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "structs/AccelEvent.h"

#include "taskglobals.h"

/**
 * @brief Perform a one-time, no-turn, single-point calibration of the ADXL343
 *
 * @details takes SPI mutex during this operation, prints the values to serial
 *          starts its own accelerometer instance
 *
 */
inline void performNTSP() {
    debuglnF("Starting calibration");
    debuglnF("Ensure device is laying flat in Z_+1g field");
    ADXL343 accel_cal(Pins::ACCEL::CS, Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);

    if (xSemaphoreTake(Mutexes::SPI, (TickType_t) 50) == pdTRUE) {

        if (!accel_cal.init()) {
            debuglnF("calibration fail");
            return;
        }

        // adxl sample rate is 200 Hz. (must be >100Hz)
        accel_cal.setRange(ADXL343_RANGE_4_G);
        accel_cal.setRange(ADXL343_DATARATE_200_HZ);
        accel_cal.setOffsets(0, 0, 0);

        vTaskDelay(2000);
        // collect 0.1 sec worth, or more, data
        // todo: set offset registers to zero; need library functionality.

        int32_t accumulatorX            = 0;
        int32_t accumulatorY            = 0;
        int32_t accumulatorZ            = 0;
        int16_t bufferX                 = 0;
        int16_t bufferY                 = 0;
        int16_t bufferZ                 = 0;
        const uint32_t samples          = ceil((200.0 * max(0.1, 0.50)));
        const TickType_t delayTimeTicks = ceil((1000.0 / 200.0)) / portTICK_PERIOD_MS;

        TickType_t tasktimer_start = xTaskGetTickCount();
        for (int i = 0; i < samples; i++) {

            accel_cal.getAccelerationRaw(&bufferX, &bufferY, &bufferZ);
            accumulatorX += bufferX;
            accumulatorY += bufferY;
            accumulatorZ += bufferZ;
            if (xTaskDelayUntil(&tasktimer_start, delayTimeTicks) == pdFALSE) {
                debuglnF("calibration not delayed?!");
            }
        }

        xSemaphoreGive(Mutexes::SPI);

        debugf("Result after %d samples\n", samples);
        debugf("xCounts: %d, yCounts: %d, zCounts: %d\n", accumulatorX, accumulatorY, accumulatorZ);
        // values for X0g, Y0g, Zplus1g, in gs
        double Sz      = 256;
        double X0g     = accumulatorX / samples;
        double Y0g     = accumulatorY / samples;
        double Zplus1g = accumulatorZ / samples;
        double Z0g     = Zplus1g - Sz;
        debugf("Measured (lsb avg): x0=%.3f, y0=%.3f, z+1=%.3f, z0=%.3f\n", X0g, Y0g, Zplus1g, Z0g);

        int8_t XOFFSET, YOFFSET, ZOFFSET; // to be placed in registers
        // 4 => 15.6/3.9 mg/lsb
        // expected to multiply by -1: but it only seems correct without the negation?
        // XOFFSET = -1*round(X0g / 4);
        // YOFFSET = -1*round(Y0g / 4);
        // ZOFFSET = -1*round(Z0g / 4);
        XOFFSET = 1 * round(X0g / 4);
        YOFFSET = 1 * round(Y0g / 4);
        ZOFFSET = 1 * round(Z0g / 4);

        debugf("Offsets (LSB): X=%d Y=%d Z=%d\n", XOFFSET, YOFFSET, ZOFFSET);

        settings.adxl.ofx = XOFFSET;
        settings.adxl.ofy = YOFFSET;
        settings.adxl.ofz = ZOFFSET;

        // then save the settings back in main
    } else {
        debugF("NTSP: Couldn't get semaphore in time.");
    }
}
