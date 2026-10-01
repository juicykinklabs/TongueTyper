#include <Arduino.h>
#include <SPI.h>
#include "esp32s3/rom/rtc.h"

#include "config/app_conf.h"
#include "config/keybinds_conf.h"

#include "services/accel.h"
#include "services/mouse_out.h"
#include "services/soundfx.h"
#include "services/wificonnection.h"
#include "services/haptics.h"
#include "services/display.h"
#include "services/buttons.h"

#include "services/modeRouter.h"

#include "webserver/webapi.h"

#include "structs/AudioMessage.h"
#include "structs/UserInputMessage.h"
#include "structs/DisplayMessage.h"

#include "util/settings.h"
#include "util/debug_memory.h"
#include "util/heartbeat.h"

#include "taskglobals.h"

#include "util/calibration.h"

// WORK IN PROGRESS

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    debugF("Stack overflow: ");
    debugln(pcTaskName);
}

void earlyPinModeChipSelect() {
    pinMode(Pins::SD::CS, OUTPUT);
    digitalWrite(Pins::SD::CS, HIGH);
    
    pinMode(Pins::HVIBE, OUTPUT);
    digitalWrite(Pins::HVIBE, LOW);

    pinMode(Pins::TFT::CS, OUTPUT);
    digitalWrite(Pins::TFT::CS, HIGH);
    // pinMode(Pins::TFT::RST, OUTPUT);
    // digitalWrite(Pins::TFT::RST, LOW);

    pinMode(Pins::ACCEL::CS, OUTPUT);
    digitalWrite(Pins::ACCEL::CS, HIGH);

#if HAS_GPIO_EXPANDER
    pinMode(Pins::EXP::CS, OUTPUT);
    digitalWrite(Pins::EXP::CS, HIGH);
    pinMode(Pins::EXP::RST, OUTPUT);
    digitalWrite(Pins::EXP::RST, LOW);
#endif

}

void initAppSettings() {
    // call after initTaskGlobals()
    // to read settings from SD card
    // required for most tasks
    
    if (not getSettingsConfig(settings, defaultsettings, true)) {
        debuglnF("getSettingsConfig failed - cannot continue. Is there an SD card?");
        while (1) {
            vTaskDelay(1000);
        }
    }
    printSettingsConfig(settings);
}

void setup() {

    // *** INITIALIZATION ***

    // put any very early init stuff here. serial printing is NOT ready.
    
    earlyPinModeChipSelect();

    SPI.begin(Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);

    // initialize globals. serial printing will be ready afterwards.
    initTaskGlobals();

    // read settings from SD card
    initAppSettings();

    // #ifdef VERSION_DEV
    //     performNTSP();
    // #endif
    
    // get reset reason and core temp
    debugf("rst: %d %d\n", rtc_get_reset_reason(0), rtc_get_reset_reason(1));
    debugf("core: %.1fC\n", temperatureRead());
    
    // *** START TASKS ***

    xTaskCreate(do_heartbeat, "LED Heartbeat", 4096, NULL, 1, NULL);
#ifdef VERSION_DEV
    xTaskCreate(task_debug_mem, "Memory Debugger", 4096, NULL, 2, NULL);
#endif

    // ***

    DEVICE_MODE = KEYBOARD;

    // *** TEST CODE BELOW ***
    //debugln("starting accel");
    //xTaskCreate(task_accel, "Accel", 4096, NULL, 3, NULL);
    
    //debugln("starting mouse");
    //xTaskCreate(task_mouse, "Mouse", 4096, NULL, 5, NULL);
    
    debugln("starting haptics");
    xTaskCreate(task_hapticEngine, "Haptics", 4096, NULL, 7, NULL);    
    debugln("starting display");
    xTaskCreate(task_displayImageOrText, "Display", 4096, NULL, 4, NULL);
    
    // xTaskCreate(task_soundfx, "Audio", 8192, NULL, 8, NULL); //requires wireless, but will ignore if wireless not ready
    // vTaskDelay(1000);
    //xTaskCreate(task_wificonnection, "Wireless", 4096, NULL, 9, NULL); // task started, but wifi may not be ready. put tasks that require wifi after the gate.
    //while (settings.wifi.enabled && !g_wifiReady) {
        //    vTaskDelay(100); // wait before advancing further into setup
        //}
        //debugln("starting server");
        //xTaskCreate(task_webserver, "WebAPI", 8192, NULL, 10, NULL);
        
        
    debugln("starting buttons");
    xTaskCreate(task_buttons, "Buttons", 8192, NULL, 8, NULL);
    
    debugln("starting core router");
    xTaskCreate(task_moderouter, "Mode Task", 8192, NULL, 5, NULL);
    
    debuglnF("setup() complete.");
}

void loop() { vTaskDelete(NULL); }