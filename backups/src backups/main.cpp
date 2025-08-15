#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <time.h>
#include "pins.h"
#include "wifi_config.h"
#include "schedule_handler.h"
#include "relaycontrol.h"
#include "ev_controller.h"
#include "refoss_monitor.h"
#include "web_routes.h"
// #include "charginglogic.h"


AsyncWebServer server(80);

void setup() {
Serial.begin(115200);
Serial.println("Starting WiFi...");

// Set WiFi to station mode
WiFi.mode(WIFI_STA);

// Configure static IP
WiFi.setHostname("Energymonitor");  // Match router reservation
WiFi.config(IPAddress(192,168,0,40), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

// Begin WiFi connection
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

Serial.println("Connecting with static IP...");
unsigned long start = millis();
while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
  Serial.print(".");
  delay(500);
}

Serial.println(WiFi.status() == WL_CONNECTED ? "\nConnected!" : "\nFailed.");
Serial.println("IP address: " + WiFi.localIP().toString());

  if (LittleFS.begin()) {
    Serial.println("LittleFS mounted successfully");
  } else {
    Serial.println("LittleFS mount failed");
    return; // Prevent further setup if FS is unavailable
  }

  Serial.println("Listing LittleFS contents:");
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.println("File: " + String(file.name()) + " (" + file.size() + " bytes)");
    file = root.openNextFile();
  }

  // loadChargingSettings(); // Make sure this comes after LittleFS.begin()

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  initRefossMonitors();
  initEVController();
  setScheduleEnabled(true);
  setRelayState(false);

  registerWebRoutes(server);
  server.begin();

  server.serveStatic("/log", LittleFS, "/log.html");
  server.serveStatic("/monitor", LittleFS, "/monitor.html");
  server.serveStatic("/charging", LittleFS, "/charging.html");
  server.serveStatic("/settings", LittleFS, "/settings.html");
  server.serveStatic("/js/", LittleFS, "/js/");
}

void loop() {
  EnergyData data = getLatestEnergyData();
  bool isPeak = isWeekend();

  if (manualOverrideActive()) {
    setRelayState(getManualOverrideState());
  } else {
    bool shouldCharge = evaluateChargingLogic(data, isPeak);
    setRelayState(shouldCharge);
  }

  delay(1000);
}
