#pragma once

extern bool relayState;
extern bool manualRelayOverride;
extern bool manualRelayState;

bool manualOverrideActive();
bool getManualOverrideState();
bool getRelayState();

void setManualOverride(bool active, bool state);
void clearManualOverride();
void setRelayState(bool state);