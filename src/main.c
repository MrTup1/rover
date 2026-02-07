#include <WiFi.h>
#include <WebServer.h>

// webpage stuff
const char* ssid = "ESP32-Group B rover";
const char* password = "Williscool";
WebServer server(80);

// -------------------- esp32 pins -----------------
// motors
//motor 1 (front left)
#define DIR_FL  13
#define PWM_FL  12
//motor 2 (front right)
#define DIR_FR  25
#define PWM_FR  33
//motor 3 (Back left)
#define DIR_BL  15
#define PWM_BL  2
//motor 4 (Back right)
#define DIR_BR  18
#define PWM_BR  19

// encoders
// (front left A and B enecoders)
#define ENA_FL 14
#define ENB_FL 27
// (front right A and B enecoders)
#define ENA_FR 32
#define ENB_FR 35
// (back left A and B enecoders)
#define ENA_BL 4
#define ENB_BL 16
// (back right A and B enecoders)
#define ENA_BR 22
#define ENB_BR 23

// ----------------- Variables ---------------

int SPEED = 100;

// counts for calculation each motor speed
volatile long encFL = 0;
volatile long encFR = 0;
volatile long encBL = 0;
volatile long encBR = 0;
long lastFL = 0, lastFR = 0, lastBL = 0, lastBR = 0;
float speedLeft = 0;
float speedRight = 0;
unsigned long lastSpeedTime = 0;

// --------------------- WEB PAGE -----------------------
String webpage =
"<!DOCTYPE html><html>"
"<head><meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>"

"*{-webkit-user-select:none;-webkit-touch-callout:none;user-select:none;}"
"html,body{touch-action:manipulation;}"

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
"<div></div>" // means empty space in 3x3 grid
"<button class='btn' onmousedown='leftturn()' onmouseup='Stop()' "
"ontouchstart='leftturn()' ontouchend='Stop()'></button>"
"<div></div>"
"<div></div>"

// row 2
"<button class='btn' onmousedown='backwards()' onmouseup='Stop()' "
"ontouchstart='backwards()' ontouchend='Stop()'></button>"
"<div></div>"
"<button class='btn' onmousedown='forward()' onmouseup='Stop()' "
"ontouchstart='forward()' ontouchend='Stop()'></button>"
"<div></div>"

// row 3
"<div></div>"
"<button class='btn' onmousedown='rightturn()' onmouseup='Stop()' "
"ontouchstart='rightturn()' ontouchend='Stop()'></button>"
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
"<input id='speedSlider' type='range' min='0' max='200' value='100' oninput='updateSpeed(this.value)'>"
"</div>"
"<div></div>"
"<div></div>"
"<div class='speedLabel'>Speed: <span id='speedValue'>100</span></div>"

// row 6
"<div></div>"
"<div></div>"
"<div></div>"
"<div class='sideSpeed'>"
" Left AVG: <span id='leftSpeed'>0</span><br>"
" Right AVG: <span id='rightSpeed'>0</span>"
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
"  busy = false;"
"  fetch('/Stop');"
"}"

// disable scrolling
"document.querySelectorAll('.btn').forEach(btn => {"
  "btn.addEventListener('touchmove', e => { e.preventDefault(); }, { passive:false });"
"});"

// release anywhere on page activates stop 
"document.addEventListener('touchend', function(){ fetch('/Stop'); });"
"document.addEventListener('mouseup', function(){ fetch('/Stop'); });"

"setInterval(() => {"
"  fetch('/speeds')"
"    .then(r => r.json())"
"    .then(data => {"
"      document.getElementById('leftSpeed').textContent  = data.left.toFixed(1);"
"      document.getElementById('rightSpeed').textContent = data.right.toFixed(1);"
"    });"
"}, 200);"

"</script>"

"</body></html>";

// functions
void forward(int speed) { //left and right side forward
  digitalWrite(DIR_FR, LOW);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, LOW);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, LOW);
  analogWrite(PWM_BR, speed);
  digitalWrite(DIR_FL, LOW);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
}
void backward(int speed) { //left and right side backwards
  digitalWrite(DIR_FL, HIGH);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
  digitalWrite(DIR_FR, HIGH);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, HIGH);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, HIGH);
  analogWrite(PWM_BR, speed);
}
void stop() { // both sides stopped
  analogWrite(PWM_FL, 0);
  analogWrite(PWM_FR, 0);
  analogWrite(PWM_BL, 0);
  analogWrite(PWM_BR, 0);
}
void rightturn(int speed) { //left forward and right backwards
  digitalWrite(DIR_FL, LOW);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
  digitalWrite(DIR_FR, HIGH);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, LOW);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, HIGH);
  analogWrite(PWM_BR, speed);
}
void leftturn(int speed) { // left backwards and right forward
  digitalWrite(DIR_FL, HIGH);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
  digitalWrite(DIR_FR, LOW);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, HIGH);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, LOW);
  analogWrite(PWM_BR, speed);
}

