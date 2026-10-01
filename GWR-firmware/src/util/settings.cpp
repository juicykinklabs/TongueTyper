#include "settings.h"

#include <ArduinoJson.h>

#include "config/app_conf.h"
#include "config/constants.h"
#include "util/sd_ops.h"

#include "taskglobals.h"

namespace {
    template <typename T> bool readSetting(JsonVariantConst value, const char *path, const char *expectedType, T &out) {
        if (value.isNull()) {
            debugf("%s missing or null (expects %s)\n", path, expectedType);
            return false;
        }

        if (!value.is<T>()) {
            debugf("%s has the wrong type (expects %s)\n", path, expectedType);
            return false;
        }

        out = value.as<T>();
        return true;
    }

} // namespace

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

    if (sdOpBegin()) {
        bool success = jdocToFile(FSPATH::Settings, doc);
        sdOpEnd();
        return success;
    }
    return false;
}

bool deleteSettingsConfig() {
    if (sdOpBegin()) {
        bool success = fileRemove(FSPATH::Settings);
        sdOpEnd();
        return success;
    }
    return false;
}

void populateDefaultSettingsConfig() {

    // we can use = with strings here because they are from the String class.
    // todo: maybe configure these in a separate header file
    defaultsettings.adxl.ofx          = 0;
    defaultsettings.adxl.ofy          = 0;
    defaultsettings.adxl.ofz          = 0;
    defaultsettings.wifi.enabled      = true;
    defaultsettings.wifi.ssid         = WIRELESS::defaultSSID;
    defaultsettings.wifi.pswd         = WIRELESS::defaultPSWD;
    defaultsettings.disp.brightness   = 0.5;
    defaultsettings.disp.font         = "default";
    defaultsettings.disp.invertColors = false;
    defaultsettings.sfx.volume        = 0.75;
    defaultsettings.sfx.announce      = false;
    defaultsettings.sfx.lang          = "en";
    defaultsettings.haptic.enabled    = true;
    defaultsettings.haptic.strength   = 0.8;
    defaultsettings.haptic.pattern    = 2;
    defaultsettings.hid.mouseSense    = 1200; // pixels per second maximum
}

bool getSettingsConfigAsJson(JsonDocument &jdoc) {
    if (sdOpBegin()) {
        bool success = fileToJdoc(FSPATH::Settings, jdoc);
        sdOpEnd();
        return success;
    }
    return false;
}

bool getSettingsConfig(SettingsConfig *sc, bool allow_modification) {

    populateDefaultSettingsConfig();
    
    // check for first-time settings write
    if (sdOpBegin()) {
        if (not fileExists(FSPATH::Settings)) {
            debugln("No settings-config, creating one from scratch");
            sdOpEnd(); // yikes
            return writeSettingsConfig(defaultsettings);
        }
        sdOpEnd(); // yikes
    }

    JsonDocument doc;
    bool success = getSettingsConfigAsJson(doc);

    if (!success || doc.isNull()) {
        if(allow_modification) {
            debuglnF("settings.json corrupted, writing full defaults");
            return writeSettingsConfig(defaultsettings);
        } else {
            // regeneration not allowed:
            return false;
        }
    }

    debugf("json doc size: %d items\n", doc.size());

    bool read_went_ok = true;

    if(not readSetting(doc["ADXL"]["OFX"], "ADXL.OFX", "integer", sc->adxl.ofx)) {
        sc->adxl.ofx = defaultsettings.adxl.ofx;
        read_went_ok = false;
    }
    if(not readSetting(doc["ADXL"]["OFY"], "ADXL.OFY", "integer", sc->adxl.ofy)) {
        sc->adxl.ofy = defaultsettings.adxl.ofy;
        read_went_ok = false;
    }
    if(not readSetting(doc["ADXL"]["OFZ"], "ADXL.OFZ", "integer", sc->adxl.ofz)) {
        sc->adxl.ofz = defaultsettings.adxl.ofz;
        read_went_ok = false;
    }
    if(not readSetting(doc["WiFi"]["Enabled"], "WiFi.Enabled", "boolean", sc->wifi.enabled)) {
        sc->wifi.enabled = defaultsettings.wifi.enabled;
        read_went_ok = false;
    }
    if(not readSetting(doc["WiFi"]["SSID"], "WiFi.SSID", "string", sc->wifi.ssid)) {
        sc->wifi.ssid = defaultsettings.wifi.ssid;
        read_went_ok = false;
    }
    if(not readSetting(doc["WiFi"]["PSWD"], "WiFi.PSWD", "string", sc->wifi.pswd)) {
        sc->wifi.pswd = defaultsettings.wifi.pswd;
        read_went_ok = false;
    }
    if(not readSetting(doc["Display"]["Brightness"], "Display.Brightness", "number", sc->disp.brightness)) {
        sc->disp.brightness = defaultsettings.disp.brightness;
        read_went_ok = false;
    }
    if(not readSetting(doc["Display"]["Font"], "Display.Font", "string", sc->disp.font)) {
        sc->disp.font = defaultsettings.disp.font;
        read_went_ok = false;
    }
    if(not readSetting(doc["Display"]["InvertColors"], "Display.InvertColors", "boolean", sc->disp.invertColors)) {
        sc->disp.invertColors = defaultsettings.disp.invertColors;
        read_went_ok = false;
    }
    if(not readSetting(doc["SoundFX"]["Announce"], "SoundFX.Announce", "boolean", sc->sfx.announce)) {
        sc->sfx.announce = defaultsettings.sfx.announce;
        read_went_ok = false;
    }
    if(not readSetting(doc["SoundFX"]["Lang"], "SoundFX.Lang", "string", sc->sfx.lang)) {
        sc->sfx.lang = defaultsettings.sfx.lang;
        read_went_ok = false;
    }
    if(not readSetting(doc["SoundFX"]["Volume"], "SoundFX.Volume", "number", sc->sfx.volume)) {
        sc->sfx.volume = defaultsettings.sfx.volume;
        read_went_ok = false;
    }
    if(not readSetting(doc["Haptics"]["Enabled"], "Haptics.Enabled", "boolean", sc->haptic.enabled)) {
        sc->haptic.enabled = defaultsettings.haptic.enabled;
        read_went_ok = false;
    }
    if(not readSetting(doc["Haptics"]["Strength"], "Haptics.Strength", "number", sc->haptic.strength)) {
        sc->haptic.strength = defaultsettings.haptic.strength;
        read_went_ok = false;
    }
    if(not readSetting(doc["Haptics"]["Pattern"], "Haptics.Pattern", "integer", sc->haptic.pattern)) {
        sc->haptic.pattern = defaultsettings.haptic.pattern;
        read_went_ok = false;
    }
    if(not readSetting(doc["HID"]["MouseSensitivity"], "HID.MouseSensitivity", "integer", sc->hid.mouseSense)) {
        sc->hid.mouseSense = defaultsettings.hid.mouseSense;
        read_went_ok = false;
    }

    
    if (!success || !read_went_ok) {
        if (allow_modification) {
            debugln("a setting was invalid, writing back its default value");
            return writeSettingsConfig(settings);
        } else {
            return false;
        }
    }
    
    debugln("settings all okay");
    return true;
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
