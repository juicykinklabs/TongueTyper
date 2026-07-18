#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "config/hardware_conf.h"
#include "structs/SettingsConfig.h"

bool writeSettingsConfigFromJson(const JsonDocument &jdoc);
bool writeSettingsConfig(const SettingsConfig &sc);
void deleteSettingsConfig();
void createDefaultSettingsConfig(bool overWriteExisting = false);
bool getSettingsConfigAsJson(JsonDocument &jdoc, bool regenerate = false);
bool getSettingsConfig(SettingsConfig *sc, bool regenerate = false);
void printSettingsConfig(const SettingsConfig &sc);