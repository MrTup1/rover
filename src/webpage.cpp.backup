#include <Arduino.h>
#include "webPage.h"

// --------------------- WEB PAGE -----------------------
//html
const char webpage[] PROGMEM = R"html(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    * { -webkit-user-select: none; -webkit-touch-callout: none; user-select: none; }
    html, body { touch-action: manipulation; }
    
    /* row 2 please keep that stuff, everything with // */
    body { font-family: Arial; text-align: center; margin-top: 40px; }

    /* set up grid 3x3 for buttons */
    .grid {
      display: grid;
      grid-template-columns: 70px 70px 70px 70px; /* 4 columns */
      grid-template-rows: 70px 70px 70px 70px 70px 70px; /* 6 rows */
      gap: 20px;
      justify-content: center;
    }

    /* sets button sizes */
    .btn {
      width: 70px;
      height: 70px;
      border-radius: 20px;
      background: #0080ff;
      color: white;
      border: none;
    }

    /* set slider layout */
    .speed-container {
      margin-top: 60px;
      width: 100%;
      display: flex;
      flex-direction: column;
    }

    /* rotated label */
    .speedLabel {
      font-size: 14px;
      font-weight: bold;
      transform: rotate(90deg); /* makes it readable from the side so when you hold phone like controller */
      transform-origin: center;
      margin-bottom: 40px;
      text-align: center;
    }

    /* big slider */
    #speedSlider {
      width: 90vw;
      height: 30px;
    }

    /* slide speed displays (needed rotating) */
    .sideSpeed {
      font-size: 14px;
      font-weight: bold;
      transform: rotate(90deg);
      transform-origin: center;
      text-align: center;
      justify-self: center;
      align-self: center;
      overflow: visible;
    }

    /* auto mode button. */
    .autoBtn {
      width: 70px;
      height: 70px;
      border-radius: 20px;
      transform: rotate(90deg);
      transform-origin: center;
      border: none;
      color: white;
      background: yellow;
      font-weight: bold;
    }

    /* Target Input Controls */
    .target-container {
    grid-column: span 2;
    display: flex;
    flex-direction: column;
    gap: 10px;
    }

    .input-box {
    width: 80px;
    border-radius: 5px;
    border: 1px solid #ccc;
    padding: 2px;
    }

    .go-btn {
    background: green;
    color: white;
    padding: 8px;
    border-radius: 10px;
    border: none;
    font-weight: bold;
    }

    .autoOn { background: green; }
  </style>
</head>
<body>

<div class='grid' id='driveControls'>
  <div></div>
  <button class='btn' onmousedown='leftturn()' onmouseup='Stop()' ontouchstart='event.preventDefault(); leftturn()' ontouchend='event.preventDefault(); Stop()'></button>
  <div></div>
  <div class='sideSpeed'>
    left dis: <span id='leftDis'>0</span><br>
    forward dis: <span id='forwardDis'>0</span><br>
    Right dis: <span id='rightDis'>0</span><br>
  </div>


  <button class='btn' onmousedown='backwards()' onmouseup='Stop()' ontouchstart='event.preventDefault(); backwards()' ontouchend='event.preventDefault(); Stop()'></button>
  <div></div>
  <button class='btn' onmousedown='forward()' onmouseup='Stop()' ontouchstart='event.preventDefault(); forward()' ontouchend='event.preventDefault(); Stop()'></button>
  <div></div>


  <div></div>
  <button class='btn' onmousedown='rightturn()' onmouseup='Stop()' ontouchstart='event.preventDefault(); rightturn()' ontouchend='event.preventDefault(); Stop()'></button>
  <div></div>
  <div></div>


  <div></div>
  <div class='sideSpeed'>
    PIDFL: <span id='dpwmFL'>0</span><br>
    PIDFR: <span id='dpwmFR'>0</span><br>
    PIDBL: <span id='dpwmBL'>0</span><br>
    PIDBR: <span id='dpwmBR'>0</span><br>
  </div>
  <div class='sideSpeed'>
    R-avg: <span id='rightside'>0</span>" " L-AVG: <span id='leftside'>0</span><br>
    FL: <span id='fl'>0</span>" " FR: <span id='fr'>0</span><br>
    L: <span id='bl'>0</span>" " BR: <span id='br'>0</span><br>
  </div>
  <button class='autoBtn' onclick='toggleAuto()'>AUTO: OFF</button>


  <div class='speed-container'>
    <input id='speedSlider' type='range' min='0' max='255' value='100' oninput='updateSpeed(this.value)'>
  </div>
  <div></div>
  <div></div>
  <div class='speedLabel'>Speed: <span id='speedValue'>100</span></div>


  <div class = 'sideSpeed' >
    Global X: <span id='globalX'>0</span><br>
    Global Y: <span id='globalY'>0</span><br>
  </div>
  <div class='sideSpeed'>
    TargetSpeed: <span id='targetSpeed'>0</span><br>
  </div>
  <div class = 'sideSpeed'>
    Destination Reached: <span id = 'destinationReached'>false</span><br>
  </div>
  <div class='sideSpeed'>
    Pitch: <span id='pitch'>0</span><br>
  </div>
  
  
  <div class = 'sideSpeed'>
        <button class="go-btn" onclick="sendTarget()">GO TO TARGET</button>
  </div>
  <div class = 'sideSpeed'>
        <label>Target Angle (°): 
        <input type="number" id="inputAngle" class="input-box" step="any">
        </label>
  </div>
  <div class = 'sideSpeed'>
        <label>Target Dist (mm): 
        <input type="number" id="inputDist" class="input-box" step="any">
        </label>
  </div>
  <div></div>


