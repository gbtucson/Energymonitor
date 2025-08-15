#include "refoss_monitor.h"

void initRefossMonitors() {
  // Initialize Refoss devices
}

EnergyData getLatestEnergyData() {
  EnergyData data;
  data.voltage = 120.0;
  data.current = 10.0;
  data.power = data.voltage * data.current;
  return data;
}