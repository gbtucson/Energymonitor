#include "relaycontrol.h"

bool relayState = false;
bool manualRelayOverride = false;
bool manualRelayState = false;

bool manualOverrideActive() { return manualRelayOverride; }
bool getManualOverrideState() { return manualRelayState; }
bool getRelayState() { return relayState; }

void setManualOverride(bool active, bool state) {
  manualRelayOverride = active;
  manualRelayState = state;
}

void clearManualOverride() {
  manualRelayOverride = false;
}

void setRelayState(bool state) {
  relayState = state;
}
