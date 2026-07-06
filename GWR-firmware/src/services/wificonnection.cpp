#include "wificonnection.h"

#include <Arduino.h>
#include <WiFi.h>

#include "config/app_conf.h"
#include "taskglobals.h"

void task_wificonnection(void *pv) {

    debuglnF("reading wifi settings");
    if (settings.wifi.enabled) {
        debugf("SSID: %c%c****\n",settings.wifi.ssid.c_str()[0], settings.wifi.ssid.c_str()[1]);
        debugf("PSWD: %c%c****\n",settings.wifi.pswd.c_str()[0], settings.wifi.pswd.c_str()[1]);
    } else {
        debugln("WiFi disabled");
    }

    while (1) {
        if (settings.wifi.enabled) {
            if (WiFi.status() != WL_CONNECTED) {
                g_wifiReady = false;
                WiFi.begin(settings.wifi.ssid.c_str(), settings.wifi.pswd.c_str());
                while (WiFi.status() != WL_CONNECTED) {
                    vTaskDelay(1500 / portTICK_PERIOD_MS);
                }
                debuglnF("wifi connected");
                g_wifiReady = true;
            }
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}