#pragma once

#include <Arduino.h>

// heartbeat configuration
#define HEARTBEAT_FREQUENCY (3)

// debug/logging setup

#define BAUDRATE_ESP32 (115200)
#define SERIAL_CONNECT_DELAY (2000) // ms
#define COMPOSITE_ENUMERATION_DELAY (3000) // ms, Windows
// todo: could we use ESP_LOGD format for consistent formatting across serial as well as log files?
// todo: use a message queue and have one task for serial printing
#define dbg_timestamp() { Serial.printf("[%3u.%03u%03u] [%s::%d]  ", millis()/1000UL, millis()%1000UL, micros() % 1000UL, __FILE__, __LINE__);}
#define dbg_timestampF() { Serial.printf(F("[%3u.%03u%03u] [%s::%d]  "), millis()/1000UL, millis()%1000UL, micros() % 1000UL, __FILE__, __LINE__);}

inline void debugPoorMansBreakPoint() {Serial.println("..."); while(!Serial.available()) {yield();} while(Serial.available()) {Serial.read();}}

#ifdef VERSION_DEV
#define debugStart() Serial.begin(BAUDRATE_ESP32);

#define debug(...) {dbg_timestamp(); Serial.print(__VA_ARGS__);}
#define debugln(...) {dbg_timestamp(); Serial.println(__VA_ARGS__);}
#define debugf(...) {dbg_timestamp(); Serial.printf(__VA_ARGS__);}
#define debugF(...) {dbg_timestampF(); Serial.print(F(__VA_ARGS__));}
#define debuglnF(...) {dbg_timestampF(); Serial.println(F(__VA_ARGS__));}

#else

#define debugStart()
#define debug(...)
#define debugln(...)
#define debugf(...)
#define debugF(...)
#define debuglnF(...)
#define debugfF(...)
#endif