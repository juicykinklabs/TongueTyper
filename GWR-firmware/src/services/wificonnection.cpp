#include "wificonnection.h"

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "config/app_conf.h"
#include "taskglobals.h"

void task_wificonnection(void *pv) {

    static bool firstTimeConnect = false;
    debuglnF("reading wifi settings");
    if (settings.wifi.enabled) {
        debugf("SSID: %c%c****\n",settings.wifi.ssid.c_str()[0], settings.wifi.ssid.c_str()[1]);
        debugf("PSWD: %c%c****\n",settings.wifi.pswd.c_str()[0], settings.wifi.pswd.c_str()[1]);
    } else {
        debugln("WiFi disabled");
    }

    if (!strcasecmp(settings.wifi.ssid.c_str(), "myPassword")) {
        firstTimeConnect = true;
    }

    while (1) {
        if (settings.wifi.enabled) {
            if (WiFi.status() != WL_CONNECTED) {
                g_wifiReady = false;
                if (firstTimeConnect) {
                    // call improv setup here
                    // improvwifi(); or something. it must update the settings object, and save to SD card
                    firstTimeConnect = false;
                }
                WiFi.setHostname("TTyper-Alpha"); // letters, numbers, and dashes only
                WiFi.mode(WIFI_STA);
                WiFi.begin(settings.wifi.ssid.c_str(), settings.wifi.pswd.c_str());
                // if (!MDNS.begin("esp32_task_test")) {
                //     // todo: handle some error
                // }
                uint32_t t_tryConnectStart = millis();
                while (WiFi.status() != WL_CONNECTED) {
                    vTaskDelay(1500 / portTICK_PERIOD_MS);
                    if (millis() > (t_tryConnectStart + 15000)) {
                        // no connection for 15 sec.
                        firstTimeConnect = true;
                        break;
                    }
                }
                if (firstTimeConnect) {
                    continue;
                } else {
                    debuglnF("wifi connected");
                    debugln(WiFi.localIP());
                    debugln(WiFi.RSSI());
                    g_wifiReady = true;
                }
            }
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}