#include "WebPageCtrl.h"
#include "WebPage.h"
#include "Motors.h"
#include "Encoders.h"

extern int SPEED;
extern float targetSpeed;
extern float dpwmFL, dpwmFR, dpwmBL, dpwmBR;
extern float rightside, leftside;
extern bool autoMode;
extern volatile float frontDistance, leftDistance, rightDistance;
extern float pitch;

void setupWebServer(WebServer &server) {
  //starts the server
  server.on("/", [&server]() {
    server.send(200, "text/html", webpage);
  });

  // causes movement when buttons pressed. checks that auto mode is not on first
  server.on("/forward", [&server](){
  if(autoMode){ server.send(403,"text/plain","AUTO"); return; }
  forward(SPEED);
  server.send(200,"text/plain","OK");
  });

  server.on("/rightturn", [&server](){
    if(autoMode){ server.send(403,"text/plain","AUTO"); return; }
    rightturn(SPEED);
    server.send(200,"text/plain","OK");
  });

  server.on("/leftturn", [&server](){
    if(autoMode){ server.send(403,"text/plain","AUTO"); return; }
    leftturn(SPEED);
    server.send(200,"text/plain","OK");
  });

  server.on("/backwards", [&server](){
    if(autoMode){ server.send(403,"text/plain","AUTO"); return; }
    backward(SPEED);
    server.send(200,"text/plain","OK");
  });

  server.on("/Stop", [&server](){
    if(autoMode){ server.send(403,"text/plain","AUTO"); return; }
    stop();
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
    json += "\"PWMFL\":" + String(dpwmFL,1) + ",";
    json += "\"PWMFR\":" + String(dpwmFR,1) + ",";
    json += "\"PWMBL\":" + String(dpwmBL,1) + ",";
    json += "\"PWMBR\":" + String(dpwmBR,1) + ",";
    json += "\"TargetSpeed\":" + String(targetSpeed,1) + ",";
    json += "\"FL\":" + String(fl,1) + ",";
    json += "\"FR\":" + String(fr,1) + ",";
    json += "\"BL\":" + String(bl,1) + ",";
    json += "\"BR\":" + String(br,1) + ",";
    json += "\"RIGHTAVG\":" + String(rightside,1) + ",";
    json += "\"LEFTAVG\":" + String(leftside,1);
    // json += "\"pitch\":" + String(pitch,1);
    json += "}";
    server.send(200, "application/json", json);
  });

  //update auto mode
  server.on("/toggleAuto", [&server](){
    autoMode = !autoMode;
    if(autoMode){
      server.send(200, "text/plain", "ON");
    } else {
      server.send(200, "text/plain", "OFF");
    }
  });
}
