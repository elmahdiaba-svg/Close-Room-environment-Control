#pragma once

void startModeChange(bool heating, int pwmValue, int fanValue);
void stopAll();
void handlePendingRelay();