#include "WebPageCtrl.h"
#include "WebPage.h"
#include "Motors.h"
#include "Encoders.h"

extern int SPEED;

void setupWebServer(WebServer &server) {

  server.on("/", [&server]() {
    server.send(200, "text/html", webpage);
  });

  server.on("/forward", [](){ forward(SPEED); });
  server.on("/rightturn", [](){ rightturn(SPEED); });
  server.on("/leftturn", [](){ leftturn(SPEED); });
  server.on("/backwards", [](){ backward(SPEED); });
  server.on("/Stop", [](){ stop(); });

  server.on("/speed", [&server]() {
    if (server.hasArg("value")) {
      SPEED = server.arg("value").toInt();
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/speeds", [&server]() {
    String json = "{";
    json += "\"FL\":" + String(fl,1) + ",";
    json += "\"FR\":" + String(fr,1) + ",";
    json += "\"BL\":" + String(bl,1) + ",";
    json += "\"BR\":" + String(br,1);
    json += "}";
    server.send(200, "application/json", json);
  });
}
