#include <Arduino.h>

#include "config/app_conf.h"

#include "services/accel.h"
#include "services/mouse_out.h"
#include "services/soundfx.h"
#include "services/heartbeat.h"
#include "services/wificonnection.h"

#include "structs/AudioEvent.h"
#include "util/settings.h"
#include "taskglobals.h"

void populateSettings() {}

void setup() {
    debugStart();
    
    // initialize globals
    
    Mutexes::SPI         = xSemaphoreCreateMutex();
    Mutexes::SerialPrint = xSemaphoreCreateMutex();
    Mutexes::AccelData   = xSemaphoreCreateMutex();
    Mutexes::SDCard      = xSemaphoreCreateMutex();
    
    q_sfx_tts = xQueueCreate(16, sizeof(AudioEvent_t));
    
    xTaskCreate(do_heartbeat, "LED Heartbeat", 1024, NULL, 1, &Tasks::heartbeat);
    
    if (!getSettingsConfig(&settings, true)) {
        createDefaultSettingsConfig(true);
    }
    printSettingsConfig(settings);

    // create any tasks

    // debugln("creating adxl task");
    // xTaskCreate(task_accel, "ADXL34X Function", 8192, NULL, 2, NULL);
    // debugln("creating mouse task");
    // xTaskCreate(task_mouse, "MouseFunction", 16384, NULL, 5, NULL);
    
    xTaskCreate(task_wificonnection, "Wireless", 4096, NULL, 1, &Tasks::wificonnection);
    while (settings.wifi.enabled && !g_wifiReady) {
        vTaskDelay(10); // wait before advancing further into setup
    }

    debugln("creating audio tts task");
    xTaskCreate(task_soundfx, "Audio Task", 4096, NULL, 6, &Tasks::soundfx);


    // example of interfacing with a task via queue
    AudioEvent_t myAudioEvent;
    settings.sfx.volume = 0.85;                                    // we can update settings on the fly without necessarily writing them back to SD card
    
    myAudioEvent.data   = String("this is only a test sentence.");
    myAudioEvent.type   = AudioEventEnum::TTS;
    xQueueSend(q_sfx_tts, (void *) &myAudioEvent, (TickType_t) 0);
    // to avoid spamming the api, we should send full sentences - when key_period is pressed, basically
    
    myAudioEvent.data = String("usermedia/sounds/22.mp3");
    myAudioEvent.type = AudioEventEnum::SFX;
    xQueueSend(q_sfx_tts, (void *) &myAudioEvent, (TickType_t) 0);
    
    // we should delay after placing our event in queue. if it goes out of scope before the consumer gets to it, who knows what could happen!?
    
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}

void loop() {
    // ideally loop will self delete. for now it's monitoring memory
    debuglnF("loop debug:");
    debugln(uxTaskGetNumberOfTasks());
    debugln(xPortGetFreeHeapSize()); // get programs heap remaining
    debugf("free memory: %dB\n", uxTaskGetStackHighWaterMark(NULL)); // task specific
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}