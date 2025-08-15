#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

inline void handleClampConfigLoad(AsyncWebServerRequest *request) {
  File file = LittleFS.open("/clamp-config.json", "r");
  if (!file) {
    request->send(500, "application/json", "{\"error\":\"File not found\"}");
    return;
  }

  String json = file.readString();
  file.close();
  request->send(200, "application/json", json);
}

inline void handleClampConfigSave(AsyncWebServerRequest *request) {
  if (request->hasParam("body", true)) {
    String body = request->getParam("body", true)->value();
    File file = LittleFS.open("/clamp-config.json", "w");
    if (!file) {
      request->send(500, "application/json", "{\"error\":\"Write failed\"}");
      return;
    }

    file.print(body);
    file.close();
    request->send(200, "application/json", "{\"status\":\"saved\"}");
  } else {
    request->send(400, "application/json", "{\"error\":\"No body\"}");
  }
}
