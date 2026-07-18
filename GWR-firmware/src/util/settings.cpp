#include "settings.h"

#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

#include "config/app_conf.h"
#include "config/constants.h"

#include "taskglobals.h"

bool writeSettingsConfigFromJson(const JsonDocument &jdoc) {
    SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
    // todo check if SD.begin was ok
    SD.remove(FSPATH::Settings);

    File file = SD.open(FSPATH::Settings, FILE_WRITE);
    // todo check if file is ok

    if (serializeJsonPretty(jdoc, file) == 0) {
        debuglnF("Failed to write to file");
        file.close();
        SD.end();
        return false;
    }

    // Close the file
    file.close();
    SD.end();
    return true;
}

bool writeSettingsConfig(const SettingsConfig &sc) {
    JsonDocument doc;

    doc["ADXL"]["OFX"]             = sc.adxl.ofx;
    doc["ADXL"]["OFY"]             = sc.adxl.ofy;
    doc["ADXL"]["OFZ"]             = sc.adxl.ofz;
    doc["WiFi"]["Enabled"]         = sc.wifi.enabled;
    doc["WiFi"]["SSID"]            = sc.wifi.ssid;
    doc["WiFi"]["PSWD"]            = sc.wifi.pswd;
    doc["Display"]["Brightness"]   = sc.disp.brightness;
    doc["Display"]["Font"]         = sc.disp.font;
    doc["Display"]["InvertColors"] = sc.disp.invertColors;
    doc["SoundFX"]["Volume"]       = sc.sfx.volume;
    doc["SoundFX"]["Announce"]     = sc.sfx.announce;
    doc["SoundFX"]["Lang"]         = sc.sfx.lang;
    doc["Haptics"]["Enabled"]      = sc.haptic.enabled;
    doc["Haptics"]["Strength"]     = sc.haptic.strength;
    doc["Haptics"]["Pattern"]      = sc.haptic.pattern;
    doc["HID"]["MouseSensitivity"] = sc.hid.mouseSense;

    return writeSettingsConfigFromJson(doc);
}

void deleteSettingsConfig() {
    SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
    // todo check if begin ok, also take semaphore
    SD.remove(FSPATH::Settings);
    SD.end();
}

void createDefaultSettingsConfig(bool overWriteExisting) {

    SettingsConfig defaults;
    // we can use = with strings here because they are from the String class.
    // todo: configure these in a separate header file
    defaults.adxl.ofx          = 0;
    defaults.adxl.ofy          = 0;
    defaults.adxl.ofz          = 0;
    defaults.wifi.enabled      = true;
    defaults.wifi.ssid         = "myAccessPoint";
    defaults.wifi.pswd         = "myPassword";
    defaults.disp.brightness   = 0.5;
    defaults.disp.font         = "default";
    defaults.disp.invertColors = false;
    defaults.sfx.volume        = 0.75;
    defaults.sfx.announce      = false;
    defaults.sfx.lang          = "en";
    defaults.haptic.enabled    = true;
    defaults.haptic.strength   = 0.8;
    defaults.haptic.pattern    = 2;
    defaults.hid.mouseSense    = 1200; // pixels per second maximum

    SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
    File file = SD.open(FSPATH::Settings, FILE_READ);

    if (!file || overWriteExisting) {
        file.close();
        SD.end();
        if (!overWriteExisting) {
            debugln("No settings-config, creating one from scratch");
        } else {
            debugln("Manually overwriting settings config");
        }
        writeSettingsConfig(defaults);
    } else {
        file.close();
        SD.end();
    }
}

