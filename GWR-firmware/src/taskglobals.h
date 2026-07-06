#pragma once

#include <Arduino.h>
#include "structs/SettingsConfig.h"
#include "structs/AccelEvent.h"

// resources
namespace Mutexes {
    extern SemaphoreHandle_t SPI;
    extern SemaphoreHandle_t SerialPrint;
    extern SemaphoreHandle_t AccelData;
    extern SemaphoreHandle_t SDCard;
}

namespace Tasks {
    extern TaskHandle_t soundfx;
    extern TaskHandle_t rgbled;
    extern TaskHandle_t mouse_out;
    extern TaskHandle_t keyboard_out;
    extern TaskHandle_t heartbeat;
    extern TaskHandle_t display;
    extern TaskHandle_t buttons;
    extern TaskHandle_t accel;
    extern TaskHandle_t wificonnection;
}
extern QueueHandle_t q_sfx_tts;

extern SettingsConfig settings;

extern AccelEvent g_accelEvent;
extern bool g_wifiReady;