void updateleft(int speed) {
  digitalWrite(DIR_FL, HIGH);   
  analogWrite(PWM_FL, speed);
  digitalWrite(DIR_BL, HIGH);   
  analogWrite(PWM_BL, speed); 
}

void updateright(int speed) {
  digitalWrite(DIR_FR, HIGH);   
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BR, HIGH);   
  analogWrite(PWM_BR, speed); 
}


void handleRoot() {
  server.send(200, "text/html", webpage);
}

// ---------------------------------- interupts --------------------------------------
// for taking motor speed. checks sensor B when sensor A is rising showing a change in motor poition. more checks = faster speed. B if HIGH = forward if LOW = backwards
void IRAM_ATTR isrFL() {
  if (digitalRead(ENB_FL)) encFL++;
  else encFL--;
}

void IRAM_ATTR isrFR() {
  if (digitalRead(ENB_FR)) encFR++;
  else encFR--;
}

void IRAM_ATTR isrBL() {
  if (digitalRead(ENB_BL)) encBL++;
  else encBL--;
}

void IRAM_ATTR isrBR() {
  if (digitalRead(ENB_BR)) encBR++;
  else encBR--;
}

// set up
void setup() {
  Serial.begin(115200);
  // set up pins
  pinMode(DIR_FL, OUTPUT); //defines pins
  pinMode(PWM_FL, OUTPUT);
  pinMode(DIR_FR, OUTPUT);
  pinMode(PWM_FR, OUTPUT);
  pinMode(DIR_BL, OUTPUT); //defines pins
  pinMode(PWM_BL, OUTPUT);
  pinMode(DIR_BR, OUTPUT);
  pinMode(PWM_BR, OUTPUT);
  // encoders
  pinMode(ENA_FL, INPUT_PULLUP);
  pinMode(ENB_FL, INPUT_PULLUP);
  pinMode(ENA_FR, INPUT_PULLUP);
  pinMode(ENB_FR, INPUT_PULLUP);
  pinMode(ENA_BL, INPUT_PULLUP);
  pinMode(ENB_BL, INPUT_PULLUP);
  pinMode(ENA_BR, INPUT_PULLUP);
  pinMode(ENB_BR, INPUT_PULLUP);

  // ISR intrupts to count motor speeds
  attachInterrupt(digitalPinToInterrupt(ENA_FL), isrFL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENA_FR), isrFR, RISING);
  attachInterrupt(digitalPinToInterrupt(ENA_BL), isrBL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENA_BR), isrBR, RISING);

  // start WiFi access point
  WiFi.softAP(ssid, password);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());


  // functions from buttons:
  server.on("/forward", [](){ 
    forward(SPEED); 
  });
  server.on("/rightturn", [](){
    rightturn(SPEED);
  });
  server.on("/leftturn", [](){
    leftturn(SPEED);
  });
  server.on("/backwards", [](){
    backward(SPEED);
  });
  server.on("/Stop", [](){
    stop();
  });
  // SPEED update from speed slider
  server.on("/speed", []() {
    if (server.hasArg("value")) {
        SPEED = server.arg("value").toInt(); //used in how fast the rover should be going
    }
    server.send(200, "text/plain", "OK");
  });
  // updates the right and left speed display with rightspeed and leftspeed
  server.on("/speeds", []() {
    String json = "{";
    json += "\"left\":" + String(speedLeft, 1) + ",";
    json += "\"right\":" + String(speedRight, 1);
    json += "}";  
    server.send(200, "application/json", json);
  });

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient(); // recieves button presses and handles inputs

  unsigned long now = millis();
  if (now - lastSpeedTime >= 100) {  
    noInterrupts(); //pauses interputs so you dont get an error reading the interupt if it is updating while trying to read
    long cFL = encFL; // recieves count 
    long cFR = encFR;
    long cBL = encBL;
    long cBR = encBR;
    interrupts();

    long dFL = cFL - lastFL; //works out change in chount since last check
    long dFR = cFR - lastFR;
    long dBL = cBL - lastBL;
    long dBR = cBR - lastBR;

    Serial.printf(
      "FL:%ld  BL:%ld  FR:%ld  BR:%ld\n",
      dFL, dBL, dFR, dBR
    );

    lastFL = cFL; // updates last count to be this loop
    lastFR = cFR;
    lastBL = cBL;
    lastBR = cBR;

    // ticks per second
    float fl = dFL * 10.0; //checks every 0.1 second so speed per second = *10
    float fr = dFR * 10.0;
    float bl = dBL * 10.0;
    float br = dBR * 10.0;

    //left side is mirrored so a high B = backwards and low = forwards ++, so flip count ++ = --
    long fl_signed = -fl;
    long bl_signed = -bl;

    // side averages
    speedLeft  = (fl_signed + bl_signed) / 2.0;
    speedRight = (fr + br) / 2.0;

    lastSpeedTime = now; //updates time
  }
}