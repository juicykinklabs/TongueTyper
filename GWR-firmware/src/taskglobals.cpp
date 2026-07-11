#include "taskglobals.h"

#include <Arduino.h>

#include "config/app_conf.h"
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

AccelEvent g_accelEvent;
bool g_wifiReady = false;

#ifndef ARDUINO_USB_MODE
#error SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#error USB is not in OTG mode
#endif

void initTaskGlobals() {
    // our entry point in setup()
    DEVICE_MODE = INITIALIZING;

    Serial.begin(BAUDRATE_ESP32);
    Keyboard.begin();
    Mouse.begin();
    bool usbsuccess = USB.begin();
    vTaskDelay(max(3000, SERIAL_CONNECT_DELAY)); // "composite enumeration delay" for computer to realize theres a usb device

    // we may access serial printing below this point

    if (!usbsuccess) {
        debuglnF("not plugged into USB?");
    } else {
        debuglnF("USB attached");
    }
 
    Mutexes::SPI       = xSemaphoreCreateMutex();
    Mutexes::USBPORT   = xSemaphoreCreateMutex();
    Mutexes::AccelData = xSemaphoreCreateMutex();
    Mutexes::SDCard    = xSemaphoreCreateMutex();

    // queues
    q_userinput   = xQueueCreate(4, sizeof(UserInputMessage));
    q_mouseclicks = xQueueCreate(8, sizeof(MouseclickMessage));
    q_sfx_tts     = xQueueCreate(10, sizeof(AudioMessage));
    q_display     = xQueueCreate(3, sizeof(DisplayMessage));
    q_haptic      = xQueueCreate(2, sizeof(HapticMessage));
  
    if (!getSettingsConfig(&settings, true)) {
        createDefaultSettingsConfig(true);
    }
    printSettingsConfig(settings);

    g_accelEvent.x = 0;
    g_accelEvent.y = 0;
    g_accelEvent.z = 0;

    g_wifiReady = false;
    debuglnF("Init complete");
}
