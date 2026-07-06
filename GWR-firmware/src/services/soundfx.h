#pragma once
#include <Arduino.h>

uint8_t volumeToVolume(double v);
void audioLoopToCompletion();
void task_soundfx(void *pv);