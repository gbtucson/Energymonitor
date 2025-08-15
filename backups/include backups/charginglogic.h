#pragma once

#include <Arduino.h>

// Timer-based charging state
extern bool chargingTimerActive;
extern unsigned long chargingStartTime;
extern unsigned long chargingDurationMs;
extern int chargingRate;

// Persistent config
extern int minimumPVWattsToCharge;

// Inline accessors for compatibility
inline bool isChargingTimerActive() { return chargingTimerActive; }
inline unsigned long getChargingStartTime() { return chargingStartTime; }
inline unsigned long getChargingDurationMs() { return chargingDurationMs; }
inline int getChargingRate() { return chargingRate; }
inline int getMinimumPVWattsToCharge() { return minimumPVWattsToCharge; }

// Core logic
void updateChargingLoop();
String getChargingStatusJson();
bool updateChargingSettings(const String& json);
void loadChargingSettings();

// Stubbed control functions (can be moved to chargingcontrol.h if modularized)
void setChargingCurrent(int amps);
void stopCharging();
