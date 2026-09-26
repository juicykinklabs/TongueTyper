#pragma once

#include <Arduino.h>

extern bool flag;

/**
 * @brief ISR triggered by button presses
 * 
 * @details Configured in task_buttons, sets a flag
 *          and clears interrupt on MCP GPIO expander
 * 
 */
void IRAM_ATTR isr_gpio_int();

/**
 * @brief Buttons (user input) service
 * 
 * @details Handle the GPIO expander via interrupts and SPI reads. Debounce all inputs,
 *          and send a stream of structs UserInputMessage to q_userinput for use in the 
 *          main app logic.
 * 
 * @param pv Not used
 */
void task_buttons(void *pv);