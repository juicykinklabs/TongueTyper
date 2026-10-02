#pragma once
#include <Arduino.h>

#include "config/hardware_conf.h"

typedef struct StatisticsConfig {
    // system
    uint32_t bootcount; // increments on every boot
    uint32_t t_overall;
    uint32_t t_wificonnected;
    uint32_t t_mode_standard;
    uint32_t t_mode_gaming;
    uint32_t t_mode_a_v;
    
    // buttonPresses
    // cast in from a dictionary
    uint32_t buttonPresses[NUM_EXPUI_BUTTONS]; 
    
    // User 
    uint32_t mm_travel; // todo: need a list of distances between every pair of buttons
    uint32_t played_haptics;
    uint32_t played_images;
    uint32_t played_sounds;
    uint32_t played_letters;
    uint32_t played_words; // increment on space after non-space character

} StatisticsConfig;