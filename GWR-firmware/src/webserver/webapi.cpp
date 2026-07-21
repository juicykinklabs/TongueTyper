#include "webapi.h"

#include <Arduino.h>
#include <SD.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "config/app_conf.h"
#include "config/constants.h"
#include "structs/AudioMessage.h"
#include "structs/DisplayMessage.h"
#include "util/settings.h"
#include "webserver/index.hpp"
#include "taskglobals.h"

#include "structs/HapticMessage.h"
#include "services/haptics.h"

WebServer server(80);

void handleNotFound() { server.send(HTTP_CODE_NOT_FOUND, "text/plain", "Not Found"); }
void handleRoot() { server.send(HTTP_CODE_OK, "text/html", Web::index); }
void handleAssetStyles() { server.send(HTTP_CODE_OK, "text/css", Web::styles); }
void handleAssetScript() { server.send(HTTP_CODE_OK, "application/javascript", Web::script); }

void serverAcknowledge() {
    server.send(HTTP_CODE_OK, "text/plain", "success");
}

void api_vibrate() {
    queueHapticPattern(pattern_Blip);
    serverAcknowledge();
}

void api_mode1() {
    DEVICE_MODE = DeviceMode_t::KEYBOARD;
    serverAcknowledge();
}
void api_mode2() {
    DEVICE_MODE = DeviceMode_t::JOYSTICK;
    serverAcknowledge();
}
void api_mode3() {
    DEVICE_MODE = DeviceMode_t::AUDIOIMAGE;
    serverAcknowledge();
}

void api_queueAudio() {
    // deserialize some json, create an audio message for the queue, and send
    //AudioMessage am;
    //am.instruction = AudioEventEnum::TTS;
    //strncpy(am.data, .....); 
//
    //xQueueSend(q_sfx_tts, (void*) &am, (TickType_t) 0);
    //serverAcknowledge();
}

void api_queueImage() {

}

void api_sensors() {
    char sensorDataJson[128];
    snprintf(sensorDataJson, 127, "{\"x\": %.2f, \"y\": %.2f, \"z\": %.2f, \"pressing\": %d, \"batt\": %.1f}", g_accelEvent.x, g_accelEvent.y, g_accelEvent.z,
             4,   // todo: use currently held button. maybe: g_userinput
             75.0 // todo: actual battery level (should be global, handled by some other task)
    );
    // debugln("Sending:");
    // debugln(g_accelEvent.x);
    // debugln(int(g_accelEvent.x));
    server.send(HTTP_CODE_OK, "application/json", sensorDataJson);
}

void api_color() {}
void api_volume() {}
void api_hid() {}

void api_form_set() {
    // recv json in args, each key is arg[i]
    debugln("args:");
    for (int i = 0; i < server.args(); i++) {
        debugln(server.arg(i));
    }
    JsonDocument doc;
    deserializeJson(doc, server.arg(0)); // todo catch error

    // settings expects doubles in range 0-1, api calls for percentages 0-100...
    // divide floating points by 100 here, I guess
    settings.disp.brightness   = doc["Brightness"].as<double>() / 100;
    settings.disp.invertColors = doc["InvertColors"].as<bool>();
    settings.sfx.volume        = doc["Volume"].as<double>() / 100;
    settings.sfx.announce      = doc["Announce"].as<bool>();
    settings.haptic.strength   = doc["HapticStrength"].as<double>() / 100;
    settings.hid.mouseSense    = doc["MouseSensitivity"].as<uint32_t>();
    // server.arg("")
    server.send(200, "okie dokie");
    // writeSettingsConfig()
}

void api_form_get() {
    xSemaphoreTake(Mutexes::SPI, portMAX_DELAY);
    xSemaphoreTake(Mutexes::SDCard, portMAX_DELAY);
    SD.begin(Pins::SD::CS, SPI, SPISpeed::SD);
    File fSettings = SD.open(FSPATH::Settings);
    server.streamFile(fSettings, "application/json", HTTP_CODE_OK);
    fSettings.close();
    SD.end();
    xSemaphoreGive(Mutexes::SDCard);
    xSemaphoreGive(Mutexes::SPI);
}

// *** MAIN TASK ***

void task_webserver(void *pv) {

    while (!g_wifiReady) {
        // wait for wifi
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    if (MDNS.begin(WIRELESS::hostname)) {
        MDNS.addService("http", "tcp", 80);
        debuglnF("MDNS responder started");
        debugf("Connect to http://%s.local (or %s)\n", WIRELESS::hostname, WiFi.localIP().toString().c_str());

    } else {
        // todo: handle some error
    }

    // basic web functionality
    server.onNotFound(handleNotFound);
    server.on("/", handleRoot);
    server.on("/styles.css", handleAssetStyles);
    server.on("/script.js", handleAssetScript);

    // API handles

    server.on("/api/setform", api_form_set);
    server.on("/api/getform", api_form_get);

    server.on("/api/sensors", api_sensors);
    server.on("/api/vibrate", api_vibrate);
    // server.on("/api/setMode", api_mode);
    // server.on("/api/setVolume", api_volume);
    // server.on("/api/setLed", api_led);
    // server.on("/api/setMouse", api_hid);

    server.begin();

    while (1) {
        server.handleClient();
        vTaskDelay(2);
    }

    vTaskDelete(NULL);
}