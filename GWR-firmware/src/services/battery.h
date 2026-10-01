#pragma once

/**
 * @brief Update battery level (g_battery_voltage) by reading the ADC periodically
 * 
 * @param pv 
 */
void task_batt(void *pv);