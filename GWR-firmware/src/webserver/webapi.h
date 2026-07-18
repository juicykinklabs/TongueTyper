#pragma once
#include <Arduino.h>
#include <WebServer.h>

extern WebServer server;

void handleNotFound();
void handleRoot();
void handleAssetStyles();
void handleAssetScript();

void serverAcknowledge();

// triggers
void api_vibrate();
void api_mode1();
void api_mode2();
void api_mode3();

void api_queueAudio();
void api_queueImage();

// getters
void api_sensors();
// setters (POST)
void api_color(); // might go in form_set
void api_volume();
void api_hid();

// Settings:
// receive a POST request to change some setting.json field
// and save it to the SD card
void api_form_set();
void api_form_get();


void task_webserver(void* pv);