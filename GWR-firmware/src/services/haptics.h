#pragma once
#include "structs/HapticMessage.h"

void task_hapticEngine(void *pv);

bool queueHapticPattern(const HapticMessage& hc);
void pattern_AccordingToSettings();

const extern HapticMessage pattern_Blip;
const extern HapticMessage pattern_TwoBlip;
const extern HapticMessage pattern_Jolt;
const extern HapticMessage pattern_Triangle;