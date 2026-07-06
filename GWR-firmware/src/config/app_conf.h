#pragma once

#include <Arduino.h>

// heartbeat configuration
#define HEARTBEAT_FREQUENCY (3)

// debug/logging setup

#define BAUDRATE_ESP32 (115200)
#define SERIAL_CONNECT_DELAY (2000) // ms

// todo: could we use ESP_LOGD format for consistent formatting across serial as well as log files?
// todo: use a message queue and have one task for serial printing
inline void timestamp() { Serial.printf("[%8u.%03u] ", millis(), micros() % 1000UL);}
inline void timestampF() { Serial.printf(F("[%8u.%03u] "), millis(), micros() % 1000UL);}

#ifdef VERSION_DEV
#define debugStart() Serial.begin(BAUDRATE_ESP32); delay(SERIAL_CONNECT_DELAY)
#define debug(...) timestamp(); Serial.print(__VA_ARGS__); Serial.flush()
#define debugln(...) timestamp(); Serial.println(__VA_ARGS__); Serial.flush()
#define debugf(...) timestamp(); Serial.printf(__VA_ARGS__); Serial.flush()
#define debugF(...) timestampF(); Serial.print(F(__VA_ARGS__)); Serial.flush()
#define debuglnF(...) timestampF(); Serial.println(F(__VA_ARGS__)); Serial.flush()

#else

#define debugStart()
#define debug(...)
#define debugln(...)
#define debugf(...)
#define debugF(...)
#define debuglnF(...)
#define debugfF(...)
#endif