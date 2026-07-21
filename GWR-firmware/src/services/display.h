#pragma once

/**
 * @brief Change the PWM value sent to the display
 * 
 * @param b Brightness value, range 0.0 - 1.0 
 */
void setTFTBrightness(double b);

/**
 * @brief Display service
 * 
 * @details Initializes the display. Upon receiving a DisplayMessage instruction from
 *          a queue q_display; may clear the display, draw an image from the SD card, 
 *          or display a string with special formatting
 * 
 * @param pv Not used 
 */
void task_displayImageOrText(void *pv);