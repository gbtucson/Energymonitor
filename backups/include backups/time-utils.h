#pragma once
#include <Arduino.h>
#include <time.h>

// Returns current time in minutes from midnight (0–1439)
int getCurrentMinutesFromMidnight();

// Returns local time info (year, month, day, hour, min, etc.)
tm getCurrentLocalTime();

// Pads an integer with two digits ("08" not "8")
String twoDigits(int val);

// Updates global sunsetMinutes based on today’s date
void updateSunsetToday();
