#include <Arduino.h>
#include <MCreatorLink.h>

void setupMC(void) {
  Serial.begin(115200);
  Serial.setTimeout(20);
  MCreatorLink.setup(Serial, "ESP32");
}

void loopMC(void) {
  MCreatorLink.loop();
}
