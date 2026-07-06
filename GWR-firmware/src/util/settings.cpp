#include "settings.h"

#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

#include "config/app_conf.h"

bool writeSettingsConfig(const SettingsConfig &sc) {
    SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
    // todo check if begin was ok
    SD.remove("/sys/setting.json"); // todo make configurable or defined
    JsonDocument doc;

    File file = SD.open("/sys/setting.json", FILE_WRITE);
    // todo check if file is ok

    doc["ADXL"]["NTSPX"]                          = sc.adxl.ntsp_xmeas;
    doc["ADXL"]["NTSPY"]                          = sc.adxl.ntsp_ymeas;
    doc["ADXL"]["NTSPZ"]                          = sc.adxl.ntsp_zmeas;
    doc["WiFi"]["Enabled"]                        = sc.wifi.enabled;
    doc["WiFi"]["SSID"]                           = sc.wifi.ssid;
    doc["WiFi"]["PSWD"]                           = sc.wifi.pswd;
    doc["TongueTyper"]["Display"]["Brightness"]   = sc.disp.brightness;
    doc["TongueTyper"]["Display"]["Font"]         = sc.disp.font;
    doc["TongueTyper"]["Display"]["InvertColors"] = sc.disp.invertColors;
    doc["TongueTyper"]["SoundFX"]["Volume"]       = sc.sfx.volume;
    doc["TongueTyper"]["SoundFX"]["Announce"]     = sc.sfx.announce;
    doc["TongueTyper"]["SoundFX"]["Lang"]         = sc.sfx.lang;
    doc["TongueTyper"]["Haptics"]["Enabled"]      = sc.haptic.enabled;
    doc["TongueTyper"]["Haptics"]["Strength"]     = sc.haptic.strength;
    doc["TongueTyper"]["Haptics"]["Pattern"]      = sc.haptic.pattern;
    doc["HID"]["MouseSensitivity"]                = sc.hid.mouseSense;

    if (serializeJsonPretty(doc, file) == 0) {
        debuglnF("Failed to write to file");
        return false;
    }

    // Close the file
    file.close();
    SD.end();
    return true;
}

void deleteSettingsConfig() {
    SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
    // todo check if begin ok
    SD.remove("/sys/setting.json"); // todo make configurable or defined
    SD.end();
}

void createDefaultSettingsConfig(bool overWriteExisting) {

    SettingsConfig defaults;
    // we can use = with strings here because they are from the String class.
    // todo: configure these in a separate header file
    defaults.adxl.ntsp_xmeas   = 0;
    defaults.adxl.ntsp_ymeas   = 0;
    defaults.adxl.ntsp_zmeas   = 265;
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
    File file = SD.open("/sys/setting.json", FILE_READ);

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

bool getSettingsConfig(SettingsConfig *sc, bool generateDefaultsIfMissing) {
    bool ret = true;
    SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
    // todo check if begin was ok
    File file = SD.open("/sys/setting.json"); // todo make configurable or defined
    // todo: check if(file)
    if (!file) {
        ret = false;
        debuglnF("getSettingsConfig: problem with file");
        if (generateDefaultsIfMissing) {
            debuglnF("settings.json corrupted or missing, force regenerating");
            file.close();
            SD.end();

            createDefaultSettingsConfig(true);
            ret = getSettingsConfig(sc, false); // recurse once, child wont reach this line
        }
        return ret;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        debugF("deserializeJson() failed: ");
        debugln(error.f_str());
        file.close();
        SD.end();
        return false;
    }

    sc->adxl.ntsp_xmeas   = doc["ADXL"].as<JsonObject>()["NTSPX"];
    sc->adxl.ntsp_ymeas   = doc["ADXL"].as<JsonObject>()["NTSPY"];
    sc->adxl.ntsp_zmeas   = doc["ADXL"].as<JsonObject>()["NTSPZ"];
    sc->wifi.enabled      = doc["WiFi"].as<JsonObject>()["Enabled"];
    sc->wifi.ssid         = doc["WiFi"].as<JsonObject>()["SSID"].as<String>();
    sc->wifi.pswd         = doc["WiFi"].as<JsonObject>()["PSWD"].as<String>();
    sc->disp.brightness   = doc["TongueTyper"].as<JsonObject>()["Display"].as<JsonObject>()["Brightness"];
    sc->disp.font         = doc["TongueTyper"].as<JsonObject>()["Display"].as<JsonObject>()["Font"].as<String>();
    sc->disp.invertColors = doc["TongueTyper"].as<JsonObject>()["Display"].as<JsonObject>()["InvertColors"];
    sc->sfx.volume        = doc["TongueTyper"].as<JsonObject>()["SoundFX"].as<JsonObject>()["Volume"];
    sc->sfx.announce      = doc["TongueTyper"].as<JsonObject>()["SoundFX"].as<JsonObject>()["Announce"];
    sc->sfx.lang          = doc["TongueTyper"].as<JsonObject>()["SoundFX"].as<JsonObject>()["Lang"].as<String>();
    sc->haptic.enabled    = doc["TongueTyper"].as<JsonObject>()["Haptics"].as<JsonObject>()["Enabled"];
    sc->haptic.strength   = doc["TongueTyper"].as<JsonObject>()["Haptics"].as<JsonObject>()["Strength"];
    sc->haptic.pattern    = doc["TongueTyper"].as<JsonObject>()["Haptics"].as<JsonObject>()["Pattern"];
    sc->hid.mouseSense    = doc["HID"].as<JsonObject>()["MouseSensitivity"];

    // we should check if any of these are null
    // and throw something up, maybe overwrite the existing config.
    file.close();
    SD.end();

    return true;
}

void printSettingsConfig(const SettingsConfig &sc) {
    debugf("x: %d\n", sc.adxl.ntsp_xmeas);
    debugf("y: %d\n", sc.adxl.ntsp_ymeas);
    debugf("z: %d\n", sc.adxl.ntsp_zmeas);
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

void testFunctions() {

    deleteSettingsConfig();

    SettingsConfig settingsconfig;

    debugln("gettings settings config");

    getSettingsConfig(&settingsconfig); // first call should generate a new
                                        // config, as it was deleted

    printSettingsConfig(settingsconfig);

    // test: increment a value
    settingsconfig.hid.mouseSense += 1;

    debugln("writing it back");

    writeSettingsConfig(settingsconfig);

    getSettingsConfig(&settingsconfig);

    debugln("here it is again, freshly read:");
    printSettingsConfig(settingsconfig);

    debugln("done");
}