#include "chargingcontrol.h"
#include <Arduino.h>

void setChargingCurrent(int amps) {
  // TODO: Implement actual current control logic
  Serial.printf("Charging at %d A\n", amps);
}

void stopCharging() {
  // TODO: Implement actual stop logic
  Serial.println("Charging stopped");
}
