#pragma once
#include "structs/HapticCommand.h"

void task_hapticEngine(void *pv);

bool queueHapticPattern(const HapticCommand& hc);
void pattern_AccordingToSettings();

const extern HapticCommand pattern_Blip;
const extern HapticCommand pattern_TwoBlip;
const extern HapticCommand pattern_Jolt;
const extern HapticCommand pattern_Triangle;