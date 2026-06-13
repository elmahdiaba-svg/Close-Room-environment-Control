#include "dashboard.h"
#include <Arduino.h>
#include "config/config.h"

const char DASHBOARD_HTML[] PROGMEM = R"=====(
  <!-- DASHBOARD PAGE -->
  <div class="page active" id="dash">
    <!-- Sensor error banner -->
    <div class="sensor-alert" id="sensorAlert">
      <span>&#9888;&nbsp;</span><span id="sensorAlertText">Sensor error</span>
    </div>

    <div class="mode-box IDLE" id="modeBox">IDLE</div>

    <!-- NOT-AUS -->
    <button id="notausBtn" class="notaus-btn" onclick="toggleNotaus()">&#9888; NOT-AUS</button>

    <div class="metrics">
      <div class="metric">
        <div class="lbl">Inside Temp</div>
        <div class="val hi" id="tempVal">--</div>
      </div>
      <div class="metric">
        <div class="lbl">Outside Temp</div>
        <div class="val" id="outsideTempVal">--</div>
      </div>
      <div class="metric">
        <div class="lbl">Pressure</div>
        <div class="val" id="pressVal">--</div>
      </div>
      <div class="metric">
        <div class="lbl">Current</div>
        <div class="val" id="currentVal">--</div>
      </div>
      <div class="metric">
        <div class="lbl">Supply Power</div>
        <div class="val" id="powerVal">--</div>
      </div>
    </div>

    <!-- Temperature setpoint -->
    <div class="sp-card">
      <div class="lbl">Temperature Setpoint</div>
      <div style="display:flex;align-items:baseline;gap:10px;margin:10px 0 6px">
        <span style="font-size:1.6em;font-weight:700" id="tempValCard">--</span>
        <span style="font-size:.75em;color:#555">°C inside</span>
        <span style="margin-left:auto;font-size:.8em;letter-spacing:.08em" id="tempCardStatus">--</span>
      </div>
      <div class="sp-row">
        <input type="number" id="spInput" step="0.5" min="10" max="35" value="22" onkeydown="if(event.key==='Enter')sendSp()">
        <button class="apply-mini" onclick="sendSp()">&#10003;</button>
        <span class="sp-unit">&#176;C</span>
      </div>
      <div class="sp-hint">allowed range: 10&nbsp;&ndash;&nbsp;35 &#176;C</div>
      <div class="sp-status" id="spStatus">&nbsp;</div>
    </div>

    <!-- Air Quality -->
    <div class="sp-card">
      <div class="lbl">Air Quality </div>
      <div style="display:flex;align-items:baseline;gap:10px;margin:10px 0 6px">
        <span style="font-size:1.6em;font-weight:700" id="aqVal">--</span>
        <span style="font-size:.75em;color:#555">raw</span>
        <span style="margin-left:auto;font-size:.8em;letter-spacing:.08em" id="aqStatus">--</span>
      </div>
      <div style="background:#1a1a1a;border-radius:2px;height:6px;overflow:hidden">
        <div id="aqBar" style="height:100%;border-radius:2px;width:0%;transition:width .5s,background .5s"></div>
      </div>
      <div style="display:flex;justify-content:space-between;font-size:.65em;color:#555;margin-top:3px">
        <span>0</span><span id="aqLimitLabel">limit: --</span><span>4095</span>
      </div>
      <div class="lbl" style="margin-top:12px">Limit (airQualitySp)</div>
      <div class="sp-row">
        <input type="number" id="aqLimitInput" step="100" min="500" max="4000" value="2500" onkeydown="if(event.key==='Enter')sendAqLimit()">
        <button class="apply-mini" onclick="sendAqLimit()">&#10003;</button>
        <span class="sp-unit">raw</span>
      </div>
      <div class="sp-hint">allowed range: 500&nbsp;&ndash;&nbsp;4000 raw</div>
      <div class="sp-status" id="aqLimitStatus">&nbsp;</div>
    </div>

    <!-- Humidity setpoint -->
    <div class="sp-card">
      <div class="lbl">Humidity </div>
      <div style="display:flex;align-items:baseline;gap:10px;margin:10px 0 6px">
        <span style="font-size:1.6em;font-weight:700" id="humValLarge">--</span>
        <span style="font-size:.75em;color:#555">% RH</span>
        <span style="margin-left:auto;font-size:.8em;letter-spacing:.08em" id="humStatus">--</span>
      </div>
      <div style="background:#1a1a1a;border-radius:2px;height:6px;overflow:hidden">
        <div id="humBar" style="height:100%;border-radius:2px;width:0%;transition:width .5s,background .5s"></div>
      </div>
      <div style="display:flex;justify-content:space-between;font-size:.65em;color:#555;margin-top:3px">
        <span>0%</span><span id="humLimitLabel">limit: --</span><span>100%</span>
      </div>
      <div class="lbl" style="margin-top:12px">Limit (humiditySp)</div>
      <div class="sp-row">
        <input type="number" id="humLimitInput" step="5" min="20" max="90" value="65" onkeydown="if(event.key==='Enter')sendHumLimit()">
        <button class="apply-mini" onclick="sendHumLimit()">&#10003;</button>
        <span class="sp-unit">% RH</span>
      </div>
      <div class="sp-hint">allowed range: 20&nbsp;&ndash;&nbsp;90 % RH</div>
      <div class="sp-status" id="humLimitStatus">&nbsp;</div>
    </div>
  </div>

  <script>
    var settingsSynced = false;

    function toggleNotaus() {
      fetch('/notaus').then(r=>r.json()).then(d=>{
        applyNotausState(d.emergency);
      });
    }

    function applyNotausState(active) {
      const btn = document.getElementById('notausBtn');
      if (active) {
        btn.classList.add('active');
        btn.innerText = '⚠ NOT-AUS ACTIVE — tap to resume';
      } else {
        btn.classList.remove('active');
        btn.innerText = '⚠ NOT-AUS';
      }
    }

    var modeLabel = {
      IDLE:     'IDLE',
      HEATING:  'HEATING',
      COOLING:  'COOLING',
      FREECOOL: 'FREE COOL',
      FREEHEAT: 'FREE HEAT'
    };

    function showPage(id, btn){
      document.querySelectorAll('.page').forEach(p=>p.classList.remove('active'));
      document.querySelectorAll('nav button').forEach(b=>b.classList.remove('active'));
      document.getElementById(id).classList.add('active');
      btn.classList.add('active');
    }

    // ---- Settings login gate ----
    let settingsUnlocked  = false;
    let pendingSettingsBtn = null;

    // Single nav button: shows "Login" while locked, "Settings" once unlocked
    function handleAuthNav(btn){
      if (settingsUnlocked) { showPage('settings', btn); return; }
      openLogin(btn);
    }

    function openLogin(btn){
      pendingSettingsBtn = btn;
      const pass = document.getElementById('loginPass');
      pass.value = '';
      document.getElementById('loginError').innerHTML = '&nbsp;';
      document.getElementById('loginOverlay').classList.add('show');
      pass.focus();
    }

    function closeLogin(){
      document.getElementById('loginOverlay').classList.remove('show');
      pendingSettingsBtn = null;
    }

    function submitLogin(){
      const pass = document.getElementById('loginPass').value;
      fetch('/login?pass='+encodeURIComponent(pass)).then(r=>r.json()).then(d=>{
        if (d.ok) {
          settingsUnlocked = true;
          const navBtn = document.getElementById('navAuthBtn');
          navBtn.innerHTML = '&#9881; Settings';
          document.getElementById('loginOverlay').classList.remove('show');
          showPage('settings', pendingSettingsBtn || navBtn);
          pendingSettingsBtn = null;
        } else {
          document.getElementById('loginError').innerText = 'Wrong password';
        }
      }).catch(()=>{
        document.getElementById('loginError').innerText = 'Connection error';
      });
    }

    function clampInput(el, min, max){
      let v = parseFloat(el.value);
      if (isNaN(v)) return el.value;
      v = Math.min(max, Math.max(min, v));
      el.value = v;
      return v;
    }

    function sendSp(){
      const st = document.getElementById('spStatus');
      const inp = document.getElementById('spInput');
      clampInput(inp, 10, 35);
      st.className = 'sp-status sending'; st.innerText = 'sending...';
      fetch('/set?sp='+inp.value).then(r=>r.json()).then(()=>{
        st.className = 'sp-status ok'; st.innerText = '✓ applied';
        setTimeout(()=>{ st.className='sp-status'; st.innerText='press Enter to apply'; }, 1800);
      }).catch(()=>{ st.className='sp-status'; st.innerText='error'; });
    }

    function sendAqLimit(){
      const st = document.getElementById('aqLimitStatus');
      const inp = document.getElementById('aqLimitInput');
      clampInput(inp, 500, 4000);
      st.className = 'sp-status sending'; st.innerText = 'sending...';
      fetch('/set?aqLimit='+inp.value).then(r=>r.json()).then(()=>{
        st.className = 'sp-status ok'; st.innerText = '✓ applied';
        setTimeout(()=>{ st.className='sp-status'; st.innerText='press Enter to apply'; }, 1800);
      }).catch(()=>{ st.className='sp-status'; st.innerText='error'; });
    }

    function sendHumLimit(){
      const st = document.getElementById('humLimitStatus');
      const inp = document.getElementById('humLimitInput');
      clampInput(inp, 20, 90);
      st.className = 'sp-status sending'; st.innerText = 'sending...';
      fetch('/set?humLimit='+inp.value).then(r=>r.json()).then(()=>{
        st.className = 'sp-status ok'; st.innerText = '✓ applied';
        setTimeout(()=>{ st.className='sp-status'; st.innerText='press Enter to apply'; }, 1800);
      }).catch(()=>{ st.className='sp-status'; st.innerText='error'; });
    }

    setInterval(()=>{
      fetch('/status').then(r=>r.json()).then(d=>{
        document.getElementById('tempVal').innerText        = d.temp.toFixed(1)+' °C';
        document.getElementById('pressVal').innerText       = d.pressure.toFixed(1)+' hPa';
        document.getElementById('currentVal').innerText     = d.current.toFixed(2)+' A';
        document.getElementById('powerVal').innerText       = (d.current * 12).toFixed(1)+' W';
        document.getElementById('outsideTempVal').innerText = d.outsideOK ? d.outsideTemp.toFixed(1)+' °C' : 'N/A';

        // Sensor health banner — collect any sensor reporting an error
        const sensorErrors = [];
        if (!d.insideOK)  sensorErrors.push('Inside Temp/Pressure (BMP180)');
        if (!d.outsideOK) sensorErrors.push('Outside Temp/Pressure (BMP180)');
        if (!d.aqOK)      sensorErrors.push('Air Quality (MQ135)');
        if (!d.humOK)     sensorErrors.push('Humidity (DHT11)');
        const alertBox = document.getElementById('sensorAlert');
        if (sensorErrors.length) {
          document.getElementById('sensorAlertText').innerText =
            'Sensor disconnected — check: ' + sensorErrors.join(', ');
          alertBox.classList.add('show');
        } else {
          alertBox.classList.remove('show');
        }

        // Inside temperature card
        const temp = d.temp, sp = d.sp, delta = d.delta;
        const tCard = document.getElementById('tempValCard');
        tCard.innerText = temp.toFixed(1);
        const tst = document.getElementById('tempCardStatus');
        if (temp >= sp + delta) {
          tCard.style.color = '#ff4444';
          tst.style.color = '#ff4444'; tst.innerText = '▲ TOO HOT';
        } else if (temp <= sp - delta) {
          tCard.style.color = '#00e5ff';
          tst.style.color = '#00e5ff'; tst.innerText = '▼ TOO COLD';
        } else {
          tCard.style.color = '#00ff88';
          tst.style.color = '#00ff88'; tst.innerText = '✓ IN RANGE';
        }

        // Humidity card
        const hum = d.humidity, hlim = d.humLimit;
        document.getElementById('humValLarge').innerText   = hum.toFixed(1);
        document.getElementById('humLimitLabel').innerText = 'limit: ' + hlim + '%';
        const hpct = Math.min(100, hum).toFixed(1);
        const hbar = document.getElementById('humBar');
        hbar.style.width = hpct + '%';
        const hst = document.getElementById('humStatus');
        if (hum > hlim) {
          hbar.style.background = '#ff4444';
          document.getElementById('humValLarge').style.color = '#ff4444';
          hst.style.color = '#ff4444'; hst.innerText = '⚠ HIGH';
        } else if (hum > hlim - 5) {
          hbar.style.background = '#ffaa00';
          document.getElementById('humValLarge').style.color = '#ffaa00';
          hst.style.color = '#ffaa00'; hst.innerText = '~ WARNING';
        } else {
          hbar.style.background = '#00e5ff';
          document.getElementById('humValLarge').style.color = '#e0e0e0';
          hst.style.color = '#00ff88'; hst.innerText = '✓ OK';
        }

        // Air quality card
        const mq = d.mq135, lim = d.aqLimit, aqHyst = d.aqHyst;
        document.getElementById('aqVal').innerText        = mq;
        document.getElementById('aqLimitLabel').innerText = 'limit: ' + lim;
        const pct = Math.min(100, (mq / 4095) * 100).toFixed(1);
        const bar = document.getElementById('aqBar');
        bar.style.width = pct + '%';
        const aqSt = document.getElementById('aqStatus');
        if (mq > lim) {
          bar.style.background = '#ff4444';
          document.getElementById('aqVal').style.color = '#ff4444';
          aqSt.style.color = '#ff4444'; aqSt.innerText = '⚠ BAD';
        } else if (mq > lim - aqHyst) {
          bar.style.background = '#ffaa00';
          document.getElementById('aqVal').style.color = '#ffaa00';
          aqSt.style.color = '#ffaa00'; aqSt.innerText = '~ WARNING';
        } else {
          bar.style.background = '#00e5ff';
          document.getElementById('aqVal').style.color = '#e0e0e0';
          aqSt.style.color = '#00ff88'; aqSt.innerText = '✓ GOOD';
        }

        const b = document.getElementById('modeBox');
        b.innerText = modeLabel[d.mode] || d.mode;
        b.className = 'mode-box ' + d.mode;

        // NOT-AUS button — sync every poll so it never goes stale
        // (state can change via hardware button or another browser/tab)
        applyNotausState(d.emergency);

        if (!settingsSynced) {
          settingsSynced = true;
          document.getElementById('spInput').value       = d.sp.toFixed(1);
          document.getElementById('aqLimitInput').value  = d.aqLimit;
          document.getElementById('humLimitInput').value = d.humLimit;
          document.getElementById('delta').value         = d.delta.toFixed(1);
          const aqHystEl = document.getElementById('aqHyst');
          if (aqHystEl) aqHystEl.value = d.aqHyst;
          const pPct = Math.round(d.peltierPower / 2.55);
          const fPct = Math.round(d.fanPWM / 2.55);
          document.getElementById('peltierPower').value  = pPct;
          document.getElementById('fanPWM').value        = fPct;
          document.getElementById('pv').innerText        = pPct + '%';
          document.getElementById('fv').innerText        = fPct + '%';
        }
      }).catch(()=>{});
    }, 1000);
  </script>
)=====";

