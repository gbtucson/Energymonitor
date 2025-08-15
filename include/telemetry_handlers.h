#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

inline void handleLiveTelemetry(AsyncWebServerRequest *request) {
  // Replace with actual telemetry JSON
  request->send(200, "application/json", "{\"voltage\":234.5,\"current\":12.3}");
}