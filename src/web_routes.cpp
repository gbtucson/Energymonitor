#include "web_routes.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "relaycontrol.h"
#include "schedule_handler.h"
#include "energylogic.h"
#include "charginglogic.h" // Temporarily disabled due to early crash risk

void handleClampConfigLoad(AsyncWebServerRequest* request) {
  request->send(200, "application/json", "{}"); // Stub: replace with actual clamp config
}

void handleClampConfigSave(AsyncWebServerRequest* request) {
  request->send(200, "text/plain", "Clamp config saved"); // Stub: replace with actual save logic
}

void handleLiveTelemetry(AsyncWebServerRequest* request) {
  request->send(200, "application/json", "{\"voltage\": 120.0}"); // Stub: replace with real telemetry
}

void registerWebRoutes(AsyncWebServer& server) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<html><body><h2>ESP32 Controller</h2><p><a href='/log'>Dashboard</a></p><p><a href='/schedule'>Scheduler</a></p></body></html>");
  });

  server.on("/clamps/config", HTTP_GET, handleClampConfigLoad);
  server.on("/clamps/config", HTTP_POST, handleClampConfigSave);
  server.on("/telemetry/live", HTTP_GET, handleLiveTelemetry);

  server.on("/scheduler/update", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String body((char*)data, len);
    bool success = updateScheduleFromJson(body);
    request->send(success ? 200 : 400, "text/plain", success ? "Schedule updated" : "Invalid schedule JSON");
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><body><h2>Energy Dashboard</h2>";
    html += "<p><a href='/status.json'>View Live Status (JSON)</a></p>";
    html += "<p><a href='/schedule'>Edit Scheduler</a></p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/schedule", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/schedule.html", "text/html");
  });

  server.on("/schedule.json", HTTP_GET, [](AsyncWebServerRequest *request){
    LittleFS.exists("/schedule.json") ?
      request->send(LittleFS, "/schedule.json", "application/json") :
      request->send(404, "text/plain", "No schedule found");
  });

  server.on("/delete-schedule", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String body((char*)data, len);
    DynamicJsonDocument incoming(256);
    if (deserializeJson(incoming, body)) {
      request->send(400, "text/plain", "Invalid JSON");
      return;
    }

    String targetName = incoming["name"].as<String>();
    if (targetName == "") {
      request->send(400, "text/plain", "Missing schedule name");
      return;
    }

    DynamicJsonDocument doc(4096);
    File file = LittleFS.open("/schedule.json", "r");
    if (file) {
      deserializeJson(doc, file);
      file.close();
    }

    JsonArray schedules = doc.is<JsonArray>() ? doc.as<JsonArray>() : doc.to<JsonArray>();
    bool found = false;
    for (size_t i = 0; i < schedules.size(); i++) {
      if (schedules[i]["name"].as<String>() == targetName) {
        schedules.remove(i);
        found = true;
        break;
      }
    }

    if (!found) {
      request->send(404, "text/plain", "Schedule not found");
      return;
    }

    file = LittleFS.open("/schedule.json", "w");
    if (!file) {
      request->send(500, "text/plain", "Failed to open file for writing");
      return;
    }
    serializeJson(doc, file);
    file.close();
    request->send(200, "text/plain", "Schedule deleted");
  });

  server.on("/add-schedule", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String body((char*)data, len);
    DynamicJsonDocument incoming(1024);
    if (deserializeJson(incoming, body)) {
      request->send(400, "text/plain", "Invalid JSON");
      return;
    }

    DynamicJsonDocument doc(4096);
    File file = LittleFS.open("/schedule.json", "r");
    if (file) {
      deserializeJson(doc, file);
      file.close();
    }

    JsonArray schedules = doc.is<JsonArray>() ? doc.as<JsonArray>() : doc.to<JsonArray>();
    String newName = incoming["name"].as<String>();
    bool replaced = false;

    for (size_t i = 0; i < schedules.size(); i++) {
      if (schedules[i]["name"].as<String>() == newName) {
        schedules[i] = incoming;
        replaced = true;
        break;
      }
    }

    if (!replaced) {
      schedules.add(incoming);
    }

    file = LittleFS.open("/schedule.json", "w");
    if (!file || file.isDirectory()) {
      request->send(500, "text/plain", "Failed to open file for writing");
      return;
    }

    size_t written = serializeJson(doc, file);
    file.close();

    if (written == 0) {
      request->send(500, "text/plain", "Failed to write JSON");
      return;
    }

    request->send(200, "text/plain", replaced ? "Schedule updated" : "Schedule added");
  });

  server.on("/setRelay", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("state")) {
      String s = request->getParam("state")->value();
      if (s == "auto") {
        clearManualOverride();
      } else {
        setManualOverride(true, (s == "on"));
      }

      if (!manualOverrideActive()) {
        setRelayState(getRelayStateFromSchedule("/schedule.json") == "ON");
      }
    }
    request->redirect("/log");
  });

  server.on("/setSchedule", HTTP_GET, [](AsyncWebServerRequest *r) {
    setScheduleEnabled(r->hasParam("enable"));
    r->redirect("/schedule");
  });

  server.on("/debug-schedule", HTTP_GET, [](AsyncWebServerRequest *request){
    File file = LittleFS.open("/schedule.json", "r");
    if (!file) {
      request->send(500, "text/plain", "Unable to open schedule.json");
      return;
    }
    String contents = file.readString();
    file.close();
    request->send(200, "text/plain", contents);
  });

  server.serveStatic("/", LittleFS, "/");

  server.on("/charging-status.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = getChargingStatusJson();
    request->send(200, "application/json", json);
  });

  server.on("/charging-settings.json", HTTP_POST, [](AsyncWebServerRequest *request) {}, nullptr,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String body((char*)data, len);
    bool success = updateChargingSettings(body);
    request->send(200, "application/json", success ? "{\"status\":\"ok\"}" : "{\"status\":\"error\"}");
  });

  server.on("/charging-settings.json", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!LittleFS.exists("/charging-settings.json")) {
      request->send(404, "application/json", "{\"error\":\"No config found\"}");
      return;
    }
    request->send(LittleFS, "/charging-settings.json", "application/json");
  });

  server.on("/api/save-settings", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    Serial.println("[save-settings] Received POST");

    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      Serial.println("[save-settings] JSON parse error");
      request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

      if (!doc.containsKey("minimumPVWattsToCharge") || !doc["minimumPVWattsToCharge"].is<int>()) {
      Serial.println("[save-settings] Invalid or missing 'minimumPVWattsToCharge'");
      request->send(400, "application/json", "{\"error\":\"Missing or invalid 'minimumPVWattsToCharge'\"}");
      return;
    }

    File file = LittleFS.open("/charging-settings.json", "w");
    if (!file) {
      Serial.println("[save-settings] Failed to open file for writing");
      request->send(500, "application/json", "{\"error\":\"Failed to open file\"}");
      return;
    }

    size_t written = serializeJson(doc, file);
    file.close();

    if (written == 0) {
      Serial.println("[save-settings] Failed to write JSON");
      request->send(500, "application/json", "{\"error\":\"Failed to write JSON\"}");
      return;
    }

    Serial.println("[save-settings] Settings saved successfully");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  server.on("/status.json", HTTP_GET, [](AsyncWebServerRequest *request){
    EnergyData data = getLatestEnergyData();
    bool isPeak = (getRelayStateFromSchedule("/schedule.json") == "OFF");
    bool relay = manualOverrideActive() ? getManualOverrideState() : evaluateChargingLogic(data, isPeak);
    int chargeRate = getChargeRateWatts(data, isPeak);

    DynamicJsonDocument doc(512);
    doc["pvWatts"] = data.pvWatts;
    doc["homeWatts"] = data.homeWatts;
    doc["evWatts"] = data.evWatts;
    doc["relayState"] = relay;
    doc["manualOverride"] = manualOverrideActive();
    doc["manualState"] = getManualOverrideState();
    doc["scheduleEnabled"] = isScheduleEnabled();
    doc["scheduleMode"] = isPeak ? "Peak" : "Regular";
    doc["chargeRateWatts"] = chargeRate;

    // Stubbed due to missing charginglogic.h
    doc["chargingTimerActive"] = false;
    doc["chargingTimeRemaining"] = 0;

    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
  });
}
