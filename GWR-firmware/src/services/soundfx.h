#pragma once

#include <Arduino.h>
#include <Audio.h>

extern Audio audio;

/**
 * @brief Convert volume from one range to another
 * 
 * @details Audio library uses VolumeSteps, so we may want to convert
 *          a percentage to a VolumeStep. Caution: does not clamp range.
 * @param v volume input in range 0.0 to 1.0
 * @return uint8_t volume in range 0 to 21
 */
uint8_t volumeToVolume(double v);

/**
 * @brief Finish playing the current audio
 * 
 * @details Does not give/take any semaphores that may be required,
 *          intended as a helper function for task_soundfx
 *
 */
void audioLoopToCompletion();

/**
 * @brief I2S Sound service
 * 
 * @details Initializes Audio I2S. Receives struct AudioMessage from a queue 
 *          q_sfx_tts and may play a sound file from the SD card or over TTS 
 *          if WiFi is available
 * 
 * @param pv Not used
 */
void task_soundfx(void *pv);