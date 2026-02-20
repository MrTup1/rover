#include <Arduino.h>
#include "webPage.h"

// --------------------- WEB PAGE -----------------------
const char webpage[] PROGMEM =
"<!DOCTYPE html><html>"
"<head><meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>"

"*{-webkit-user-select:none;-webkit-touch-callout:none;user-select:none;}"
"html,body{touch-action:manipulation;}"
// row 2 please keep that stuff, everything with //
"body{font-family:Arial;text-align:center;margin-top:40px;}"

// set up grid 3x3 for buttons
".grid{"
"  display:grid;"
"  grid-template-columns:70px 70px 70px 70px;" // 4 coloums
"  grid-template-rows:70px 70px 70px 70px 70px 70px;" // 6 rows
"  gap:20px;"
"  justify-content:center;"
"}"

// sets button sizes 
".btn{"
"  width:70px;"
"  height:70px;"
"  border-radius:20px;"
"  background:#0080ff;"
"  color:white;"
"  border:none;"
"}"

// set slider layput
".speed-container{"
"  margin-top:60px;"
"  width:100%;"
"  display:flex;"
"  flex-direction:column;"
"}"

// rotated label 
".speedLabel{"
"  font-size:14px;"
"  font-weight:bold;"
"  transform:rotate(90deg);" // makes it readable from the side so when you hold phone like controller
"  transform-origin:center;"
"  margin-bottom:40px;"
"  text-align:center;"
"}"

/* big slider */
"#speedSlider{"
"  width:90vw;"
"  height:30px;"
"}"

// slide speed displays (needed rotating)
".sideSpeed{"
"  font-size:14px;"
"  font-weight:bold;"
"  transform:rotate(90deg);"
"  transform-origin:center;"
"  text-align:center;"
"  justify-self:center;"
"  align-self:center;"
"}"

"</style>"
"</head>"
"<body>"

// buttons
"<div class='grid'>"
// row 1
"<div></div>" 
"<button class='btn' onmousedown='leftturn()' onmouseup='Stop()' "
"ontouchstart='event.preventDefault(); leftturn()' ontouchend='event.preventDefault(); Stop()'></button>"
"<div></div>"
"<div></div>"

// row 2
"<button class='btn' onmousedown='backwards()' onmouseup='Stop()' "
"ontouchstart='event.preventDefault(); backwards()' ontouchend='event.preventDefault(); Stop()'></button>"
"<div></div>"
"<button class='btn' onmousedown='forward()' onmouseup='Stop()' "
"ontouchstart='event.preventDefault(); forward()' ontouchend='event.preventDefault(); Stop()'></button>"
"<div></div>"

// row 3
"<div></div>"
"<button class='btn' onmousedown='rightturn()' onmouseup='Stop()' "
"ontouchstart='event.preventDefault(); rightturn()' ontouchend='event.preventDefault(); Stop()'></button>"
"<div></div>"
"<div></div>"

// row 4
"<div></div>"
"<div></div>"
"<div></div>"
"<div></div>"

// row 5
//slider
"<div class='speed-container'>"
"<input id='speedSlider' type='range' min='0' max='255' value='100' oninput='updateSpeed(this.value)'>"
"</div>"
"<div></div>"
"<div></div>"
"<div class='speedLabel'>Speed: <span id='speedValue'>100</span></div>"

// row 6
"<div></div>"
"<div></div>"
"<div class='sideSpeed'>Pitch: <span id='pitch'>0.0</span>&deg;</div>"
"<div class='sideSpeed'>"
"  <div>FL: <span id='fl'>0.0</span>&nbsp;&nbsp;FR: <span id='fr'>0.0</span></div>"
"  <div>BL: <span id='bl'>0.0</span>&nbsp;&nbsp;BR: <span id='br'>0.0</span></div>"
"</div>"

"</div>"

"<script>"

// speed update from scroller
"function updateSpeed(val){"
"  document.getElementById('speedValue').textContent = val;"
"  fetch('/speed?value=' + val);"
"}"

// only allow one button to be pressed at once
"let busy = false;"
"function forward(){ " //checks if another buttons is being pressed, if so doent let other work
"  if(busy) return;"
"  busy = true;"
"  fetch('/forward');"
"}"
"function rightturn(){ "
"  if(busy) return;"
"  busy = true;"
"  fetch('/rightturn');"
"}"
"function leftturn(){ "
"  if(busy) return;"
"  busy = true;"
"  fetch('/leftturn');"
"}"
"function backwards(){ "
"  if(busy) return;"
"  busy = true;"
"  fetch('/backwards');"
"}"
"function Stop(){ "
"  if(!busy) return;"  //Ensures rover is actually moving, prevent overlaps
"  busy = false;"
"  fetch('/Stop');"
"}"

// disable scrolling
"document.querySelectorAll('.btn').forEach(btn => {"
"  btn.addEventListener('touchmove', e => { e.preventDefault(); }, { passive:false });"
"});"

// release anywhere on page activates stop 
"document.addEventListener('touchend', function(){ Stop(); });" //removed fetch
"document.addEventListener('mouseup', function(){ Stop(); });"

"setInterval(() => {"
"  fetch('/speeds')"
"    .then(r => r.json())"
"    .then(data => {"
"      document.getElementById('fl').textContent  = data.FL.toFixed(1);"
"      document.getElementById('fr').textContent = data.FR.toFixed(1);"
"      document.getElementById('bl').textContent  = data.BL.toFixed(1);"
"      document.getElementById('br').textContent = data.BR.toFixed(1);"
"      document.getElementById('pitch').textContent = data.Pitch.toFixed(1);"
"    });"
"}, 200);"

"</script>"
"</body></html>";