void handleStatus() {
  lastUserAccess = millis();   // Dashboard-Zugriff erkannt -> ACTIVE MODE bleibt aktiv; FE 

  const char* modeStr = currentMode == HEATING      ? "HEATING"
                      : currentMode == COOLING      ? "COOLING"
                      : currentMode == FREE_COOLING ? "FREECOOL"
                      : currentMode == FREE_HEATING ? "FREEHEAT"
                      : "IDLE";

  String json = "{\"temp\":"         + String(insideTemp, 2) +
                ",\"pressure\":"     + String(insidePressure, 2) +
                ",\"insideOK\":"     + String(insideTemperatureSensorOK ? "true" : "false") +
                ",\"current\":"      + String(currentAmps, 2) +
                ",\"outsideTemp\":"  + String(outsideTemp, 2) +
                ",\"outsideOK\":"    + String(outsideTemperatureSensorOK ? "true" : "false") +
                ",\"mq135\":"        + String(mq135Raw) +
                ",\"aqOK\":"         + String(airQualitySensorOK ? "true" : "false") +
                ",\"humOK\":"        + String(humiditySensorOK ? "true" : "false") +
                ",\"aqLimit\":"      + String(airQualitySp) +
                ",\"aqHyst\":"       + String(airQualityHysteresis) +
                ",\"humidity\":"     + String(currentHumidity, 1) +
                ",\"humLimit\":"     + String(humiditySp) +
                ",\"sp\":"           + String(TemperatureSp, 2) +
                ",\"delta\":"        + String(TemperatureHysteresis, 2) +
                ",\"peltierPower\":" + String(peltierPower) +
                ",\"fanPWM\":"       + String(fanPower) +
                ",\"mode\":\""       + modeStr + "\"" +
                ",\"emergency\":"    + String(emergencyStop ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}
