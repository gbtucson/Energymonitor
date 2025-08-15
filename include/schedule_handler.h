#ifndef SCHEDULE_HANDLER_H
#define SCHEDULE_HANDLER_H

#include <Arduino.h>
#include <vector>

// Represents a single schedule block
struct ScheduleBlock {
  String start;   // Format: "HH:MM"
  String end;     // Format: "HH:MM"
  bool enabled;
};

// Global schedule state
extern bool scheduleEnabled;
extern std::vector<ScheduleBlock> scheduleBlocks;

// Core control
void setScheduleEnabled(bool enabled);
bool isScheduleEnabled();
bool isWeekend();

// Relay logic
String getRelayStateFromSchedule(const String& path);

// Schedule management
bool updateScheduleFromJson(const String& json);
String getScheduleAsJson();

#endif // SCHEDULE_HANDLER_H