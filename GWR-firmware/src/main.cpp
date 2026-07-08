#include <Arduino.h>

#include "config/app_conf.h"
#include "config/keybinds_conf.h"

#include "services/accel.h"
#include "services/mouse_out.h"
#include "services/soundfx.h"
#include "services/heartbeat.h"
#include "services/wificonnection.h"
#include "services/mode_keyboard.h"
#include "services/haptics.h"

#include "structs/AudioEvent.h"
#include "structs/UserInputEvent.h"
#include "util/settings.h"
#include "taskglobals.h"

// WORK IN PROGRESS

void setup() {
    // initialize debug, globals

    initTaskGlobals();

    xTaskCreate(do_heartbeat, "LED Heartbeat", 1024, NULL, 1, &Tasks::heartbeat);
    // xTaskCreate(task_accel, "ADXL34X", 8192, NULL, 2, &Tasks::accel);
    // xTaskCreate(task_mouse, "Mouse", 16384, NULL, 5, &Tasks::mouse_out);
    xTaskCreate(task_mode_keyboard, "MODE TASK", 8192, NULL, 5, &Tasks::mode);

    DEVICE_MODE = JOYSTICK;
    UserInputEvent_t myInputEvent;

       
    // create any tasks

    // xTaskCreate(task_wificonnection, "Wireless", 4096, NULL, 1, &Tasks::wificonnection);
    // while (settings.wifi.enabled && !g_wifiReady) {
    //     vTaskDelay(10); // wait before advancing further into setup
    // }
    //
    debugln("creating audio receiver task");
    xTaskCreate(task_soundfx, "Audio", 4096, NULL, 6, &Tasks::soundfx);
    xTaskCreate(task_hapticEngine, "Haptics", 4096, NULL, 12, &Tasks::haptics);
    //
    //// example of interfacing with a task via queue
    AudioEvent_t myAudioEvent;
    settings.sfx.volume = 0.85; // we can update settings on the fly without necessarily writing them back to SD card
    //
    // myAudioEvent.data = String("this is only a test sentence.");
    // myAudioEvent.type = AudioEventEnum::TTS;
    // xQueueSend(q_sfx_tts, (void *) &myAudioEvent, (TickType_t) 0);
    //// to avoid spamming the api, we should send full sentences - when key_period is pressed, basically
    //
    vTaskDelay(5000);
    for(;;) {

        //myAudioEvent.data = String("sys/media/interface-click.wav");
        //myAudioEvent.type = AudioEventEnum::SFX;
        //xQueueSend(q_sfx_tts, (void *) &myAudioEvent, (TickType_t) 0);
        //// wihtout delay, we corrupt .data, since the underlying string points to the same char buf..
        //vTaskDelay(2000);
        //myAudioEvent.data = String("sys/media/negative-tone.wav");
        //myAudioEvent.type = AudioEventEnum::SFX;
        //xQueueSend(q_sfx_tts, (void *) &myAudioEvent, (TickType_t) 0);
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
}

void loop() {
    // ideally loop will self delete. for now it's monitoring memory
    debuglnF("MEMORY DEBUG:");
    debugf("Tasks: %d\n", uxTaskGetNumberOfTasks());
    debugf("Heap free: %d\n", xPortGetFreeHeapSize()); // get programs heap remaining

    debugf(F("Task %s: %d Bytes free\n"), "loop()", uxTaskGetStackHighWaterMark(NULL));
    debugf(F("Task %s: %d Bytes free\n"), "soundfx", uxTaskGetStackHighWaterMark(Tasks::soundfx));
    debugf(F("Task %s: %d Bytes free\n"), "rgb", uxTaskGetStackHighWaterMark(Tasks::rgbled));
    debugf(F("Task %s: %d Bytes free\n"), "mouse", uxTaskGetStackHighWaterMark(Tasks::mouse_out));
    debugf(F("Task %s: %d Bytes free\n"), "mode", uxTaskGetStackHighWaterMark(Tasks::mode));
    debugf(F("Task %s: %d Bytes free\n"), "heart", uxTaskGetStackHighWaterMark(Tasks::heartbeat));
    debugf(F("Task %s: %d Bytes free\n"), "disp", uxTaskGetStackHighWaterMark(Tasks::display));
    debugf(F("Task %s: %d Bytes free\n"), "button", uxTaskGetStackHighWaterMark(Tasks::buttons));
    debugf(F("Task %s: %d Bytes free\n"), "accel", uxTaskGetStackHighWaterMark(Tasks::accel));
    debugf(F("Task %s: %d Bytes free\n"), "wifi", uxTaskGetStackHighWaterMark(Tasks::wificonnection));
    debugf(F("Task %s: %d Bytes free\n"), "haptic", uxTaskGetStackHighWaterMark(Tasks::haptics));

    vTaskDelay(5000 / portTICK_PERIOD_MS);
}