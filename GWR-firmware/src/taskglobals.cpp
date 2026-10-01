#include "taskglobals.h"

#include <Arduino.h>

#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "util/settings.h"
#include "structs/AudioMessage.h"
#include "structs/UserInputMessage.h"
#include "structs/MouseClickMessage.h"
#include "structs/HapticMessage.h"
#include "structs/DisplayMessage.h"

DeviceMode_t DEVICE_MODE;

USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;

SemaphoreHandle_t Mutexes::SPI       = NULL;
SemaphoreHandle_t Mutexes::USBPORT   = NULL;
SemaphoreHandle_t Mutexes::AccelData = NULL;
SemaphoreHandle_t Mutexes::SDCard    = NULL;

QueueHandle_t q_sfx_tts     = NULL;
QueueHandle_t q_userinput   = NULL;
QueueHandle_t q_mouseclicks = NULL;
QueueHandle_t q_haptic      = NULL;
QueueHandle_t q_display     = NULL;

SettingsConfig settings;
SettingsConfig defaultsettings;

AccelEvent g_accelEvent;
bool g_wifiReady = false;

float g_battery_voltage = VBAT_FULL;

#ifndef ARDUINO_USB_MODE
#error SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#error USB is not in OTG mode
#endif

void initTaskGlobals() {
    // our entry point in setup()
    DEVICE_MODE = INITIALIZING;

    g_accelEvent.x = 0;
    g_accelEvent.y = 0;
    g_accelEvent.z = 0;

    g_wifiReady = false;

    Serial.begin(BAUDRATE_ESP32);
    Keyboard.begin();
    Mouse.begin();
    bool usbsuccess = USB.begin();
    vTaskDelay(max(COMPOSITE_ENUMERATION_DELAY, SERIAL_CONNECT_DELAY));

    // we may access serial printing below this point

    if (!usbsuccess) {
        debuglnF("not plugged into USB?");
    } else {
        debuglnF("USB attached");
    }

    Mutexes::SPI       = xSemaphoreCreateMutex();
    Mutexes::USBPORT   = xSemaphoreCreateMutex();
    Mutexes::AccelData = xSemaphoreCreateMutex();
    Mutexes::SDCard    = xSemaphoreCreateMutex(); // you must take the SPI mutex before and in addition to the SDCard Mutex.

    // queues
    q_userinput   = xQueueCreate(16, sizeof(UserInputMessage));
    q_mouseclicks = xQueueCreate(8, sizeof(MouseclickMessage));
    q_sfx_tts     = xQueueCreate(10, sizeof(AudioMessage));
    q_display     = xQueueCreate(16, sizeof(DisplayMessage));
    q_haptic      = xQueueCreate(2, sizeof(HapticMessage));

    debuglnF("Init complete");
}