bool getSettingsConfigAsJson(JsonDocument &jdoc, bool regenerate) {
    if (regenerate) {
        bool success = getSettingsConfigAsJson(jdoc, false);
        if (!success) {
            debuglnF("settings.json corrupted or missing, force regenerating");
            createDefaultSettingsConfig(true);
            return getSettingsConfigAsJson(jdoc, false);
        }
        return true;
    } else {
        if (xSemaphoreTake(Mutexes::SPI, (TickType_t) 500)) {
            if (xSemaphoreTake(Mutexes::SDCard, (TickType_t) 500)) {
                bool success = SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
                if (!success) {
                    debuglnF("getSettingsConfigAsJson: SD begin failure");
                    return false;
                }
                File file = SD.open(FSPATH::Settings);
                if (!file) {
                    debuglnF("getSettingsConfigAsJson: problem with file");
                    SD.end();
                    xSemaphoreGive(Mutexes::SPI);
                    xSemaphoreGive(Mutexes::SDCard);
                    return false;
                }

                DeserializationError error = deserializeJson(jdoc, file);
                if (error) {
                    debugF("deserializeJson() failed: ");
                    debugln(error.f_str());
                }
                file.close();
                SD.end();
                xSemaphoreGive(Mutexes::SPI);
                xSemaphoreGive(Mutexes::SDCard);
                return !error;
            } else {
                xSemaphoreGive(Mutexes::SPI);
                debuglnF("sd semaphore problem");
                return false;
            }
        } else {
            debuglnF("spi semaphore problem");
            return false;
        }
    }
}

bool getSettingsConfig(SettingsConfig *sc, bool regenerate) {
    JsonDocument doc;
    bool success = getSettingsConfigAsJson(doc, regenerate);

    sc->adxl.ofx          = doc["ADXL"].as<JsonObject>()["OFX"];
    sc->adxl.ofy          = doc["ADXL"].as<JsonObject>()["OFY"];
    sc->adxl.ofz          = doc["ADXL"].as<JsonObject>()["OFZ"];
    sc->wifi.enabled      = doc["WiFi"].as<JsonObject>()["Enabled"];
    sc->wifi.ssid         = doc["WiFi"].as<JsonObject>()["SSID"].as<String>();
    sc->wifi.pswd         = doc["WiFi"].as<JsonObject>()["PSWD"].as<String>();
    sc->disp.brightness   = doc["Display"].as<JsonObject>()["Brightness"];
    sc->disp.font         = doc["Display"].as<JsonObject>()["Font"].as<String>();
    sc->disp.invertColors = doc["Display"].as<JsonObject>()["InvertColors"];
    sc->sfx.volume        = doc["SoundFX"].as<JsonObject>()["Volume"];
    sc->sfx.announce      = doc["SoundFX"].as<JsonObject>()["Announce"];
    sc->sfx.lang          = doc["SoundFX"].as<JsonObject>()["Lang"].as<String>();
    sc->haptic.enabled    = doc["Haptics"].as<JsonObject>()["Enabled"];
    sc->haptic.strength   = doc["Haptics"].as<JsonObject>()["Strength"];
    sc->haptic.pattern    = doc["Haptics"].as<JsonObject>()["Pattern"];
    sc->hid.mouseSense    = doc["HID"].as<JsonObject>()["MouseSensitivity"];

    // we should check if any of these are null
    // and throw something up, maybe even overwrite the existing config.
    // but range validation is up to the individual task.

    return success;
}

void printSettingsConfig(const SettingsConfig &sc) {
    debugf("x: %d\n", sc.adxl.ofx);
    debugf("y: %d\n", sc.adxl.ofy);
    debugf("z: %d\n", sc.adxl.ofz);
    debugf("w: %s\n", sc.wifi.enabled ? "TRUE" : "FALSE");
    debugf("u: %s\n", sc.wifi.ssid.c_str());
    debugf("p: %s\n", sc.wifi.pswd.c_str());
    debugf("b: %f\n", sc.disp.brightness);
    debugf("f: %s\n", sc.disp.font.c_str());
    debugf("c: %s\n", sc.disp.invertColors ? "TRUE" : "FALSE");
    debugf("v: %f\n", sc.sfx.volume);
    debugf("a: %s\n", sc.sfx.announce ? "TRUE" : "FALSE");
    debugf("d: %s\n", sc.sfx.lang.c_str());
    debugf("h: %s\n", sc.haptic.enabled ? "TRUE" : "FALSE");
    debugf("s: %f\n", sc.haptic.strength);
    debugf("n: %d\n", sc.haptic.pattern);
    debugf("m: %d\n", sc.hid.mouseSense);
}
