#include "AutoOTA_config.h"
#include "AutoOTA.h"

#define LED 2


AutoOTA ota(serverRootCA);

void setup() {

  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Add your sketch setup code


  // AutoOTA code here
  Serial.printf("\nCurrent sketch version: %s\n", currentVersion);

  if(!ota.begin(host.c_str(), port)) return;    // connection failed

  int status = ota.available(sketchVersionFile, currentVersion);
  switch (status) {
    case 0:
        Serial.println("Already has the latest firmware");
        break;
    case 1: {
          Serial.println("Updating firmware... ");
          // ota.update(sketchBinaryFile);        // if your board doesn't have an LED
          ota.update(sketchBinaryFile, LED);
          break;
        }
    default:
        Serial.printf("HTTP error: %d\n", status);
        break;
  }

}

void loop() {
  // Add your sketch code here
}
