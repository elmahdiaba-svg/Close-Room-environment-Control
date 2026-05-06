#include "settings.h"
#include <Arduino.h>
#include "config/config.h"

const char SETTINGS_HTML[] PROGMEM = R"=====(
  <!-- SETTINGS PAGE -->
  <div class="page" id="settings">
    <div class="card">
      <h3>Control</h3>
      <div class="field">
        <label>Delta / Hysteresis (&#176;C)</label>
        <input type="number" id="delta" step="0.5" value="2">
      </div>
    </div>
    <div class="card">
      <h3>Power</h3>
      <div class="field">
        <label>Peltier Power (0&#8211;255)</label>
        <div class="slider-row">
          <input type="range" id="peltierPWM" min="0" max="255" value="180"
                 oninput="document.getElementById('pv').innerText=this.value">
          <span class="rv" id="pv">180</span>
        </div>
      </div>
      <div class="field">
        <label>Fan Speed (0&#8211;255)</label>
        <div class="slider-row">
          <input type="range" id="fanPWM" min="0" max="255" value="255"
                 oninput="document.getElementById('fv').innerText=this.value">
          <span class="rv" id="fv">255</span>
        </div>
      </div>
    </div>
    <button class="apply-btn" onclick="applySettings()">Apply Settings</button>
  </div>

  <script>
    function applySettings(){
      const p = new URLSearchParams({
        delta:      document.getElementById('delta').value,
        peltierPWM: document.getElementById('peltierPWM').value,
        fanPWM:     document.getElementById('fanPWM').value
      });
      fetch('/set?'+p.toString()).then(r=>r.json()).then(()=>{
        const t = document.getElementById('toast');
        t.classList.add('show');
        setTimeout(()=>t.classList.remove('show'), 2200);
      });
    }
  </script>
)=====";

void handleSet() {
  if (server.hasArg("sp"))         SP         = server.arg("sp").toFloat();
  if (server.hasArg("delta"))      DELTA      = server.arg("delta").toFloat();
  if (server.hasArg("peltierPWM")) peltierPWM = server.arg("peltierPWM").toInt();
  if (server.hasArg("fanPWM"))     fanPower   = server.arg("fanPWM").toInt();

  if (currentMode != IDLE) {
    ledcWrite(mosfetPin, peltierPWM);
    ledcWrite(fan1Pin,   fanPower);
    ledcWrite(fan2Pin,   fanPower);
    Serial.println("[WEB] PWM updated live.");
  }

  Serial.printf("[WEB] SP=%.1f DELTA=%.1f PeltierPWM=%d FanPWM=%d\n",
                SP, DELTA, peltierPWM, fanPower);

  String json = "{\"sp\":"         + String(SP, 2)      +
                ",\"delta\":"      + String(DELTA, 2)    +
                ",\"peltierPWM\":" + String(peltierPWM)  +
                ",\"fanPWM\":"     + String(fanPower)     + "}";
  server.send(200, "application/json", json);
}