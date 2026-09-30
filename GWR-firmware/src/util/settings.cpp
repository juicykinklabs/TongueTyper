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

    bool need_write = false;
    if (sdOpBegin()) {
        if (not fileExists(FSPATH::Settings)) {
            debugln("No settings-config, creating one from scratch");
            need_write = true;
        } else if (overWriteExisting) {
            debugln("Manually overwriting settings config");
            need_write = true;
        }
        sdOpEnd();
    }

    if (need_write) {
        // has its own sd begin/end
        writeSettingsConfig(defaults);
    }
}

bool getSettingsConfigAsJson(JsonDocument &jdoc, bool allow_regeneration) {
    if (allow_regeneration) {
        bool success = getSettingsConfigAsJson(jdoc, false);
        if (!success) {
            debuglnF("settings.json corrupted or missing, regenerating");
            createDefaultSettingsConfig(true);
            return getSettingsConfigAsJson(jdoc, false);
        }
        return true;
    } else {

        if (sdOpBegin()) {
            bool success = fileToJdoc(FSPATH::Settings, jdoc);
            sdOpEnd();
            return success;
        }
        return false;
    }
}

bool getSettingsConfig(SettingsConfig *sc, bool allow_regeneration) {
    JsonDocument doc;
    bool success = getSettingsConfigAsJson(doc, allow_regeneration);

    if (doc.isNull()) {
        debugln("json doc is null. uh oh");
    }

    bool ok = true;

    if(not readSetting(doc["ADXL"]["OFX"], "ADXL.OFX", "integer", sc->adxl.ofx)) {
        ok = false;
    }
    if(not readSetting(doc["ADXL"]["OFY"], "ADXL.OFY", "integer", sc->adxl.ofy)) {
        ok = false;
    }
    if(not readSetting(doc["ADXL"]["OFZ"], "ADXL.OFZ", "integer", sc->adxl.ofz)) {
        ok = false;
    }
    if(not readSetting(doc["WiFi"]["Enabled"], "WiFi.Enabled", "boolean", sc->wifi.enabled)) {
        ok = false;
    }
    if(not readSetting(doc["WiFi"]["SSID"], "WiFi.SSID", "string", sc->wifi.ssid)) {
        ok = false;
    }
    if(not readSetting(doc["WiFi"]["PSWD"], "WiFi.PSWD", "string", sc->wifi.pswd)) {
        ok = false;
    }
    if(not readSetting(doc["Display"]["Brightness"], "Display.Brightness", "number", sc->disp.brightness)) {
        ok = false;
    }
    if(not readSetting(doc["Display"]["Font"], "Display.Font", "string", sc->disp.font)) {
        ok = false;
    }
    if(not readSetting(doc["Display"]["InvertColors"], "Display.InvertColors", "boolean", sc->disp.invertColors)) {
        ok = false;
    }
    if(not readSetting(doc["SoundFX"]["Announce"], "SoundFX.Announce", "boolean", sc->sfx.announce)) {
        ok = false;
    }
    if(not readSetting(doc["SoundFX"]["Lang"], "SoundFX.Lang", "string", sc->sfx.lang)) {
        ok = false;
    }
    if(not readSetting(doc["SoundFX"]["Volume"], "SoundFX.Volume", "number", sc->sfx.volume)) {
        ok = false;
    }
    if(not readSetting(doc["Haptics"]["Enabled"], "Haptics.Enabled", "boolean", sc->haptic.enabled)) {
        ok = false;
    }
    if(not readSetting(doc["Haptics"]["Strength"], "Haptics.Strength", "number", sc->haptic.strength)) {
        ok = false;
    }
    if(not readSetting(doc["Haptics"]["Pattern"], "Haptics.Pattern", "integer", sc->haptic.pattern)) {
        ok = false;
    }
    if(not readSetting(doc["HID"]["MouseSensitivity"], "HID.MouseSensitivity", "integer", sc->hid.mouseSense)) {
        ok = false;
    }

    debugf("doc size: %d items\n", doc.size());
    if (not ok) {
        debugln("!!! BIG ERROR IN SETTINGS FILE !!!");
    } else {
        debugln("settings all okay");
    }

    // todo if any setting is missing, overwrite JUST it, with a good default
    // but range validation is still up to the individual task.

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
