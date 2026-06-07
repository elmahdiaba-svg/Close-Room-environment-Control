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
      <h3>Air Quality Hysteresis</h3>
      <div class="field">
        <label>Hysteresis (raw MQ135 units below limit)</label>
        <input type="number" id="aqHyst" step="50" min="0" max="4095" value="300">
      </div>
      <div style="font-size:.65em;color:#555;letter-spacing:.05em;line-height:1.5">
        Flap + fan switch ON above the limit, and OFF again once the reading
        drops below <em>limit &minus; hysteresis</em>.
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

    <div class="card">
      <h3>Power Presets</h3>
      <div style="font-size:.65em;color:#555;letter-spacing:.05em;line-height:1.5;margin-bottom:14px">
        Tap a preset to instantly apply its Peltier &amp; fan power. Values are
        sent as exact PWM levels — the sliders above update to match.
      </div>
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
  </div>

  <script>
    function applyPreset(pPWM, fPWM) {
      const pPct = Math.round(pPWM / 2.55);
      const fPct = Math.round(fPWM / 2.55);
      const pSlider = document.getElementById('peltierPower');
      const fSlider = document.getElementById('fanPWM');
      if (pSlider) pSlider.value = pPct;
      if (fSlider) fSlider.value = fPct;
      const pv = document.getElementById('pv');
      const fv = document.getElementById('fv');
      if (pv) pv.innerText = pPct + '%';
      if (fv) fv.innerText = fPct + '%';

      fetch('/set?peltierPower='+pPWM+'&fanPWM='+fPWM).then(r=>r.json()).then(()=>{
        const t = document.getElementById('toast');
        t.classList.add('show');
        setTimeout(()=>t.classList.remove('show'), 2200);
      });
    }

    function applySettings(){
      const pPct = parseInt(document.getElementById('peltierPower').value);
      const fPct = parseInt(document.getElementById('fanPWM').value);
      const p = new URLSearchParams({
        delta:        document.getElementById('delta').value,
        aqHyst:       document.getElementById('aqHyst').value,
        peltierPower: Math.round(pPct * 2.55),
        fanPWM:       Math.round(fPct * 2.55)
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
  if (server.hasArg("sp"))
    TemperatureSp = constrain(server.arg("sp").toFloat(), TemperatureSp_MIN, TemperatureSp_MAX);
  if (server.hasArg("delta"))       TemperatureHysteresis = server.arg("delta").toFloat();
  if (server.hasArg("peltierPower")) peltierPower     = server.arg("peltierPower").toInt();
  if (server.hasArg("fanPWM"))      fanPower          = server.arg("fanPWM").toInt();
  if (server.hasArg("aqLimit"))
    airQualitySp = constrain(server.arg("aqLimit").toInt(), AirqualitySp_MIN, AirqualitySp_MAX);
  if (server.hasArg("aqHyst"))      airQualityHysteresis = server.arg("aqHyst").toInt();
  if (server.hasArg("humLimit"))
    humiditySp = constrain(server.arg("humLimit").toInt(), HumiditySp_MIN, HumiditySp_MAX);

  if (currentMode == HEATING || currentMode == COOLING) {
    ledcWrite(MosfetPetelierCh, peltierPower);
    ledcWrite(MosfetFan1Ch,     fanPower);
    ledcWrite(MosfetFan2Ch,     fanPower);
    Serial.println("[WEB] PWM updated live.");
  }

  Serial.printf("[WEB] SP=%.1f DELTA=%.1f peltierPower=%d FanPWM=%d aqLimit=%d aqHyst=%d\n",
                TemperatureSp, TemperatureHysteresis, peltierPower, fanPower, airQualitySp, airQualityHysteresis);

  String json = "{\"sp\":"         + String(TemperatureSp, 2)      +
                ",\"delta\":"      + String(TemperatureHysteresis, 2)    +
                ",\"aqHyst\":"     + String(airQualityHysteresis) +
                ",\"peltierPower\":" + String(peltierPower)  +
                ",\"fanPWM\":"     + String(fanPower)     + "}";
  server.send(200, "application/json", json);
}
