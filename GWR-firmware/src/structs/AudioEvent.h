#pragma once

#include <Arduino.h>

enum AudioEventEnum {
    SFX,
    TTS,
};

typedef struct AudioEvent_t {
    AudioEventEnum type; // sfx or tts
    String data; // a filename or tts phrase
} AudioEvent_t;