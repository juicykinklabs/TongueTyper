#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#define USING_ADAFRUIT_SD_FORK_FOR_FILES // todo make configurable 

#ifdef USING_ADAFRUIT_SD_FORK_FOR_FILES

#ifndef DISABLE_FS_H_WARNING
#define DISABLE_FS_H_WARNING
#endif

#include "SdFat_Adafruit_Fork.h"

extern SdFat32 SD_as_FAT32;
extern SdSpiConfig sdConfig;

#endif

bool sdOpBegin();
void sdOpEnd();
bool fileExists(const char* fspath);
bool fileRemove(const char* fspath);
bool fileToJdoc(const char* fspath, JsonDocument &jdoc);
bool jdocToFile(const char* fspath, const JsonDocument &jdoc);
