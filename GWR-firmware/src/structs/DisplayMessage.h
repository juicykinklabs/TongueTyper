#pragma once

#include <Arduino.h>

#define DISPLAYMESSAGE_DATA_SZ (64)

enum DisplayInstruction {
    JUST_CLEAR,
    DRAW_IMAGE,
    SHOW_STRING
};

typedef struct DisplayMessage {
    DisplayInstruction instruction;
    char data[DISPLAYMESSAGE_DATA_SZ]; // may be a string to show or a file path to an image
} DisplayMessage;