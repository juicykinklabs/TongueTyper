#pragma once
#include <Arduino.h>
#include <WebServer.h>

extern WebServer server;

void handleRoot();
void handleNotFound();

void task_server_test(void* pv);