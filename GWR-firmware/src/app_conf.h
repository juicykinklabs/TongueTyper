#pragma once

#include <Arduino.h>

#ifdef BUILD_TYPE_DEBUG && BUILD_TYPE_DEBUG == 1
#define debugStart() Serial.begin(115200); delay(2000)
#define debug(...) Serial.print(__VA_ARGS__)
#define debugln(...) Serial.println(__VA_ARGS__)
#define debugf(...) Serial.printf(__VA_ARGS__)
#define debugF(...) Serial.print(F(__VA_ARGS__))
#define debuglnF(...) Serial.println(F(__VA_ARGS__))
#define debugfF(...) Serial.printf(F(__VA_ARGS__))

#else

#define debugStart()
#define debug(...)
#define debugln(...)
#define debugf(...)
#define debugF(...)
#define debuglnF(...)
#define debugfF(...)
#endif