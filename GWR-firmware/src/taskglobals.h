#pragma once

#include <Arduino.h>

#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>

#include "structs/SettingsConfig.h"
#include "structs/AccelEvent.h"

// highest level device mode

enum DeviceMode_t {
    INITIALIZING,
    KEYBOARD,
    JOYSTICK,
    AUDIOIMAGE,
    STUCK,
};

extern DeviceMode_t DEVICE_MODE;

// objects

extern USBHIDKeyboard Keyboard;
extern USBHIDMouse Mouse;

// resources
namespace Mutexes {
    extern SemaphoreHandle_t SPI;
    extern SemaphoreHandle_t USBPORT;
    extern SemaphoreHandle_t AccelData;
    extern SemaphoreHandle_t SDCard;
}

namespace Tasks {
    extern TaskHandle_t soundfx;
    extern TaskHandle_t rgbled;
    extern TaskHandle_t mouse_out;
    extern TaskHandle_t mode;
    extern TaskHandle_t heartbeat;
    extern TaskHandle_t display;
    extern TaskHandle_t buttons;
    extern TaskHandle_t accel;
    extern TaskHandle_t wificonnection;
    extern TaskHandle_t haptics;
}

extern QueueHandle_t q_sfx_tts;
extern QueueHandle_t q_userinput;
extern QueueHandle_t q_mouseclicks;
extern QueueHandle_t q_haptic;
//extern QueueHandle_t q_img;

extern SettingsConfig settings;

extern AccelEvent g_accelEvent;
extern bool g_wifiReady;


void initTaskGlobals();