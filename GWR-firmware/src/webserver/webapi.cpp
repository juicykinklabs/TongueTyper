#include "webapi.h"

#include <Arduino.h>
#include <WiFi.h>
// #include <NetworkClient.h>
#include <WebServer.h>

#include "taskglobals.h"

WebServer server(80);

void handleRoot() {
    server.send(200, "text/plain", "connection is gud");
}

void handleNotFound() {
    server.send(404, "text/plain", "Not Found");
}

void task_server_test(void *pv) {

    while (!g_wifiReady) {
        // wait for wifi
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);
    server.begin();

    while (1) {
        server.handleClient();
        vTaskDelay(5);
    }

    vTaskDelete(NULL);
}