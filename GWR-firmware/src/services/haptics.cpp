#include "haptics.h"
#include <Arduino.h>

#include "config/hardware_conf.h"
#include "structs/HapticCommand.h"
#include "util/settings.h"
#include "util/mapfloat.h"
#include "taskglobals.h"

void task_hapticEngine(void *pv) {
    // we should create the task with a higher priority
    // to faithfully recreate incoming the haptic pattern
    pinMode(Pins::HVIBE, OUTPUT);
    digitalWrite(Pins::HVIBE, LOW);

    HapticCommand hc;
    while (1) {
        if (xQueueReceive(q_haptic, (void *) &hc, portMAX_DELAY)) {
            if (settings.haptic.enabled) {
                for (uint8_t i = 0; i < HAPTIC_COMMAND_LEN; i++) {

                    if (hc.intensities[i] == 0x00) {
                        digitalWrite(Pins::HVIBE, LOW);
                    } else {
                        double actual_strength_multiplier = constrain(settings.haptic.strength, 0.0, 1.0);
                        // map 0.0-1.0 onto 1.5/4.2 (0.35) to 3.7/4.2 (0.88)
                        // todo check isnan
                        actual_strength_multiplier = mapfloat(actual_strength_multiplier, 0.0, 1.0, 0.35, 0.88);
                        analogWrite(Pins::HVIBE, hc.intensities[i] * actual_strength_multiplier);
                    }
                    vTaskDelay(max(hc.durations[i] / portTICK_PERIOD_MS, (TickType_t) 1));
                }
            }
            digitalWrite(Pins::HVIBE, LOW);
            vTaskDelay(1);
        }
    }
    vTaskDelete(NULL);
}

bool queueHapticPattern(const HapticCommand &hc) { return xQueueSend(q_haptic, (void *) &hc, (TickType_t) 0) == pdPASS; }

void pattern_AccordingToSettings() {
    // (something like this)
    switch (settings.haptic.pattern) {
        case 1:
            queueHapticPattern(pattern_Blip);
            break;
        case 2:
            queueHapticPattern(pattern_TwoBlip);
            break;
        case 3:
            queueHapticPattern(pattern_Jolt);
            break;
        case 4:
            queueHapticPattern(pattern_Triangle);
            break;

        default:
            // (0)
            break;
    }
}

// note for new patterns: scale so that the peak intensity is 0xFF.
const HapticCommand pattern_Blip = {
    {0x80, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00}, // intensity
    {0x1A, 0x50, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00}  // duration
};

const HapticCommand pattern_TwoBlip = {
    {0x80, 0xFF, 0x80, 0x00, 0x73, 0xE5, 0x73, 0x00}, // intensity
    {0x1A, 0x50, 0x1A, 0x96, 0x1A, 0x50, 0x1A, 0x00}  // duration
};

const HapticCommand pattern_Jolt = {
    {0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // intensity
    {0x60, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // duration
};

const HapticCommand pattern_Triangle = {
    {0x33, 0x65, 0xA0, 0xCC, 0xFF, 0xCC, 0xA0, 0x65}, // intensity
    {0x50, 0x50, 0x50, 0x50, 0x55, 0x50, 0x50, 0x50}  // duration
};
