#pragma once

struct EnergyData {
  int pvWatts;
  int homeWatts;
  int evWatts;
};

inline EnergyData getLatestEnergyData() {
  return { 3200, 2800, 400 };
}

inline bool evaluateChargingLogic(const EnergyData& data, bool isPeak) {
  return data.pvWatts > data.evWatts && !isPeak;
}

inline int getChargeRateWatts(const EnergyData& data, bool isPeak) {
  return isPeak ? 1200 : 2400;
}
