#pragma once

#include "ADXL343.h"

extern ADXL343 accel;

/**
 * @brief Accelerometer service
 * 
 * @details Initializes the accelerometer once, then repeatedly polls
 *          for new data, updating a global structure, g_accelEvent
 * 
 * @param pv 
 */
void task_accel(void *pv);
