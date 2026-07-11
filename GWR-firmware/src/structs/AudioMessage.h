#pragma once

#include <Arduino.h>

#define AUDIOMESSAGE_DATA_SZ (64)

enum AudioEventEnum {
    SFX,
    TTS,
};

typedef struct AudioMessage {
    AudioEventEnum type; // sfx or tts
    char data[AUDIOMESSAGE_DATA_SZ]; // a filename or tts phrase
} AudioMessage;