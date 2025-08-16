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

  // Optional: add derived fields for frontend display
  doc["netSurplus"] = 0; // Placeholder, update with real telemetry if available
  doc["aggressiveness"] = (chargingRate >= 30) ? "aggressive" :
                          (chargingRate >= 15) ? "moderate" : "conservative";

  String output;
  serializeJson(doc, output);
  return output;
}

// Accepts JSON payload to update timer settings and persistent config
bool updateChargingSettings(const String& json) {
  Serial.println("[updateChargingSettings] Received JSON:");
  Serial.println(json);

  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    Serial.print("[updateChargingSettings] JSON parse failed: ");
    Serial.println(err.c_str());
    return false;
  }

  if (doc.containsKey("minimumPVWattsToCharge")) {
    minimumPVWattsToCharge = doc["minimumPVWattsToCharge"];
    Serial.print("[updateChargingSettings] Parsed minimumPVWattsToCharge: ");
    Serial.println(minimumPVWattsToCharge);
  }

  File f = LittleFS.open("/charging-settings.json", "w");
  if (!f) {
    Serial.println("[updateChargingSettings] Failed to open file for writing.");
    return false;
  }

  StaticJsonDocument<256> out;
  out["minimumPVWattsToCharge"] = minimumPVWattsToCharge;
  serializeJson(out, f);
  f.close();

  Serial.println("[updateChargingSettings] File written successfully.");
  return true;
}


// Load settings from flash on boot
void loadChargingSettings() {
  Serial.println("[loadChargingSettings] Attempting to open /charging-settings.json");

  File f = LittleFS.open("/charging-settings.json", "r");
  if (!f) {
    Serial.println("[loadChargingSettings] File not found.");
    return;
  }

  Serial.println("[loadChargingSettings] File opened successfully.");

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    Serial.print("[loadChargingSettings] JSON parse failed: ");
    Serial.println(err.c_str());
    return;
  }

  if (doc.containsKey("minimumPVWattsToCharge")) {
    minimumPVWattsToCharge = doc["minimumPVWattsToCharge"];
    Serial.print("[loadChargingSettings] Loaded minimumPVWattsToCharge: ");
    Serial.println(minimumPVWattsToCharge);
  } else {
    Serial.println("[loadChargingSettings] Key 'minimumPVWattsToCharge' not found.");
  }
}

