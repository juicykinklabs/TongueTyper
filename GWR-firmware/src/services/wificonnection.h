#pragma once

/**
 * @brief Create and maintain a WiFi connection
 * 
 * @details Reads WiFi details from the SD card config file
 *          and synchronizes RTC
 * 
 * @param pv Not used 
 */
void task_wificonnection(void *pv);