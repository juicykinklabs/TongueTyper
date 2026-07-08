#pragma once

#include <Arduino.h>
#include "config/hardware_conf.h"
#include "structs/SettingsConfig.h"

bool writeSettingsConfig(const SettingsConfig& sc);
void deleteSettingsConfig();
void createDefaultSettingsConfig(bool overWriteExisting = false);
bool getSettingsConfig(SettingsConfig * sc, bool generateDefaultsIfMissing = true);
void printSettingsConfig(const SettingsConfig &sc);