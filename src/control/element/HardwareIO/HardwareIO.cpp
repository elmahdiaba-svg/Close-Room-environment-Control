#include "HardwareIO.h"
#include "config/config.h"

// -----------------------------------------------------------------------
// initHardwareIO — call once in setup()
// -----------------------------------------------------------------------
void initHardwareIO() {
    // Status LEDs (active HIGH)
    pinMode(LED_HEATING_PIN, OUTPUT); digitalWrite(LED_HEATING_PIN, LOW);
    pinMode(LED_COOLING_PIN, OUTPUT); digitalWrite(LED_COOLING_PIN, LOW);
    pinMode(LED_FREE_PIN,    OUTPUT); digitalWrite(LED_FREE_PIN,    LOW);
    pinMode(LED_READY_PIN,   OUTPUT); digitalWrite(LED_READY_PIN,   HIGH); // server alive from boot

    // NOT-AUS relay: LOW = relay coil energised = NC contacts open = power cut
    // Connect your safety relay NC contacts in series with the main power line.
    pinMode(NotAusRelay_PIN, OUTPUT); digitalWrite(NotAusRelay_PIN, HIGH); // normal = relay off = power on

    // NOT-AUS button: active LOW, internal pull-up
    pinMode(NotAusButton_PIN, INPUT_PULLUP);

    Serial.println("[HW] Hardware I/O ready.");
}

// -----------------------------------------------------------------------
// updateStatusLEDs — call every loop; mirrors mode + emergency state
// -----------------------------------------------------------------------
void updateStatusLEDs() {
    digitalWrite(LED_HEATING_PIN, currentMode == HEATING ? HIGH : LOW);
    digitalWrite(LED_COOLING_PIN, currentMode == COOLING ? HIGH : LOW);
    digitalWrite(LED_FREE_PIN,
        (currentMode == FREE_COOLING || currentMode == FREE_HEATING) ? HIGH : LOW);

    // Server-ready LED: solid ON normally, OFF when emergency stop active
    digitalWrite(LED_READY_PIN, emergencyStop ? LOW : HIGH);

    // NOT-AUS relay: energise (LOW) when emergency stop active → cuts NC contacts
    digitalWrite(NotAusRelay_PIN, emergencyStop ? LOW : HIGH);
}

// -----------------------------------------------------------------------
// readNotAusButton — debounced; toggles emergencyStop on falling edge
// -----------------------------------------------------------------------
void readNotAusButton() {
    static bool          lastRaw       = HIGH;
    static bool          confirmed     = HIGH;
    static unsigned long debounceStart = 0;

    bool raw = digitalRead(NotAusButton_PIN);

    if (raw != lastRaw) {
        debounceStart = millis();   // restart debounce timer on any change
    }
    lastRaw = raw;

    if (millis() - debounceStart >= 50 && raw != confirmed) {
        confirmed = raw;

        if (confirmed == LOW) {              // falling edge = button pressed
            emergencyStop = !emergencyStop;

            if (emergencyStop) {
                // cut all outputs immediately
                ledcWrite(MosfetPetelierCh, 0);
                ledcWrite(MosfetFan1Ch,     0);
                ledcWrite(MosfetFan2Ch,     0);
                digitalWrite(HeatingRelay, HIGH);
                digitalWrite(CoolingRelay, HIGH);
                currentMode = IDLE;
                Serial.println("[NOT-AUS] Hardware button pressed — emergency stop ACTIVE.");
            } else {
                Serial.println("[NOT-AUS] Hardware button pressed — emergency stop CLEARED.");
            }
        }
    }
}
