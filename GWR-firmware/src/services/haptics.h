#pragma once
#include "structs/HapticMessage.h"

/**
 * @brief Completely shutdown haptics
 * 
 */
void disableHVibe();

/**
 * @brief Haptics service
 * 
 * @details Receives a HapticMessage from q_haptic and plays the pattern
 *          on the haptic motor
 * 
 * @param pv Not used 
 */
void task_hapticEngine(void *pv);

/**
 * @brief Queue a haptic pattern for the haptics service
 * 
 * @param hc a HapticMessage containing a byte pattern to play
 * @return true if the pattern was queued successfully
 * @return false otherwise
 */
bool queueHapticPattern(const HapticMessage& hc);

/**
 * @brief Not yet implemented
 * 
 */
void pattern_AccordingToSettings();

const extern HapticMessage pattern_Blip;
const extern HapticMessage pattern_TwoBlip;
const extern HapticMessage pattern_Jolt;
const extern HapticMessage pattern_Triangle;