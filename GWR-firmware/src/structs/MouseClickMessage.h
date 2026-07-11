#pragma once

#include <Arduino.h>

enum MouseButtonState {
    RELEASED,
    PRESSED,
};

typedef struct MouseclickMessage {
    MouseButtonState state;
    uint8_t button;
} MouseclickMessage;