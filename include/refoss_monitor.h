#pragma once

struct EnergyData {
  float voltage;
  float current;
  float power;
};

void initRefossMonitors();
EnergyData getLatestEnergyData();
