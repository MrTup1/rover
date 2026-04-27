#include "webPageCtrl.h"
#include "webPage.h"
#include "motors.h"
#include "encoders.h"
#include "auto.h"
#include "return.h"
#include "IMU.h"

extern int SPEED;
extern float targetSpeed;
extern float dpwmFL, dpwmFR, dpwmBL, dpwmBR;
extern float rightside, leftside;
extern bool autoMode;
extern bool navigationMode;
extern bool returnMode;
extern bool recording;
extern volatile float frontDistance, leftDistance, rightDistance;
extern float lefttargetSpeed, righttargetspeed;
extern float Direction;
extern int moveCount;
extern float targetDistance;
extern float targetAngle;
extern float desiredHeading;
extern float heading;
extern int inputState;
extern float globalX; 
extern float globalY; 


String stateStr; // for displaying state of the rover when in auto mode
String modeStr; // for displaying the mode the rover is in.


 

void setupWebServer(WebServer &server) {
  //starts the server
  server.on("/", [&server]() {
    server.send(200, "text/html", webpage);
  });

  // causes movement when buttons pressed. checks that auto mode is not on first
  server.on("/forward", [&server](){
  if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
  forward(SPEED);
  server.send(200,"text/plain","OK");
  });

  server.on("/rightturn", [&server](){
    if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
    rightturn(SPEED);
    server.send(200,"text/plain","OK");
  });

  server.on("/leftturn", [&server](){
    if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
    leftturn(SPEED);
    server.send(200,"text/plain","OK");
  });

  server.on("/backwards", [&server](){
    if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
    backward(SPEED);
    server.send(200,"text/plain","OK");
  });

  server.on("/Stop", [&server](){
    if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
    stop();
    server.send(200,"text/plain","OK");
  });

  server.on("/forwardrightturn", [&server](){
  if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
  forwardrightturn(SPEED);
  server.send(200,"text/plain","OK");
  });

  server.on("/forwardleftturn", [&server](){
  if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
  forwardleftturn(SPEED);
  server.send(200,"text/plain","OK");
  });

  server.on("/backwardsrightturn", [&server](){
    if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
    backwardsrightturn(SPEED);
    server.send(200,"text/plain","OK");
  });

  server.on("/backwardsleftturn", [&server](){
    if(mode != FREEDRIVE){ server.send(403,"text/plain","AUTO"); return; }
    backwardsleftturn(SPEED);
    server.send(200,"text/plain","OK");
  });

  //speed slider and update speed.
  server.on("/speed", [&server]() {
    if (server.hasArg("value")) {
      SPEED = server.arg("value").toInt();
    }
    server.send(200, "text/plain", "OK");
  });

  // updates all lables and displays
  server.on("/speeds", [&server]() {
    String json = "{";
    json += "\"leftDistance\":" + String(leftDistance,1) + ",";
    json += "\"frontDistance\":" + String(frontDistance,1) + ",";
    json += "\"rightDistance\":" + String(rightDistance,1) + ",";
    // json += "\"PWMFL\":" + String(dpwmFL,1) + ",";
    // json += "\"PWMFR\":" + String(dpwmFR,1) + ",";
    // json += "\"PWMBL\":" + String(dpwmBL,1) + ",";
    // json += "\"PWMBR\":" + String(dpwmBR,1) + ",";
    json += "\"TargetSpeed\":" + String(targetSpeed,1) + ",";
    json += "\"LeftTargetSpeed\":" + String(lefttargetSpeed,1) + ",";
    json += "\"RightTargetSpeed\":" + String(righttargetspeed,1) + ",";
    json += "\"FL\":" + String(fl,1) + ",";
    json += "\"FR\":" + String(fr,1) + ",";
    json += "\"BL\":" + String(bl,1) + ",";
    json += "\"BR\":" + String(br,1) + ",";
    // json += "\"RIGHTAVG\":" + String(rightside,1) + ",";
    // json += "\"LEFTAVG\":" + String(leftside,1) + ",";
    json += "\"YAW\":" + String(Direction,1) + ",";
    json += "\"MOVECOUNT\":" + String(moveCount) + ",";
    json += "\"globalX\":" + String(globalX) + ","; 
    json += "\"globalY\":" + String(globalY) + ","; 
    json += "\"inputState\":" + String(inputState) + ","; 

    switch(motionState){
      case STOPPED: stateStr = "STOPPED"; break;
      case FORWARD: stateStr = "FORWARD"; break;
      case BACKWARDS: stateStr = "BACKWARDS"; break;
      case LEFTTURN: stateStr = "LEFT TURN"; break;
      case RIGHTTURN: stateStr = "RIGHT TURN"; break;
      case FORWARDLEFT: stateStr = "FORWARD LEFT"; break;
      case FORWARDRIGHT: stateStr = "FORWARD RIGHT"; break;
      case BACKWARDSLEFT: stateStr = "BACKWARDS LEFT"; break;
      case BACKWARDSRIGHT: stateStr = "BACKWARDS RIGHT"; break;
      case OBSTRUCTION: stateStr = "OBSTRUCTION"; break;
      case TURNING_90: stateStr = "TURNING_90"; break;
      case TURNING_180: stateStr = "TURNING_180"; break;
      case WAITING: stateStr = "WAITING"; break;
      case STARTRETURN: stateStr = "START RETURN"; break;

      default: stateStr = "UNKNOWN";
    }

    json += "\"MOTIONSTATE\":\"" + stateStr + "\"" + ","; 

    switch(mode){
      case FREEDRIVE: modeStr = "FREE DRIVE"; break;
      case AUTO: modeStr = "AUTO"; break;
      case RETURN: modeStr = "RETURN"; break;
      case NAVIGATION: modeStr = "NAVIGATION"; break;

      default: modeStr = "UNKNOWN";
    }
    
    json += "\"MODE\":\"" +modeStr + "\""; 

    json += "}";
    server.send(200, "application/json", json);
  });

  //update auto mode
  server.on("/toggleAuto", [&server](){
    if (mode != FREEDRIVE && mode != AUTO) return;
    autoMode = !autoMode;
    if(autoMode){
      mode = AUTO;
      server.send(200, "text/plain", "ON");
      stop();
    } else {
      mode = FREEDRIVE;
      server.send(200, "text/plain", "OFF");
      motionState = STOPPED;
      stop();
    }
  });

  //update navigation mode
  server.on("/toggleNavigation", [&server](){
    if (mode != FREEDRIVE && mode != NAVIGATION) return;
    navigationMode = !navigationMode;
    if(navigationMode){
      mode = NAVIGATION;
      server.send(200, "text/plain", "ON");
      stop();
    } else {
      mode = FREEDRIVE;
      server.send(200, "text/plain", "OFF");
      stop();
    }
  });

  //update return mode
  server.on("/toggleReturn", [&server](){
    if (mode != FREEDRIVE && mode != RETURN) return;
    if (recording) return;
    returnMode = !returnMode;
    if(returnMode){
      mode = RETURN;
      server.send(200, "text/plain", "ON");
      stop();
      motionState = STARTRETURN;
    } else {
      mode = FREEDRIVE;
      moveCount = 0;
      server.send(200, "text/plain", "OFF");
      stop();
    }
  });

  //update RECORDING button
  server.on("/toggleRec", [&server](){
    if (mode == RETURN || motionState != STOPPED) return;
    recording = !recording;
    if(recording){
      server.send(200, "text/plain", "ON");
    } else {
      server.send(200, "text/plain", "OFF");
    }
  });

  // navigation use 
  server.on("/setTarget", [&server](){
    if (!server.hasArg("angle") || !server.hasArg("dist")) {
      server.send(400, "text/plain", "Missing angle or dist parameter");
      return;
    }
    if (inputState == 2 || inputState == 3) {
      server.send(503, "text/plain", "Rover is already moving to a target");
      return;
    }
    targetAngle    = server.arg("angle").toFloat();
    targetDistance = server.arg("dist").toFloat();
    desiredHeading = targetAngle + heading; // translate to IMU-relative heading
    inputState = 2; // jump straight to turning phase
    mode = NAVIGATION;
    server.send(200, "text/plain",
      "Target set: " + String(targetDistance,1) + " mm at " + String(targetAngle,1) + " deg");
  });
}

