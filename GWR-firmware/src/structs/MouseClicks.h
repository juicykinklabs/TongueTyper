#pragma once

#include <Arduino.h>

enum MouseButtonState {
    RELEASED,
    PRESSED,
};

typedef struct MouseClicks {
    MouseButtonState state;
    uint8_t button;
} MouseClicks;