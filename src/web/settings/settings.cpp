#include "settings.h"
#include <Arduino.h>
#include "config/config.h"

const char SETTINGS_HTML[] PROGMEM = R"=====(
  <!-- SETTINGS PAGE -->
  <div class="page" id="settings">
    <div class="card">
      <h3>Inside Temperature Control Hysteresis</h3>
      <div class="field">
        <label>Delta / Hysteresis (&#176;C)</label>
        <input type="number" id="delta" step="0.5" value="2">
      </div>
    </div>
    <div class="card">
      <h3>Presets</h3>
      <div class="preset-grid">
        <button class="preset-btn" onclick="applyPreset(50,120)">
          <span class="preset-name">Economic</span>
          <span class="preset-info">10.2 W</span>
          <span class="preset-pwm">P:20% &nbsp; F:47%</span>
        </button>
        <button class="preset-btn" onclick="applyPreset(100,180)">
          <span class="preset-name">Medium</span>
          <span class="preset-info">17.1 W</span>
          <span class="preset-pwm">P:39% &nbsp; F:71%</span>
        </button>
        <button class="preset-btn" onclick="applyPreset(150,255)">
          <span class="preset-name">Balanced</span>
          <span class="preset-info">31.2 W</span>
          <span class="preset-pwm">P:59% &nbsp; F:100%</span>
        </button>
        <button class="preset-btn" onclick="applyPreset(255,255)">
          <span class="preset-name">Full</span>
          <span class="preset-info">40.1 W</span>
          <span class="preset-pwm">P:100% &nbsp; F:100%</span>
        </button>
      </div>
    </div>
    <div class="card">
      <h3>Power</h3>
      <div class="field">
        <label>Peltier Power (0&#8211;100 %)</label>
        <div class="slider-row">
          <input type="range" id="peltierPower" min="0" max="100" value="71"
                 oninput="document.getElementById('pv').innerText=this.value+'%'">
          <span class="rv" id="pv">71%</span>
        </div>
      </div>
      <div class="field">
        <label>Fan Speed (0&#8211;100 %)</label>
        <div class="slider-row">
          <input type="range" id="fanPWM" min="0" max="100" value="100"
                 oninput="document.getElementById('fv').innerText=this.value+'%'">
          <span class="rv" id="fv">100%</span>
        </div>
      </div>
    </div>
    <button class="apply-btn" onclick="applySettings()">Apply Settings</button>
  </div>

  <script>
    function applySettings(){
      const pPct = parseInt(document.getElementById('peltierPower').value);
      const fPct = parseInt(document.getElementById('fanPWM').value);
      const p = new URLSearchParams({
        delta:        document.getElementById('delta').value,
        peltierPower: Math.round(pPct * 2.55),
        fanPWM:       Math.round(fPct * 2.55)
      });
      fetch('/set?'+p.toString()).then(r=>r.json()).then(()=>{
        const t = document.getElementById('toast');
        t.classList.add('show');
        setTimeout(()=>t.classList.remove('show'), 2200);
      });
    }

    function applyPreset(pPWM, fPWM) {
      const pPct = Math.round(pPWM / 2.55);
      const fPct = Math.round(fPWM / 2.55);
      document.getElementById('peltierPower').value = pPct;
      document.getElementById('fanPWM').value       = fPct;
      document.getElementById('pv').innerText       = pPct + '%';
      document.getElementById('fv').innerText       = fPct + '%';
      fetch('/set?peltierPower='+pPWM+'&fanPWM='+fPWM).then(r=>r.json()).then(()=>{
        const t = document.getElementById('toast');
        t.classList.add('show');
        setTimeout(()=>t.classList.remove('show'), 2200);
      });
    }
  </script>
)=====";

void handleSet() {
  if (server.hasArg("sp"))          SP                = server.arg("sp").toFloat();
  if (server.hasArg("delta"))       DELTA             = server.arg("delta").toFloat();
  if (server.hasArg("peltierPower")) peltierPower     = server.arg("peltierPower").toInt();
  if (server.hasArg("fanPWM"))      fanPower          = server.arg("fanPWM").toInt();
  if (server.hasArg("aqLimit"))     airQualityLimits  = server.arg("aqLimit").toInt();
  if (server.hasArg("humLimit"))    humidityLimit     = server.arg("humLimit").toInt();

  if (currentMode == HEATING || currentMode == COOLING) {
    ledcWrite(MosfetPetelierCh, peltierPower);
    ledcWrite(MosfetFan1Ch,     fanPower);
    ledcWrite(MosfetFan2Ch,     fanPower);
    Serial.println("[WEB] PWM updated live.");
  }

  Serial.printf("[WEB] SP=%.1f DELTA=%.1f peltierPower=%d FanPWM=%d aqLimit=%d\n",
                SP, DELTA, peltierPower, fanPower, airQualityLimits);

  String json = "{\"sp\":"         + String(SP, 2)      +
                ",\"delta\":"      + String(DELTA, 2)    +
                ",\"peltierPower\":" + String(peltierPower)  +
                ",\"fanPWM\":"     + String(fanPower)     + "}";
  server.send(200, "application/json", json);
}