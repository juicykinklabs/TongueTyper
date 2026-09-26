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

extern QueueHandle_t q_sfx_tts;
extern QueueHandle_t q_userinput;
extern QueueHandle_t q_mouseclicks;
extern QueueHandle_t q_haptic;
extern QueueHandle_t q_display;

extern SettingsConfig settings;

extern AccelEvent g_accelEvent;
extern bool g_wifiReady;

/**
 * @brief System initialization and global variable initialization routine
 * 
 * @details Starts USB devices as well as our semaphores and queues
 */
void initTaskGlobals();