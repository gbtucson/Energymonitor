#include "schedule_handler.h"
#include <ArduinoJson.h>
#include <time.h>

bool scheduleEnabled = true;

// struct ScheduleBlock {
//   String start;
//   String end;
//   bool enabled;
// };

std::vector<ScheduleBlock> scheduleBlocks;

void setScheduleEnabled(bool enabled) {
  scheduleEnabled = enabled;
}

bool isScheduleEnabled() {
  return scheduleEnabled;
}

bool isWeekend() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  int day = timeinfo->tm_wday;
  return (day == 0 || day == 6); // Sunday or Saturday
}

String getRelayStateFromSchedule(const String& path) {
  // Placeholder logic: always ON if schedule is enabled
  return scheduleEnabled ? "ON" : "OFF";
}

bool updateScheduleFromJson(const String& json) {
  StaticJsonDocument<2048> doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    Serial.println("JSON parse failed: " + String(err.c_str()));
    return false;
  }

  if (!doc.is<JsonArray>()) {
    Serial.println("Expected JSON array");
    return false;
  }

  scheduleBlocks.clear();

  for (JsonObject schedule : doc.as<JsonArray>()) {
    const char* name = schedule["name"];
    const char* startDate = schedule["startDate"];
    const char* endDate = schedule["endDate"];
    JsonArray sections = schedule["sections"];

    if (!sections) {
      Serial.println("Invalid schedule: missing sections");
      continue;
    }

    for (JsonObject section : sections) {
      ScheduleBlock block;
      block.start = section["start"] | "";
      block.end = section["end"] | "";
      block.enabled = true; // default to enabled

      if (block.start == "" || block.end == "") {
        Serial.println("Invalid section: missing start/end");
        continue;
      }

      scheduleBlocks.push_back(block);
    }
  }

  Serial.printf("Loaded %d schedule blocks\n", scheduleBlocks.size());
  return true;
}

String getScheduleAsJson() {
  StaticJsonDocument<1024> doc;
  JsonArray arr = doc.to<JsonArray>();

  for (const auto& block : scheduleBlocks) {
    JsonObject obj = arr.createNestedObject();
    obj["start"] = block.start;
    obj["end"] = block.end;
    obj["enabled"] = block.enabled;
  }

  String output;
  serializeJson(doc, output);
  return output;
}
