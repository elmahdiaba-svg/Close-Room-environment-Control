#include "web/webhandler/webhandler.h"
#include "web/dashboard/dashboard.h"
#include "web/settings/settings.h"
#include "config/config.h"
#include <Arduino.h>

// shared CSS and nav, wraps both pages together
const char SHELL_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Peltier Controller</title>
  <style>
    *{box-sizing:border-box;margin:0;padding:0}
    body{font-family:'Courier New',monospace;background:#0d0d0d;color:#e0e0e0;min-height:100vh}
    nav{display:flex;border-bottom:1px solid #2a2a2a;background:#111}
    nav button{
      flex:1;padding:14px;background:none;border:none;color:#666;
      font-family:inherit;font-size:.85em;letter-spacing:.12em;text-transform:uppercase;
      cursor:pointer;transition:color .2s,border-bottom .2s;border-bottom:2px solid transparent
    }
    nav button.active{color:#00e5ff;border-bottom:2px solid #00e5ff}
    nav button:hover:not(.active){color:#aaa}
    .page{display:none;padding:24px 20px;max-width:480px;margin:0 auto;animation:fadeIn .25s ease}
    .page.active{display:block}
    @keyframes fadeIn{from{opacity:0;transform:translateY(6px)}to{opacity:1;transform:none}}
    .mode-box{
      text-align:center;padding:18px;border-radius:4px;font-size:1.8em;
      font-weight:700;letter-spacing:.08em;margin-bottom:16px;
      border:1px solid #222;transition:background .4s,color .4s
    }
    .IDLE{background:#1a1a1a;color:#555;border-color:#2a2a2a}
    .HEATING{background:#1a0a00;color:#ff6d00;border-color:#ff6d0044}
    .COOLING{background:#001a2a;color:#00e5ff;border-color:#00e5ff44}
    .FREECOOL{background:#001a10;color:#00ff88;border-color:#00ff8844}
    .FREEHEAT{background:#1a0f00;color:#ffaa00;border-color:#ffaa0044}
    .metrics{display:grid;grid-template-columns:1fr 1fr;gap:12px;margin-bottom:16px}
    .metric{background:#111;border:1px solid #222;border-radius:4px;padding:14px;text-align:center}
    .metric .lbl{font-size:.7em;letter-spacing:.15em;color:#555;text-transform:uppercase;margin-bottom:6px}
    .metric .val{font-size:1.25em;color:#e0e0e0}
    .metric .val.hi{color:#00e5ff}
    .sp-card{background:#111;border:1px solid #2a2a2a;border-radius:4px;padding:16px;margin-bottom:16px}
    .sp-card .lbl{font-size:.7em;letter-spacing:.15em;color:#555;text-transform:uppercase;margin-bottom:10px}
    .sp-row{display:flex;align-items:center;gap:10px}
    .sp-row input[type=number]{
      flex:1;padding:10px 12px;background:#0d0d0d;border:1px solid #2a2a2a;
      border-radius:3px;color:#00e5ff;font-family:inherit;font-size:1.4em;
      font-weight:700;outline:none;text-align:center;transition:border-color .2s
    }
    .sp-row input[type=number]:focus{border-color:#00e5ff}
    .sp-unit{color:#555;font-size:.9em;letter-spacing:.08em;white-space:nowrap}
    .sp-hint{font-size:.62em;letter-spacing:.06em;color:#444;margin-top:4px}
    .sp-status{font-size:.7em;letter-spacing:.1em;margin-top:6px;min-height:1.2em;transition:color .3s;color:#555}
    .sp-status.sending{color:#ff9800}
    .sp-status.ok{color:#00e5ff}
    .apply-mini{
      padding:8px 12px;background:#1a1a1a;border:1px solid #2a2a2a;border-radius:3px;
      color:#00e5ff;font-size:1.1em;cursor:pointer;flex-shrink:0;transition:background .2s
    }
    .apply-mini:hover{background:#002a2a}
    .apply-mini:active{background:#004444}
    .notaus-btn{
      width:100%;padding:15px;background:#2a0000;border:2px solid #cc0000;
      border-radius:4px;color:#ff4444;font-family:inherit;font-size:.9em;
      letter-spacing:.18em;text-transform:uppercase;cursor:pointer;font-weight:700;
      transition:background .2s,box-shadow .2s,color .2s;margin-bottom:16px
    }
    .sensor-alert{
      display:none;align-items:center;gap:6px;background:#2a1c00;border:1px solid #cc8800;
      color:#ffb300;border-radius:4px;padding:11px 14px;margin-bottom:16px;
      font-size:.72em;letter-spacing:.07em;line-height:1.5;
      animation:pulseAlert 1.6s ease-in-out infinite
    }
    .sensor-alert.show{display:flex}
    @keyframes pulseAlert{0%,100%{box-shadow:0 0 0 #ffb30000}50%{box-shadow:0 0 12px #ffb30055}}
    .notaus-btn:hover{background:#3a0000;box-shadow:0 0 14px #ff000055}
    .notaus-btn.active{background:#cc0000;color:#fff;border-color:#ff4444;box-shadow:0 0 18px #ff000099}
    .card{background:#111;border:1px solid #222;border-radius:4px;padding:20px;margin-bottom:16px}
    .card h3{font-size:.75em;letter-spacing:.2em;text-transform:uppercase;color:#555;margin-bottom:16px}
    .field{margin-bottom:14px}
    label{display:block;font-size:.75em;letter-spacing:.12em;text-transform:uppercase;color:#666;margin-bottom:6px}
    input[type=number]{
      width:100%;padding:9px 12px;background:#0d0d0d;border:1px solid #2a2a2a;
      border-radius:3px;color:#e0e0e0;font-family:inherit;font-size:.95em;outline:none;
      transition:border-color .2s
    }
    input[type=number]:focus{border-color:#00e5ff}
    .slider-row{display:flex;align-items:center;gap:12px}
    input[type=range]{
      flex:1;-webkit-appearance:none;height:3px;background:#2a2a2a;border-radius:2px;outline:none
    }
    input[type=range]::-webkit-slider-thumb{
      -webkit-appearance:none;width:16px;height:16px;border-radius:50%;
      background:#00e5ff;cursor:pointer;border:2px solid #0d0d0d
    }
    .rv{min-width:34px;text-align:right;color:#00e5ff;font-size:.95em}
    .preset-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:4px}
    .preset-btn{
      background:#0d0d0d;border:1px solid #2a2a2a;border-radius:3px;
      color:#e0e0e0;padding:12px 8px;cursor:pointer;text-align:center;
      font-family:inherit;transition:border-color .2s,background .2s;width:100%
    }
    .preset-btn:hover{border-color:#00e5ff;background:#001a1a}
    .preset-btn:active{background:#002a2a}
    .preset-name{display:block;font-size:.85em;letter-spacing:.1em;text-transform:uppercase;color:#00e5ff;margin-bottom:3px}
    .preset-info{display:block;font-size:.72em;color:#aaa;letter-spacing:.05em;margin-bottom:2px}
    .preset-pwm{display:block;font-size:.65em;color:#555;letter-spacing:.05em}
    .apply-btn{
      width:100%;padding:13px;background:#00e5ff;color:#0d0d0d;border:none;
      border-radius:3px;font-family:inherit;font-size:.85em;letter-spacing:.15em;
      text-transform:uppercase;cursor:pointer;font-weight:700;transition:background .2s
    }
    .apply-btn:hover{background:#00b8cc}
    .toast{
      position:fixed;bottom:20px;left:50%;transform:translateX(-50%);
      background:#00e5ff;color:#0d0d0d;padding:10px 22px;border-radius:3px;
      font-size:.8em;letter-spacing:.12em;opacity:0;transition:opacity .3s;pointer-events:none
    }
    .toast.show{opacity:1}
    .login-overlay{
      position:fixed;inset:0;background:rgba(0,0,0,.75);display:none;
      align-items:center;justify-content:center;z-index:50
    }
    .login-overlay.show{display:flex}
    .login-box{
      background:#111;border:1px solid #2a2a2a;border-radius:4px;
      padding:28px 24px;width:260px;text-align:center
    }
    .login-box h3{font-size:.85em;letter-spacing:.15em;text-transform:uppercase;color:#00e5ff;margin-bottom:16px}
    .login-box input{
      width:100%;padding:10px 12px;background:#0d0d0d;border:1px solid #2a2a2a;
      border-radius:3px;color:#e0e0e0;font-family:inherit;font-size:.95em;outline:none;margin-bottom:10px
    }
    .login-box input:focus{border-color:#00e5ff}
    .login-box button{
      width:100%;padding:11px;background:#00e5ff;color:#0d0d0d;border:none;
      border-radius:3px;font-family:inherit;font-size:.8em;letter-spacing:.15em;
      text-transform:uppercase;cursor:pointer;font-weight:700;margin-bottom:8px;transition:background .2s
    }
    .login-box button:hover{background:#00b8cc}
    .login-box button.cancel{background:none;border:1px solid #2a2a2a;color:#888}
    .login-box button.cancel:hover{background:#1a1a1a;color:#aaa}
    .login-error{color:#ff4444;font-size:.7em;letter-spacing:.05em;min-height:14px;margin-bottom:6px}
  </style>
</head>
<body>
  <nav>
    <button class="active" onclick="showPage('dash',this)">&#11041; Dashboard</button>
    <button id="navAuthBtn" onclick="handleAuthNav(this)">&#128274; Login</button>
  </nav>
  <div class="toast" id="toast">Settings applied</div>

  <div class="login-overlay" id="loginOverlay">
    <div class="login-box">
      <h3>&#128274; Settings Login</h3>
      <input type="password" id="loginPass" placeholder="Password"
             onkeydown="if(event.key==='Enter')submitLogin()">
      <div class="login-error" id="loginError">&nbsp;</div>
      <button onclick="submitLogin()">Unlock</button>
      <button class="cancel" onclick="closeLogin()">Cancel</button>
    </div>
  </div>
)=====";

const char SHELL_HTML_END[] PROGMEM = R"=====(
</body>
</html>
)=====";

void handleNotAus() {
  emergencyStop = !emergencyStop;
  if (emergencyStop) {
    ledcWrite(MosfetPetelierCh, 0);
    ledcWrite(MosfetFan1Ch, 0);
    ledcWrite(MosfetFan2Ch, 0);
    digitalWrite(HeatingRelay, HIGH);
    digitalWrite(CoolingRelay, HIGH);
    currentMode = IDLE;
    Serial.println("[NOT-AUS] Emergency stop ACTIVE — all outputs OFF.");
  } else {
    Serial.println("[NOT-AUS] Emergency stop CLEARED — control resumed.");
  }
  String json = "{\"emergency\":" + String(emergencyStop ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void handleLogin() {
  bool ok = server.hasArg("pass") && server.arg("pass") == String(settingsPassword);
  if (ok) {
    settingsLoggedIn = true;
    Serial.println("[LOGIN] Settings unlocked.");
  } else {
    Serial.println("[LOGIN] Wrong password attempt.");
  }
  String json = "{\"ok\":" + String(ok ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  lastUserAccess = millis();   // Benutzer öffnet Dashboard -> ACTIVE MODE
  String page = "";
  page += FPSTR(SHELL_HTML);
  page += FPSTR(DASHBOARD_HTML);
  page += FPSTR(SETTINGS_HTML);
  page += FPSTR(SHELL_HTML_END);
  server.send(200, "text/html", page);
}