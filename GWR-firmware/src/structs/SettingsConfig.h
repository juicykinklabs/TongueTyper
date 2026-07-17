#pragma once
#include <Arduino.h>

typedef struct sc_adxl {
    int16_t ofx;
    int16_t ofy;
    int16_t ofz;
} sc_adxl;

typedef struct sc_wifi {
    bool enabled;
    String ssid;
    String pswd;
} sc_wifi;

typedef struct sc_disp {
    double brightness;
    bool invertColors;
    String font;
} sc_disp;

typedef struct sc_sfx {
    double volume;
    bool announce;
    String lang;
} sc_sfx;

typedef struct sc_haptic {
    bool enabled;
    double strength;
    uint32_t pattern;
} sc_haptic;

typedef struct sc_hid {
    uint32_t mouseSense;
} sc_hid;

typedef struct SettingsConfig {
    sc_adxl adxl;
    sc_wifi wifi;
    sc_disp disp;
    sc_sfx sfx;
    sc_haptic haptic;
    sc_hid hid;
} SettingsConfig;