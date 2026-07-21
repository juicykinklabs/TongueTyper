#pragma once

/**
 * @brief USB mouse emulation service
 * 
 * @details Receives mouseclicks from q_mouseclicks and reads accelerometer data
 *          from global struct g_accelEvent. Formats and forwards this info to
 *          the Mouse object
 * 
 * @param pv Not used 
 */
void task_mouse(void *pv);

