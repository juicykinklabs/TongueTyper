#pragma once

#include <Arduino.h>
#include "config/app_conf.h"

inline void do_heartbeat(void *pv)
{
    pinMode(LED_BUILTIN, OUTPUT);
    static bool led_state = LOW; 
    while (1)
    {
        digitalWrite(LED_BUILTIN, led_state ? HIGH : LOW);
        led_state = !led_state;
        vTaskDelay((1000 / HEARTBEAT_FREQUENCY) / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}