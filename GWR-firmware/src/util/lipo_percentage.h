#pragma once

#include "util/mapfloat.h"

/**
 * @brief LiPo battery voltage to percentage points
 *        (Don't actually use this method, the approximation is poor)
 * 
 * @param v battery voltage
 * @param vmin battery voltage at full charge
 * @param vmax battery voltage when dead
 * @return float in range 0-100
 */
float lipoPercentLinear(float v, float vmin = 3.30, float vmax = 4.20) {
    return constrain(mapfloat(v, vmin, vmax, 0.0, 100.0), 0.0, 100.0);
}

/**
 * @brief Based on https://electronics.stackexchange.com/q/55166
 *        For standard 3.7V nominal (4.2V max) batteries
 * 
 * @param v battery voltage
 * @return float in range 0-100
 */
float lipoPercentFancy(float v) {
    return constrain(123.0 - (123.0 / pow((1.0 + pow(v / 3.70, 80.0)), 0.165)), 0.0, 100.0);
}