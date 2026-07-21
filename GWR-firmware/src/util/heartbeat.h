#pragma once

#include <Arduino.h>
#include "config/app_conf.h"


/**
 * @brief Onboard LED heartbeat service
 * 
 * @details Blinks the development board's built-in LED with a 50% 
 *          duty cycle at a rate of HEARTBEAT_FREQUENCY hertz
 * 
 * @param pv Not used
 */
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