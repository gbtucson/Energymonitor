#pragma once
#include "refoss_monitor.h"

extern bool scheduleEnabled;

void initEVController();
bool evaluateChargingLogic(const EnergyData& data, bool isPeak);
int getChargeRateWatts(const EnergyData& data, bool isPeak);
void startChargingTimer(unsigned long durationMs);
void stopChargingTimer();
