#pragma once

/**
 * @brief Serial debugging helper service
 *
 * @details Reads a character from serial monitor and performs a 
 *          function based on which character is sent
 *          'm' -> show memory status
 *
 * @param pv Not used
 */
void task_debug_mem(void *pv);