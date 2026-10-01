#pragma once

/**
 * @brief The primary logic behind TongueTyper
 * 
 * @details This task is mode-aware, and sends data from other tasks (buttons, wifi)
 *          to specific queues in a specific format depending on the current mode.
 * 
 * @param pv Not used
 */
void task_moderouter(void *pv);