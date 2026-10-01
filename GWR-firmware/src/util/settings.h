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

/**
 * @brief Helper for getSettingsConfig
 * 
 * @param overWriteExisting 
 */
void populateDefaultSettingsConfig();

/**
 * @brief Helper for getSettingsConfig. Get the Settings Config As Json object
 * 
 * @param jdoc reference to store the output in
 * @return true
 * @return false 
 */
bool getSettingsConfigAsJson(JsonDocument &jdoc);

/**
 * @brief Populate the primary settings struct from the SD card
 *        If the file is missing, we generate the defaults.
 *        If a particular setting is missing from the file, we set it to a default
 *        and perform a writeback to the SD card
 *        range validation is still up to the individual task.
 * 
 * @param sc output struct
 * @param dsc default settings struct
 * @param allow_modification whether we may modify the contents of an existing settings file
 * @return true on success or successful regeneration
 * @return false on catastrophic failure, implying the program should halt
 */
bool getSettingsConfig(SettingsConfig &sc, const SettingsConfig& dsc, bool allow_modification = true);

void printSettingsConfig(const SettingsConfig &sc);