#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "structs/SettingsConfig.h"

/**
 * @brief Write a SettingsConfig struct to the SD card
 * 
 * @param sc 
 * @return true
 * @return false
 */
bool writeSettingsConfig(const SettingsConfig &sc);


bool deleteSettingsConfig();

void createDefaultSettingsConfig(bool overWriteExisting = false);

/**
 * @brief Helper for getSettingsConfig. Get the Settings Config As Json object,
 *        may recurse once for regeneration logic
 * 
 * @param jdoc reference to store the output in
 * @param allow_regeneration whether we may delete and overwrite the existing file
 * @return true
 * @return false 
 */
bool getSettingsConfigAsJson(JsonDocument &jdoc, bool allow_regeneration = false);

bool getSettingsConfig(SettingsConfig *sc, bool allow_regeneration = false);

void printSettingsConfig(const SettingsConfig &sc);