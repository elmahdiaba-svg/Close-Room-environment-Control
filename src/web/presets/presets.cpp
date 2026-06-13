#include "presets.h"
#include <Arduino.h>

const char PRESETS_HTML[] PROGMEM = R"=====(
  <!-- PRESETS PAGE -->
  <div class="page" id="presets">
    <div class="card">
      <h3>Power Presets</h3>
      <div style="display:flex;flex-direction:column;gap:8px">
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
      if (pSlider) { pSlider.value = pPct; document.getElementById('pv').innerText = pPct + '%'; }
      if (fSlider) { fSlider.value = fPct; document.getElementById('fv').innerText = fPct + '%'; }
      fetch('/set?peltierPower='+pPWM+'&fanPWM='+fPWM).then(r=>r.json()).then(()=>{
        const t = document.getElementById('toast');
        t.classList.add('show');
        setTimeout(()=>t.classList.remove('show'), 2200);
      });
    }
  </script>
)=====";