</div>

<script>
  // ----------------------------- Javascript ------------------------------------

  // speed update from scroller
  function updateSpeed(val) {
    document.getElementById('speedValue').textContent = val;
    fetch('/speed?value=' + val);
  }

  // only allow one button to be pressed at once
  let busy = false;
  
  function forward() {
    // checks if another buttons is being pressed, if so doent let other work
    if(busy) return;
    busy = true;
    fetch('/forward');
  }
  
  function rightturn() {
    if(busy) return;
    busy = true;
    fetch('/rightturn');
  }
  
  function leftturn() {
    if(busy) return;
    busy = true;
    fetch('/leftturn');
  }
  
  function backwards() {
    if(busy) return;
    busy = true;
    fetch('/backwards');
  }
  
  function Stop() {
    // Ensures rover is actually moving, prevent overlaps
    if(!busy) return;
    busy = false;
    fetch('/Stop');
  }

  // disable scrolling
  document.querySelectorAll('.btn').forEach(btn => {
    btn.addEventListener('touchmove', e => { e.preventDefault(); }, { passive: false });
  });

  // release anywhere on page activates stop 
  // removed fetch
  document.addEventListener('touchend', function(){ Stop(); });
  document.addEventListener('mouseup', function(){ Stop(); });

  setInterval(() => {
    fetch('/speeds')
      .then(r => r.json())
      .then(data => {
        document.getElementById('leftDis').textContent  = data.leftDistance.toFixed(1);
        document.getElementById('forwardDis').textContent  = data.frontDistance.toFixed(1);
        document.getElementById('rightDis').textContent  = data.rightDistance.toFixed(1);
        document.getElementById('dpwmFL').textContent  = data.PWMFL.toFixed(1);
        document.getElementById('dpwmFR').textContent  = data.PWMFR.toFixed(1);
        document.getElementById('dpwmBL').textContent  = data.PWMBL.toFixed(1);
        document.getElementById('dpwmBR').textContent  = data.PWMBR.toFixed(1);
        document.getElementById('targetSpeed').textContent  = data.TargetSpeed.toFixed(1);
        document.getElementById('fl').textContent  = data.FL.toFixed(1);
        document.getElementById('fr').textContent = data.FR.toFixed(1);
        document.getElementById('bl').textContent  = data.BL.toFixed(1);
        document.getElementById('br').textContent = data.BR.toFixed(1);
        document.getElementById('rightside').textContent  = data.RIGHTAVG.toFixed(1);
        document.getElementById('leftside').textContent = data.LEFTAVG.toFixed(1);
        // document.getElementById('pitch').textContent = data.pitch.toFixed(1);
        document.getElementById('pitch').textContent = data.Pitch.toFixed(1);
        document.getElementById('destinationReached').textContent = data.destinationReached.toFixed(1);
        document.getElementById('globalX').textContent = data.globalX.toFixed(1);
        document.getElementById('globalY').textContent = data.globalY.toFixed(1);
      });
  }, 200);

  // send target angle and distance to rover
  function sendTarget() {
    const angle = document.getElementById('inputAngle').value;
    const dist  = document.getElementById('inputDist').value;
    if (angle === '' || dist === '') {
      alert('Please enter both a target angle and distance.');
      return;
    }
    fetch('/setTarget?angle=' + angle + '&dist=' + dist) // HTTPS get request
      .then(r => r.text())
      .then(msg => alert(msg))
      .catch(() => alert('No response from rover — is it connected?'));
  }

  // auto mode button 
  function toggleAuto() {
    fetch('/toggleAuto')
      .then(response => response.text())
      .then(state => {
        const btn = document.querySelector('.autoBtn');
        if(state === 'ON'){
          btn.style.backgroundColor = 'green';
          btn.textContent = 'AUTO: ON';
        } else {
          btn.style.backgroundColor = 'red';
          btn.textContent = 'AUTO: OFF';
        }
      });
  }
</script>
</body>
</html>
)html";