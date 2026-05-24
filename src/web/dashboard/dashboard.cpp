#include "dashboard.h"
#include <Arduino.h>
#include "config/config.h"

const char DASHBOARD_HTML[] PROGMEM = R"=====(
  <!-- DASHBOARD PAGE -->
  <div class="page active" id="dash">
    <div class="mode-box IDLE" id="modeBox">IDLE</div>
    <div class="metrics">
      <div class="metric">
        <div class="lbl">Temperature</div>
        <div class="val hi" id="tempVal">--</div>
      </div>
      <div class="metric">
        <div class="lbl">Pressure</div>
        <div class="val" id="pressVal">--</div>
      </div>
    </div>
    <div class="sp-card">
      <div class="lbl">Setpoint</div>
      <div class="sp-row">
        <input type="number" id="spInput" step="0.5" value="22" oninput="onSpChange()">
        <span class="sp-unit">&#176;C</span>
      </div>
      <div class="sp-status" id="spStatus">&nbsp;</div>
    </div>
  </div>

  <script>
    var spTimer = null;
    var settingsSynced = false;

    function showPage(id, btn){
      document.querySelectorAll('.page').forEach(p=>p.classList.remove('active'));
      document.querySelectorAll('nav button').forEach(b=>b.classList.remove('active'));
      document.getElementById(id).classList.add('active');
      btn.classList.add('active');
    }

    function onSpChange(){
      clearTimeout(spTimer);
      const st = document.getElementById('spStatus');
      st.className = 'sp-status sending';
      st.innerText = 'pending...';
      spTimer = setTimeout(sendSp, 600);
    }

    function sendSp(){
      const val = document.getElementById('spInput').value;
      fetch('/set?sp='+val).then(r=>r.json()).then(()=>{
        const st = document.getElementById('spStatus');
        st.className = 'sp-status ok';
        st.innerText = '\u2713 applied';
        setTimeout(()=>{st.className='sp-status';st.innerHTML='&nbsp;';}, 1800);
      }).catch(()=>{
        const st = document.getElementById('spStatus');
        st.className = 'sp-status';
        st.innerText = 'error';
      });
    }

    setInterval(()=>{
      fetch('/status').then(r=>r.json()).then(d=>{
        document.getElementById('tempVal').innerText  = d.temp.toFixed(1)+' \u00b0C';
        document.getElementById('pressVal').innerText = d.pressure.toFixed(1)+' hPa';
        if(!settingsSynced){
          settingsSynced = true;
          document.getElementById('spInput').value    = d.sp.toFixed(1);
          document.getElementById('delta').value      = d.delta.toFixed(1);
          document.getElementById('peltierPower').value = d.peltierPower;
          document.getElementById('fanPWM').value     = d.fanPWM;
          document.getElementById('pv').innerText     = d.peltierPower;
          document.getElementById('fv').innerText     = d.fanPWM;
        }
        const b = document.getElementById('modeBox');
        b.innerText = d.mode;
        b.className = 'mode-box ' + d.mode;
      }).catch(()=>{});
    }, 1000);
  </script>
)=====";

void handleStatus() {
  const char* modeStr = currentMode == HEATING ? "HEATING"
                      : currentMode == COOLING ? "COOLING"
                      : "IDLE";
  String json = "{\"temp\":"+String(currentTemp, 2)+
                ",\"pressure\":"+String(currentPressure, 2)+
                ",\"sp\":"+String(SP, 2)+
                ",\"delta\":"+String(DELTA, 2)+
                ",\"peltierPower\":"+String(peltierPower)+
                ",\"fanPWM\":"+String(fanPower)+
                ",\"mode\":\""+modeStr+"\"}";
  server.send(200, "application/json", json);
}