#include <Arduino.h>
#include <SPI.h>
#include "esp32s3/rom/rtc.h"

#include <SD.h>

#include "config/app_conf.h"
#include "config/keybinds_conf.h"

#include "services/accel.h"
#include "services/mouse_out.h"
#include "services/soundfx.h"
#include "services/wificonnection.h"
#include "services/mode_keyboard.h"
#include "services/haptics.h"
#include "services/display.h"

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

void setup() {

    // *** INITIALIZATION ***

    // put any very early init stuff here. serial printing is NOT ready.
    pinMode(Pins::HVIBE, OUTPUT);
    digitalWrite(Pins::HVIBE, LOW);

    pinMode(Pins::TFT::CS, OUTPUT);
    digitalWrite(Pins::TFT::CS, HIGH);
    //pinMode(Pins::TFT::RST, OUTPUT);
    //digitalWrite(Pins::TFT::RST, LOW);

    pinMode(Pins::EXP::CS, OUTPUT);
    digitalWrite(Pins::EXP::CS, HIGH);
    pinMode(Pins::EXP::RST, OUTPUT);
    digitalWrite(Pins::EXP::RST, LOW);
    
    SPI.begin(Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);

    // initialize globals. serial printing will be ready afterwards.
    initTaskGlobals();
    // get reset reason and core temp
    debugf("rst: %d %d\n", rtc_get_reset_reason(0), rtc_get_reset_reason(1));
    debugf("core: %.1fC\n", temperatureRead());
//#ifdef VERSION_DEV
//    performNTSP();
//    writeSettingsConfig(settings);
//#endif 
    
    // *** START TASKS ***

    xTaskCreate(do_heartbeat, "LED Heartbeat", 4096, NULL, 1, NULL);
#ifdef VERSION_DEV
    xTaskCreate(task_debug_mem, "Memory Debugger", 4096, NULL, 2, NULL);
#endif

    // *** 

    DEVICE_MODE = KEYBOARD;

    // *** TEST CODE BELOW ***
    //xTaskCreate(task_accel, "Accel", 4096, NULL, 3, NULL);
    //debugln("starting keyboard");
    //xTaskCreate(task_mode_keyboard, "Mode Task", 8192, NULL, 5, NULL);
    //debugln("starting mouse");
    //xTaskCreate(task_mouse, "Mouse", 4096, NULL, 5, NULL);
    
    xTaskCreate(task_hapticEngine, "Haptics", 4096, NULL, 8, NULL);
    //xTaskCreate(task_displayImageOrText, "Display", 4096, NULL, 4, NULL);
    //xTaskCreate(task_soundfx, "Audio", 8192, NULL, 7, NULL); //requires wireless, but will ignore if wireless not ready
    //vTaskDelay(1000);
    xTaskCreate(task_wificonnection, "Wireless", 4096, NULL, 9, NULL); // task started, but wifi may not be ready. put tasks that require wifi after the gate.
    while (settings.wifi.enabled && !g_wifiReady) {
        vTaskDelay(100); // wait before advancing further into setup
    }
    debugln("starting server");
    xTaskCreate(task_webserver, "WebAPI", 8192, NULL, 10, NULL);
    
//
    //debuglnF("setup() complete.");

}

void loop() { 
    const char fullString[] = "i can haz hamburger?";
    
    //DisplayMessage d;
    //d.instruction = DisplayInstruction::DRAW_IMAGE;
    //strcpy(d.data, "/usermedia/images/11.bmp");
    //xQueueSend(q_display, (void*) &d, 0);
    //strcpy(d.data, "/usermedia/images/12.bmp");
    //xQueueSend(q_display, (void*) &d, 0);
    //strcpy(d.data, "/usermedia/images/13.bmp");
    //xQueueSend(q_display, (void*) &d, 0);
    //strcpy(d.data, "/usermedia/images/14.bmp");
    //xQueueSend(q_display, (void*) &d, 0);
    //strcpy(d.data, "/usermedia/images/15.bmp");
    //xQueueSend(q_display, (void*) &d, 0);
    vTaskDelay(5000);
    
    //d.instruction = DisplayInstruction::SHOW_STRING;
    //for (int L = 0; L < strlen(fullString) + 1; L++){
    //    strncpy(d.data, fullString, L);
    //    d.data[L] = '\0'; // data must have null terminator, expected by the consumer
    //    debugf("SENDING: %s<<<\n", d.data);
    //    xQueueSend(q_display, (void*) &d, 0);
    //    vTaskDelay(500);
    //}
    //vTaskDelay(5000);

    //AudioMessage m;
    //m.instruction = AudioEventEnum::SFX;
    //settings.sfx.volume = 0.9;
    //strcpy(m.data, "/usermedia/sounds/11.mp3");
    //xQueueSend(q_sfx_tts, (void*) &m, 0);
    //strcpy(m.data, "/usermedia/sounds/22.mp3");
    //xQueueSend(q_sfx_tts, (void*) &m, 0);

    
    //vTaskDelete(NULL); 
    }