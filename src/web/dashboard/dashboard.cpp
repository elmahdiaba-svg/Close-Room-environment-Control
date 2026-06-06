#include "dashboard.h"
#include <Arduino.h>
#include "config/config.h"

const char DASHBOARD_HTML[] PROGMEM = R"=====(
  <!-- DASHBOARD PAGE -->
  <div class="page active" id="dash">
    <div class="mode-box IDLE" id="modeBox">IDLE</div>
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
        <div class="lbl">Humidity</div>
        <div class="val" id="humVal">--</div>
      </div>
    </div>

    <!-- Temperature setpoint -->
    <div class="sp-card">
      <div class="lbl">Setpoint</div>
      <div class="sp-row">
        <input type="number" id="spInput" step="0.5" value="22" oninput="onSpChange()">
        <span class="sp-unit">&#176;C</span>
      </div>
      <div class="sp-status" id="spStatus">&nbsp;</div>
    </div>

    <!-- Air Quality -->
    <div class="sp-card">
      <div class="lbl">Air Quality — MQ135</div>
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
      <div class="lbl" style="margin-top:12px">Limit (airQualityLimits)</div>
      <div class="sp-row">
        <input type="number" id="aqLimitInput" step="100" min="0" max="4095" value="2500" oninput="onAqLimitChange()">
        <span class="sp-unit">raw</span>
      </div>
      <div class="sp-status" id="aqLimitStatus">&nbsp;</div>
    </div>

    <!-- Humidity setpoint -->
    <div class="sp-card">
      <div class="lbl">Humidity Limit (DHT11)</div>
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
      <div class="lbl" style="margin-top:12px">Limit (humidityLimit)</div>
      <div class="sp-row">
        <input type="number" id="humLimitInput" step="5" min="0" max="100" value="65" oninput="onHumLimitChange()">
        <span class="sp-unit">% RH</span>
      </div>
      <div class="sp-status" id="humLimitStatus">&nbsp;</div>
    </div>
  </div>

  <script>
    var spTimer   = null;
    var aqTimer   = null;
    var humTimer  = null;
    var settingsSynced = false;

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

    function onSpChange(){
      clearTimeout(spTimer);
      const st = document.getElementById('spStatus');
      st.className = 'sp-status sending'; st.innerText = 'pending...';
      spTimer = setTimeout(sendSp, 600);
    }
    function sendSp(){
      fetch('/set?sp='+document.getElementById('spInput').value).then(r=>r.json()).then(()=>{
        const st = document.getElementById('spStatus');
        st.className = 'sp-status ok'; st.innerText = '✓ applied';
        setTimeout(()=>{st.className='sp-status';st.innerHTML='&nbsp;';}, 1800);
      }).catch(()=>{ document.getElementById('spStatus').innerText='error'; });
    }

    function onAqLimitChange(){
      clearTimeout(aqTimer);
      const st = document.getElementById('aqLimitStatus');
      st.className = 'sp-status sending'; st.innerText = 'pending...';
      aqTimer = setTimeout(sendAqLimit, 600);
    }
    function sendAqLimit(){
      fetch('/set?aqLimit='+document.getElementById('aqLimitInput').value).then(r=>r.json()).then(()=>{
        const st = document.getElementById('aqLimitStatus');
        st.className = 'sp-status ok'; st.innerText = '✓ applied';
        setTimeout(()=>{st.className='sp-status';st.innerHTML='&nbsp;';}, 1800);
      }).catch(()=>{ document.getElementById('aqLimitStatus').innerText='error'; });
    }

    function onHumLimitChange(){
      clearTimeout(humTimer);
      const st = document.getElementById('humLimitStatus');
      st.className = 'sp-status sending'; st.innerText = 'pending...';
      humTimer = setTimeout(sendHumLimit, 600);
    }
    function sendHumLimit(){
      fetch('/set?humLimit='+document.getElementById('humLimitInput').value).then(r=>r.json()).then(()=>{
        const st = document.getElementById('humLimitStatus');
        st.className = 'sp-status ok'; st.innerText = '✓ applied';
        setTimeout(()=>{st.className='sp-status';st.innerHTML='&nbsp;';}, 1800);
      }).catch(()=>{ document.getElementById('humLimitStatus').innerText='error'; });
    }

    setInterval(()=>{
      fetch('/status').then(r=>r.json()).then(d=>{
        document.getElementById('tempVal').innerText        = d.temp.toFixed(1)+' °C';
        document.getElementById('pressVal').innerText       = d.pressure.toFixed(1)+' hPa';
        document.getElementById('currentVal').innerText     = d.current.toFixed(2)+' A';
        document.getElementById('outsideTempVal').innerText = d.outsideOK ? d.outsideTemp.toFixed(1)+' °C' : 'N/A';
        document.getElementById('humVal').innerText         = d.humidity.toFixed(1)+' %';

        // Humidity card
        const hum = d.humidity, hlim = d.humLimit;
        document.getElementById('humValLarge').innerText  = hum.toFixed(1);
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

        // Air quality
        const mq = d.mq135, lim = d.aqLimit;
        document.getElementById('aqVal').innerText = mq;
        document.getElementById('aqLimitLabel').innerText = 'limit: ' + lim;
        const pct = Math.min(100, (mq / 4095) * 100).toFixed(1);
        const bar = document.getElementById('aqBar');
        bar.style.width = pct + '%';
        const aqSt = document.getElementById('aqStatus');
        if (mq > lim) {
          bar.style.background = '#ff4444';
          document.getElementById('aqVal').style.color = '#ff4444';
          aqSt.style.color = '#ff4444'; aqSt.innerText = '⚠ BAD';
        } else if (mq > lim - 300) {
          bar.style.background = '#ffaa00';
          document.getElementById('aqVal').style.color = '#ffaa00';
          aqSt.style.color = '#ffaa00'; aqSt.innerText = '~ WARNING';
        } else {
          bar.style.background = '#00e5ff';
          document.getElementById('aqVal').style.color = '#e0e0e0';
          aqSt.style.color = '#00ff88'; aqSt.innerText = '✓ GOOD';
        }

        if(!settingsSynced){
          settingsSynced = true;
          document.getElementById('spInput').value       = d.sp.toFixed(1);
          document.getElementById('aqLimitInput').value  = d.aqLimit;
          document.getElementById('humLimitInput').value = d.humLimit;
          document.getElementById('delta').value        = d.delta.toFixed(1);
          document.getElementById('peltierPower').value = d.peltierPower;
          document.getElementById('fanPWM').value       = d.fanPWM;
          document.getElementById('pv').innerText       = d.peltierPower;
          document.getElementById('fv').innerText       = d.fanPWM;
        }
        const b = document.getElementById('modeBox');
        b.innerText = modeLabel[d.mode] || d.mode;
        b.className = 'mode-box ' + d.mode;
      }).catch(()=>{});
    }, 1000);
  </script>
)=====";

void handleStatus() {
  const char* modeStr = currentMode == HEATING      ? "HEATING"
                      : currentMode == COOLING      ? "COOLING"
                      : currentMode == FREE_COOLING ? "FREECOOL"
                      : currentMode == FREE_HEATING ? "FREEHEAT"
                      : "IDLE";

  String json = "{\"temp\":"         + String(currentTemp, 2) +
                ",\"pressure\":"     + String(currentPressure, 2) +
                ",\"current\":"      + String(currentAmps, 2) +
                ",\"outsideTemp\":"  + String(outsideTemp, 2) +
                ",\"outsideOK\":"    + String(outsideTemperatureSensorOK ? "true" : "false") +
                ",\"mq135\":"        + String(mq135Raw) +
                ",\"aqLimit\":"      + String(airQualityLimits) +
                ",\"humidity\":"     + String(currentHumidity, 1) +
                ",\"humLimit\":"     + String(humidityLimit) +
                ",\"sp\":"           + String(SP, 2) +
                ",\"delta\":"        + String(DELTA, 2) +
                ",\"peltierPower\":" + String(peltierPower) +
                ",\"fanPWM\":"       + String(fanPower) +
                ",\"mode\":\""       + modeStr + "\"}";
  server.send(200, "application/json", json);
}
