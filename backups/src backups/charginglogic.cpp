#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include "charginglogic.h"
#include "chargingcontrol.h"

// Timer-based charging state
bool chargingTimerActive = false;
unsigned long chargingStartTime = 0;
unsigned long chargingDurationMs = 0;
int chargingRate = 16; // Default rate in amps

// Persistent setting
int minimumPVWattsToCharge = 500; // Default threshold

// Called periodically from loop()
void updateChargingLoop() {
  if (chargingTimerActive) {
    unsigned long now = millis();
    if (now - chargingStartTime >= chargingDurationMs) {
      stopCharging();
      chargingTimerActive = false;
      return;
    }
    setChargingCurrent(chargingRate);
    return;
  }

  // Future: PV-aware logic, override handling, etc.
  stopCharging(); // Default fallback if no mode is active
}

// Returns current status as JSON string
String getChargingStatusJson() {
  StaticJsonDocument<512> doc;

  doc["chargingTimerActive"] = chargingTimerActive;
  doc["chargingStartTime"] = chargingStartTime;
  doc["chargingDurationMs"] = chargingDurationMs;
  doc["chargingRate"] = chargingRate;
  doc["minimumPVWattsToCharge"] = minimumPVWattsToCharge;

  String output;
  serializeJson(doc, output);
  return output;
}

// Accepts JSON payload to update timer settings and persistent config
bool updateChargingSettings(const String& json) {
  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) return false;

  if (doc.containsKey("chargingDurationMs")) {
    chargingDurationMs = doc["chargingDurationMs"];
  }
  if (doc.containsKey("chargingRate")) {
    chargingRate = doc["chargingRate"];
  }
  if (doc.containsKey("minimumPVWattsToCharge")) {
    minimumPVWattsToCharge = doc["minimumPVWattsToCharge"];
  }

  chargingStartTime = millis();
  chargingTimerActive = true;

  // Save persistent settings
  File f = LittleFS.open("/chargingconfig.json", "w");
  if (f) {
    StaticJsonDocument<256> out;
    out["minimumPVWattsToCharge"] = minimumPVWattsToCharge;
    serializeJson(out, f);
    f.close();
  }

  return true;
}

// Load settings from flash on boot
void loadChargingSettings() {
  File f = LittleFS.open("/chargingconfig.json", "r");
  if (!f) return;

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) return;

  if (doc.containsKey("minimumPVWattsToCharge")) {
    minimumPVWattsToCharge = doc["minimumPVWattsToCharge"];
  }
}

