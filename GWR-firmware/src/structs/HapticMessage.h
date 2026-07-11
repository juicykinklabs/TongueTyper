#pragma once

#include <Arduino.h>

#define HAPTIC_COMMAND_LEN (8)

typedef struct HapticMessage {
    uint8_t intensities[HAPTIC_COMMAND_LEN]; // array of pwm values
    uint8_t durations[HAPTIC_COMMAND_LEN]; // array of durations, ms
} HapticMessage;
